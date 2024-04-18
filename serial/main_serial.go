package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"strconv"
	"strings"
	"syscall"
	"time"

	"go.bug.st/serial"
)

func mainSerial() {
	flag.StringVar(&devicePathFlag, "d", "", "")
	flag.Parse()

	if devicePathFlag == "" {
		//log.Fatal("-d devicePath is required")
		devicePathFlag = "/dev/ttyUSB0"
	}

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)

	c := &serial.Mode{
		BaudRate: 115200,
		//StopBits: 1,
		Parity: serial.EvenParity,
	}
	s, err := serial.Open(devicePathFlag, c)
	if err != nil {
		log.Fatal(err)
	}

	go func() {
		for {
			buf := make([]byte, 128)
			n, err := s.Read(buf)
			if err != nil {
				log.Fatal(err)
			}
			if n != 0 {
				for i := 0; i < n; i++ {
					data := uint8(buf[i])
					//fmt.Printf("bin %08b  ", data)
					//fmt.Printf("dec: %03d  ", data)
					//fmt.Printf("cha: %s\n", string(data))
					fmt.Printf("%s", string(data))
				}
				fmt.Print("\n")
			}
		}
	}()

	msg := make(chan string, 1)
	go func() {
		reader := bufio.NewReader(os.Stdin)
		for {
			s, err := reader.ReadString('\n')
			if err != nil {
				log.Println(err)
				return
			}

			if len(s) > 1 {
				// remove delimiter
				s = s[:len(s)-1]
				msg <- s
			}
		}
		//msg <- "loop"
	}()

loop:
	for {
		select {
		case <-sig:
			fmt.Println("Got shutdown, exiting")
			// Break out of the outer for statement and end the program
			break loop
		case input := <-msg:
			// user can type "close" (in alternative to Ctrl + C) to close the program
			if input == "close" {
				break loop
			}
			if input == "start" {
				for i := 0; i < 20; i++ {
					s.Write([]byte{byte('a')})
					time.Sleep(200 * time.Millisecond)
				}
			}
			if strings.HasPrefix(input, "0b") || strings.HasPrefix(input, "0x") {
				i, err := strconv.ParseUint(input, 0, 8)
				if err != nil {
					log.Fatal(err)
				}

				_, err = s.Write([]byte{byte(i)})
				if err != nil {
					log.Fatal(err)
				}
				continue
			}

			dataBytes := []byte(input)
			for _, v := range dataBytes {
				_, err = s.Write([]byte{byte(v)})
				if err != nil {
					log.Fatal(err)
				}
			}
		}
	}

	log.Println("closing serial")
	s.Close()
	log.Println("serial closed")
}
