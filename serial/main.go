package main

import (
	"errors"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/containerd/console"
	"go.bug.st/serial"
	"go.bug.st/serial/enumerator"
)

var (
	devicePathFlag string
	pipeFlag       bool
)

func main() {
	flag.StringVar(&devicePathFlag, "d", "", "device path for serial")
	flag.BoolVar(&pipeFlag, "p", false, "use a pipe for I/O")
	flag.Parse()

	if devicePathFlag == "" && !pipeFlag {
		log.Fatal("one of [-d -p] must be specified")
	}
	if devicePathFlag != "" && pipeFlag {
		log.Fatal("only one of [-d -p] must be specified")
	}

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)
	doneChan := make(chan struct{})
	mainDoneChan := make(chan struct{})

	go func() {
		for {
			select {
			case <-sig:
				close(mainDoneChan)
				return
			case <-doneChan:
				close(mainDoneChan)
				return
			}
		}
	}()

	s, err := getDevice()
	if err != nil {
		log.Fatal(err)
	}
	c, err := getConsole()
	if err != nil {
		log.Fatal(err)
	}
	defer func() {
		c.Reset()
		c.Close()
	}()

	go func() {
		defer close(doneChan)
		defer s.Close()
		run(s)
	}()

	go func() {
		buf := make([]byte, 128)

		for {
			n, err := os.Stdin.Read(buf)
			if err != nil {
				log.Println(err)
				return
			}
			if n >= 1 && buf[0] == 3 {
				// 3 -> CTRL + C
				// terminate program
				sig <- syscall.SIGINT
				return
			}
			s.Write(buf[:n])
		}
	}()

	<-mainDoneChan
}

func getDevice() (ReaderWriter, error) {
	if devicePathFlag != "" {
		return &serialReadWriter{
			devicePath: devicePathFlag,
		}, nil
	}
	if pipeFlag {
		return &pipe{}, nil
	}
	return nil, errors.New("no device available")
}

func getConsole() (c console.Console, err error) {
	c = console.Current()
	defer func() {
		if err != nil {
			c.Reset()
			c.Close()
		}
	}()
	err = c.SetRaw()
	if err != nil {
		return
	}
	var ws console.WinSize
	ws, err = c.Size()
	if err != nil {
		return
	}
	err = c.Resize(ws)
	return
}

func run(s ReaderWriter) {
	logger, logClose := setupLogs()
	defer logClose()
	disk, err := newDisk(logger)
	defer disk.close()
	if err != nil {
		log.Fatal(err)
	}

	tokz := newTokenizer(logger)

	var f = func() error {
		for {
			buf := make([]byte, 128)
			n, err := s.Read(buf)
			if err != nil {
				return err
			}

			for i := 0; i < n; i++ {
				tokz.submit(buf[i])
				if tok, found := tokz.next(); found {
					handleToken(tok, disk, s, logger)
					continue
				}
				b, err := tokz.data()
				if err != nil {
					log.Fatal(err)
				}
				for _, v := range b {
					fmt.Printf("%s", string(v))
				}
			}

		}
	}

	for {
		err = s.Open()
		if err != nil {
			fmt.Println(err)
			return
		}
		err = f()
		portError, isPortError := err.(*serial.PortError)
		if isPortError && portError.Code() == serial.PortClosed {
			fmt.Println("port closed")

			// free device and attempt to reconnect
			s.Close()
			continue
		}
		fmt.Println(err)
		return
	}
}

func setupLogs() (*os.File, func()) {
	// Open the log file in append mode, create it if it doesn't exist
	file, err := os.OpenFile("/tmp/log-disk.log", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatalf("failed to open log file: %v", err)
	}
	return file, nil

	// // Create a logger that writes to the file
	// logger := log.New(file, "prefix: ", log.LstdFlags)
	// return logger, func() {
	// 	file.Close()
	// }
}

func handleToken(tok token, disk *disk, s ReaderWriter, logFile *os.File) {
	data := tok.data

	if data[0] == 'R' {
		fmt.Fprint(logFile, "Received read disk request ")
		sector, err := getSector(tok.data)
		if err != nil {
			panic(err)
		}
		fmt.Fprintf(logFile, "for sector %d\n", sector)
		d, err := disk.read(sector)
		if err != nil {
			panic(err)
		}
		_, err = s.Write(d)
		time.Sleep(time.Millisecond)
		if err != nil {
			panic(err)
		}

		fmt.Fprintf(logFile, "[Serial] done sending\n\n")
	}
	if data[0] == 'W' {
		fmt.Fprint(logFile, "Received write disk request ")
		sector, err := getSector(tok.data)
		if err != nil {
			panic(err)
		}
		fmt.Fprintf(logFile, "for sector %d ", sector)
		c, err := getContent(tok.data)
		if err != nil {
			panic(err)
		}
		fmt.Fprintf(logFile, "content length %d\n\n", len(c))
		err = disk.write(sector, c)
		if err != nil {
			panic(err)
		}

	}
	if data[0] != 'W' && data[0] != 'R' {
		fmt.Fprint(logFile, "invalid data type\n")
		fmt.Printf("%v", data[:min(20, len(data))])
		os.Exit(0)
	}
}

func getSector(data []byte) (uint8, error) {
	return data[1], nil
}

func getContent(data []byte) ([]byte, error) {
	return data[2:], nil
}

type ReaderWriter interface {
	io.Reader
	io.Writer
	io.Closer

	Open() error
}

type serialReadWriter struct {
	devicePath string

	// for reconnection
	serialID string
	port     serial.Port
}

func (t *serialReadWriter) Read(out []byte) (int, error) {
	return t.port.Read(out)
}

func (t *serialReadWriter) Write(b []byte) (int, error) {
	return t.port.Write(b)
}

func (t *serialReadWriter) Close() error {
	if t.port != nil {
		return t.port.Close()
	}
	return nil
}

func (t *serialReadWriter) Open() error {
	if t.serialID == "" {
		fmt.Println("opening")
		return t.open()
	}
	fmt.Println("reopening")
	return t.reopen()
}

func (t *serialReadWriter) reopen() error {
	var portFound bool
	for {
		if portFound {
			break
		}

		time.Sleep(time.Second)

		ports, err := enumerator.GetDetailedPortsList()
		if err != nil {
			fmt.Println("error getting the port list")
			continue
		}
		if len(ports) == 0 {
			fmt.Println("no ports found")
			continue
		}
		for _, v := range ports {
			if v.IsUSB && v.SerialNumber == t.serialID {
				fmt.Printf("%v\n", v)
				fmt.Printf("found device %s\n", v.Name)
				t.devicePath = v.Name
				portFound = true
				break
			}
		}
	}

	return t.open()
}

func (t *serialReadWriter) open() error {
	ports, err := enumerator.GetDetailedPortsList()
	if err != nil {
		return err
	}
	for _, v := range ports {
		if v.IsUSB && v.Name == t.devicePath {
			fmt.Printf("%v\n", v)
			fmt.Printf("saving serial number %s\n", v.SerialNumber)
			t.serialID = v.SerialNumber
			break
		}
	}
	if t.serialID == "" {
		return errors.New("device not found")
	}

	c := &serial.Mode{
		BaudRate: 1200, //115200,
		Parity:   serial.EvenParity,
	}
	s, err := serial.Open(t.devicePath, c)
	if err != nil {
		return err
	}
	t.port = s
	return nil
}

type pipe struct {
	input  *os.File
	output *os.File
}

func (t *pipe) Read(out []byte) (int, error) {
	return t.input.Read(out)
}

func (t *pipe) Write(b []byte) (int, error) {
	return t.output.Write(b)
}

func (t *pipe) Close() error {
	err1 := t.input.Close()
	err2 := t.output.Close()

	return errors.Join(err1, err2)
}

func (t *pipe) Open() error {
	os.Remove("/tmp/fifo_out")
	os.Remove("/tmp/fifo_in")
	err := syscall.Mkfifo("/tmp/fifo_out", 0666)
	if err != nil {
		return err
	}
	err = syscall.Mkfifo("/tmp/fifo_in", 0666)
	if err != nil {
		return err
	}

	fout, err := os.OpenFile("/tmp/fifo_out", os.O_CREATE|os.O_RDONLY, 0666)
	if err != nil {
		return err
	}
	fin, err := os.OpenFile("/tmp/fifo_in", os.O_CREATE|os.O_WRONLY, 0666)
	if err != nil {
		return err
	}

	t.input = fout
	t.output = fin

	return nil
}
