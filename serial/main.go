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

	"github.com/containerd/console"
	"go.bug.st/serial"
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
		c := &serial.Mode{
			BaudRate: 115200,
			Parity:   serial.EvenParity,
		}
		s, err := serial.Open(devicePathFlag, c)
		if err != nil {
			return nil, err
		}
		return s, nil
	}
	if pipeFlag {
		s, err := pipeOpen()
		if err != nil {
			return nil, err
		}
		return s, nil
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
	disk, err := newDisk()
	defer disk.close()
	if err != nil {
		log.Fatal(err)
	}
	tokz := newTokenizer()

	for {
		buf := make([]byte, 128)
		n, err := s.Read(buf)
		if err != nil {
			if err == io.EOF {
				return
			}
			log.Fatal(err)
		}
		for i := 0; i < n; i++ {
			tokz.submit(buf[i])
			if tok, found := tokz.next(); found {
				handleToken(tok, disk, s)
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

func handleToken(tok token, disk *disk, s ReaderWriter) {
	data := tok.data

	if data[0] == 'R' {
		fmt.Println("reading from disk")
		sector, err := getSector(tok.data)
		if err != nil {
			panic(err)
		}
		d, err := disk.read(sector)
		if err != nil {
			panic(err)
		}
		_, err = s.Write(d)
		if err != nil {
			panic(err)
		}
	}
	if data[0] == 'W' {
		fmt.Println("writing to disk")
		sector, err := getSector(tok.data)
		if err != nil {
			panic(err)
		}
		c, err := getContent(tok.data)
		if err != nil {
			panic(err)
		}
		fmt.Printf("content length %d\n", len(c))
		err = disk.write(sector, c)
		if err != nil {
			panic(err)
		}

	}
	if data[0] != 'W' && data[0] != 'R' {
		fmt.Println("invalid data type")
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
}

type pipe struct {
	input  *os.File
	output *os.File
}

func pipeOpen() (*pipe, error) {
	os.Remove("/tmp/fifo_out")
	os.Remove("/tmp/fifo_in")
	err := syscall.Mkfifo("/tmp/fifo_out", 0666)
	if err != nil {
		return nil, err
	}
	err = syscall.Mkfifo("/tmp/fifo_in", 0666)
	if err != nil {
		return nil, err
	}

	fout, err := os.OpenFile("/tmp/fifo_out", os.O_CREATE|os.O_RDONLY, 0666)
	if err != nil {
		return nil, err
	}
	fin, err := os.OpenFile("/tmp/fifo_in", os.O_CREATE|os.O_WRONLY, 0666)
	if err != nil {
		return nil, err
	}
	return &pipe{
		input:  fout,
		output: fin,
	}, err
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
