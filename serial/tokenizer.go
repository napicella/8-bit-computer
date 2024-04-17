package main

import (
	"bytes"
	"fmt"
	"os"
	"path/filepath"
)

const (
	startOfHeading = 0b00000001
	endOfHeading   = 0b00000011
	readOp         = 0b00000100
	writeOp        = 0b00001000
)

// RD_CMD_START = <START_HEADING>R<SECTOR><END_HEADING>
// <SECTOR> - 8 bits
//
// MCU   -> UM245 (RD_CMD_START)
// UM245 -> PC
// PC    -> UM245 (<DATA>)
var RD_CMD_START = []byte{
	startOfHeading,
	readOp,
	0b00000000,
	endOfHeading,
}

// WR_CMD_START = <START_HEADING>W<SECTOR><DATA><END_HEADING>
// <SECTOR> - 8 bits
var WR_CMD_START = []byte{
	startOfHeading,
	writeOp,
	0b00000000,
	0b00000000,
	endOfHeading,
}

type token struct {
	tType string
	data  []byte
}

type tokenizer struct {
	cmdMode bool
	cmd     bytes.Buffer

	tokens []token
	disk   *disk
}

func (t *tokenizer) submit(val uint8) {
	if val == startOfHeading {
		t.cmdMode = true
		t.cmd.WriteByte(val)
		return
	}
	if val == endOfHeading {
		t.cmdMode = false
		t.cmd.WriteByte(val)
		data, err := t.cmd.ReadBytes(endOfHeading)
		if err != nil {
			panic(err)
		}
		if data[1] == 'R' {
			t.tokens = append(t.tokens, token{
				tType: "read",
				data:  data,
			})
			return
		}
		if data[1] == 'W' {
			t.tokens = append(t.tokens, token{
				tType: "write",
				data:  data,
			})
		}
		return
	}
	if t.cmdMode {
		t.cmd.WriteByte(val)
		return
	}

	t.tokens = append(t.tokens, token{
		tType: "data",
		data:  []byte{val},
	})
}

func (t *tokenizer) next() (token, bool) {
	if len(t.tokens) == 0 {
		return token{}, false
	}
	head := t.tokens[0]
	t.tokens = t.tokens[1:]
	return head, true
}

func (t *tokenizer) handleToken(tok token, write func([]byte) error) error {
	if tok.tType == "read" {
		fmt.Println("reading from disk")
		sector, err := getSector(tok.data)
		if err != nil {
			panic(err)
		}
		d, err := t.disk.read(sector)
		if err != nil {
			panic(err)
		}
		err = write(d)
		if err != nil {
			panic(err)
		}
		return nil
	}
	if tok.tType == "write" {
		fmt.Println("writing to disk")
		sector, err := getSector(tok.data)
		if err != nil {
			panic(err)
		}
		c, err := getContent(tok.data)
		if err != nil {
			panic(err)
		}
		err = t.disk.write(sector, c)
		if err != nil {
			panic(err)
		}
		return nil
	}

	if tok.tType == "data" {
		fmt.Printf("%s", tok.data)
	}
	return nil
}

func getSector(data []byte) (uint8, error) {
	return data[2], nil
}

func getContent(data []byte) ([]byte, error) {
	return data[3 : len(data)-1], nil
}

func newDisk() (*disk, error) {
	name := filepath.Join(os.TempDir(), "disk")
	data := make([]byte, 2048)
	for i := 0; i < 2048; i++ {
		data[i] = 0xFF
	}
	os.WriteFile(name, data, 0666)
	f, err := os.OpenFile(name, os.O_RDWR, 0666)
	if err != nil {
		return nil, err
	}
	return &disk{
		f: f,
	}, nil
}

const blockSize = 512

type disk struct {
	f *os.File
}

func (t *disk) close() {
	t.f.Close()
}

func (t *disk) read(sector uint8) ([]byte, error) {
	_, err := t.f.Seek(int64(uint(sector)*blockSize), 0)
	if err != nil {
		return nil, err
	}
	block := make([]byte, blockSize)
	_, err = t.f.Read(block)
	if err != nil {
		return nil, err
	}
	return block, nil
}

func (t *disk) write(sector uint8, data []uint8) error {
	_, err := t.f.Seek(int64(uint(sector)*blockSize), 0)
	if err != nil {
		return err
	}
	_, err = t.f.Write(data)
	if err != nil {
		return err
	}
	return nil
}
