package main

import (
	"errors"
	"log"
	"os"
	"os/signal"
	"syscall"
)

func main() {
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)

	s, err := pipeOpen()
	defer s.Close()
	if err != nil {
		log.Fatal(err)
	}

	disk, err := newDisk()
	defer disk.close()
	if err != nil {
		log.Fatal(err)
	}
	tokz := tokenizer{disk: disk}

	for {
		buf := make([]byte, 128)
		n, err := s.Read(buf)
		if err != nil {
			log.Fatal(err)
		}
		for i := 0; i < n; i++ {
			tokz.submit(buf[i])
			if tok, found := tokz.next(); found {
				tokz.handleToken(tok, func(b []byte) error {
					for _, v := range b {
						_, err = s.Write([]byte{byte(v)})
						if err != nil {
							log.Fatal(err)
						}
					}
					return nil
				})
			}
		}
	}
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
