package main

import (
	"bytes"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
)

type token struct {
	data []byte
}

func newTokenizer() *tokenizer {
	return &tokenizer{
		tokenStart: [7]uint8{'c', 'm', 'd', 'm', 'o', 'd', 'e'},
	}
}

type tokenizer struct {
	tokenStart  [7]uint8
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
		if t.currToMatch > len(t.tokenStart) {
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

			n, err := t.tokenData.Read(data)
			fmt.Printf("packet end - read %d data %v\n", n, data[:min(10, len(data))])
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
