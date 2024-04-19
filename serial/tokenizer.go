package main

import (
	"bytes"
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"slices"
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

func newTokenizer(d *disk) *tokenizer {
	return &tokenizer{
		cmdModeBuff: make([]uint8, 7),
		disk:        d,
		count:       0,
	}
}

type tokenizer struct {
	cmdMode     bool
	cmd         bytes.Buffer
	cmdModeBuff []uint8

	tokens []token
	disk   *disk
	count  int

	state         int
	contentLength uint16
	bytesCount    int
}

const (
	stateMagicValue int = iota
	stateDataSize
	stateDataSizeSecondByte
	stateDataRead
)

func (t *tokenizer) submit(val uint8) {
	if t.state == stateMagicValue {
		_, r, _ := Shift(t.cmdModeBuff)
		r = append(r, val)
		t.cmdModeBuff = r
		if slices.Compare(t.cmdModeBuff, []uint8{'c', 'm', 'd', 'm', 'o', 'd', 'e'}) == 0 {
			t.state = stateDataSize
			t.contentLength = 0
			t.cmdModeBuff = make([]uint8, 7)
			t.bytesCount = 0
			t.cmd.Reset()

			return
		}
	}
	if t.state == stateDataSize {
		t.contentLength = uint16(val)
		t.state = stateDataSizeSecondByte
		return
	}
	if t.state == stateDataSizeSecondByte {
		t.contentLength = (uint16(val) << 8) + t.contentLength
		t.state = stateDataRead
		return
	}
	if t.state == stateDataRead {
		t.bytesCount++
		t.cmd.WriteByte(val)

		if t.bytesCount == int(t.contentLength) {
			t.state = stateMagicValue

			data := make([]uint8, t.bytesCount)
			t.bytesCount = 0

			n, err := t.cmd.Read(data)
			fmt.Printf("packet end - read %d data %v\n", n, data[:min(10, len(data))])
			if err != nil {
				panic(err)
			}

			t.count++
			os.WriteFile(fmt.Sprintf("/tmp/write_%d", t.count), data, 0666)

			t.tokens = append(t.tokens, token{
				data: data,
			})

			return
		}
	}
}

func (t *tokenizer) next() (token, bool) {
	if len(t.tokens) == 0 {
		return token{}, false
	}
	head := t.tokens[0]
	t.tokens = t.tokens[1:]
	return head, true
}

func newDisk() (*disk, error) {
	name := filepath.Join(os.TempDir(), "disk")
	var shouldInitialize bool
	if _, err := os.Stat(name); errors.Is(err, os.ErrNotExist) {
		shouldInitialize = true
	}
	f, err := os.OpenFile(name, os.O_RDWR|os.O_CREATE, 0666)
	if err != nil {
		return nil, err
	}

	if shouldInitialize {
		data := make([]byte, 2048)
		for i := 0; i < 2048; i++ {
			data[i] = 0xFF
		}
		_, err := f.Write(data)
		if err != nil {
			return nil, err
		}
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

// Shift removes the first element from s, returning false if the slice is empty.
func Shift[S ~[]E, E any](s S) (first E, rest S, ok bool) {
	if len(s) == 0 {
		rest = s
	} else {
		first, rest, ok = s[0], s[1:], true
	}
	return
}
