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

	"go.bug.st/serial"
)

var (
	devicePathFlag string
	pipeFlag       bool
)

func main() {
	flag.StringVar(&devicePathFlag, "d", "", "device path for serial")
	flag.BoolVar(&pipeFlag, "p", true, "")
	flag.Parse()

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)
	doneChan := make(chan struct{})

	if devicePathFlag != "" {
		go func() {
			defer close(doneChan)

			c := &serial.Mode{
				BaudRate: 115200,
				Parity:   serial.EvenParity,
			}
			s, err := serial.Open(devicePathFlag, c)
			if err != nil {
				log.Fatal(err)
			}
			run(s)
		}()
	}
	if devicePathFlag == "" && pipeFlag {
		go func() {
			defer close(doneChan)
			s, err := pipeOpen()
			defer s.Close()
			if err != nil {
				log.Fatal(err)
			}

			run(s)
		}()
	}

	for {
		select {
		case <-sig:
			return
		case <-doneChan:
			return
		}
	}
}

func run(s ReaderWriter) {
	disk, err := newDisk()
	defer disk.close()
	if err != nil {
		log.Fatal(err)
	}
	tokz := newTokenizer(disk)

	for {
		buf := make([]byte, 128)
		n, err := s.Read(buf)
		if err != nil {
			log.Fatal(err)
		}
		for i := 0; i < n; i++ {
			tokz.submit(buf[i])
			if tok, found := tokz.next(); found {
				handleToken(tok, disk, s)
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
