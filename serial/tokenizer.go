package main

import (
	"bytes"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
)

const tokenLength = 8

type token struct {
	data []byte
}

func newTokenizer(logFile *os.File) *tokenizer {
	return &tokenizer{
		tokenStart: [tokenLength]uint8{'@', 'c', 'm', 'd', 'm', 'o', 'd', 'e'},
		logFile:    logFile,
	}
}

type tokenizer struct {
	logFile *os.File

	tokenStart  [tokenLength]uint8
	currToMatch int

	tokenData         bytes.Buffer
	textData          bytes.Buffer
	potentialTextData bytes.Buffer

	tokens []token

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
		t.potentialTextData.WriteByte(val)

		if val != t.tokenStart[t.currToMatch] {
			b, err := io.ReadAll(&t.potentialTextData)
			if err != nil {
				panic(err)
			}
			t.textData.Write(b)
			t.potentialTextData.Reset()
			// reset character to match
			t.currToMatch = 0
			return
		}
		t.currToMatch++
		if t.currToMatch == len(t.tokenStart) {
			t.potentialTextData.Reset()
			t.currToMatch = 0
			t.state = stateDataSize
			t.contentLength = 0
			t.bytesCount = 0
			t.tokenData.Reset()

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
		t.tokenData.WriteByte(val)

		if t.bytesCount == int(t.contentLength) {
			t.state = stateMagicValue

			data := make([]uint8, t.bytesCount)
			t.bytesCount = 0

			_, err := t.tokenData.Read(data)
			if err != nil {
				panic(err)
			}

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

func (t *tokenizer) data() ([]byte, error) {
	return io.ReadAll(&t.textData)
}

const (
	blockSize      = 512
	numberOfBlocks = 500
)

var diskSize = blockSize * numberOfBlocks

func newDisk(logFile *os.File) (*disk, error) {
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
		data := make([]byte, diskSize)
		for i := 0; i < diskSize; i++ {
			data[i] = 0xFF
		}
		_, err := f.Write(data)
		if err != nil {
			return nil, err
		}
	}

	return &disk{
		f:       f,
		logFile: logFile,
	}, nil
}

type disk struct {
	f       *os.File
	logFile *os.File
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
	fmt.Fprintf(t.logFile, "========\n")
	fmt.Fprintf(t.logFile, "Read block from disk(size %d)\n", len(block))
	fmt.Fprintf(t.logFile, "%x\n", block)
	fmt.Fprintf(t.logFile, "========\n")
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
	fmt.Fprintf(t.logFile, "========\n")
	fmt.Fprintf(t.logFile, "Wrote block to disk(size %d)\n", len(data))
	fmt.Fprintf(t.logFile, "========\n")
	return nil
}
