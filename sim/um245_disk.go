package sim

import (
	"bytes"
	"fmt"
	"os"
	"path/filepath"
)

const (
	UART_WRITE  = 0b1001000000000001
	UART_READ   = 0b1001000000000010
	UART_STATUS = 0b1001000000000100

	UART_WRITE_BUSY_MASK   = 0b00000001
	UART_READ_NO_DATA_MASK = 0b00000010
)

func NewUm245() *Um245 {
	d, err := newDisk()
	if err != nil {
		panic(err)
	}
	t := &tokenizer{}
	return &Um245{
		b:         &bytes.Buffer{},
		tokenizer: t,
		disk:      d,
	}
}

type Um245 struct {
	b         *bytes.Buffer
	tokenizer *tokenizer
	disk      *disk
}

func (t *Um245) Start() uint16 {
	return 0x9000
}

func (t *Um245) Length() uint32 {
	return 0x0005
}

func (t *Um245) Read(addr uint16) uint8 {
	switch addr {
	case UART_READ:
		b, _ := t.b.ReadByte()
		return b
	case UART_STATUS:
		val := 0b00000000
		if t.b.Available() == 0 {
			val = val & UART_READ_NO_DATA_MASK
		}
		return uint8(val)
	default:
		panic("invalid read address for um245")
	}
}

func (t *Um245) Write(addr uint16, val uint8) {
	if addr != UART_WRITE {
		panic("invalid write address for um245")
	}

	t.tokenizer.submit(val)
	if tok, found := t.tokenizer.next(); found {
		t.handleToken(tok)
	}
}

func (t *Um245) handleToken(tok token) {
	if tok.tType == "read" {
		sector, err := getSector(tok.data)
		if err != nil {
			panic(err)
		}
		d, err := t.disk.read(sector)
		if err != nil {
			panic(err)
		}
		_, err = t.b.Write(d)
		if err != nil {
			panic(err)
		}
		return
	}
	if tok.tType == "write" {
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
		return
	}

	if tok.tType == "data" {
		fmt.Printf("%s", tok.data)
	}
}

func getSector(data []byte) (uint8, error) {
	return data[2], nil
}

func getContent(data []byte) ([]byte, error) {
	return data[3 : len(data)-1], nil
}

func (t *Um245) Close() {
	t.disk.close()
}

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

type disk struct {
	f *os.File
}

func (t *disk) close() {
	t.f.Close()
}

func (t *disk) read(sector uint8) ([]byte, error) {
	_, err := t.f.Seek(int64(uint(sector)*255), 0)
	if err != nil {
		return nil, err
	}
	block := make([]byte, 255)
	_, err = t.f.Read(block)
	if err != nil {
		return nil, err
	}
	return block, nil
}

func (t *disk) write(sector uint8, data []uint8) error {
	_, err := t.f.Seek(int64(uint(sector)*255), 0)
	if err != nil {
		return err
	}
	_, err = t.f.Write(data)
	if err != nil {
		return err
	}
	return nil
}
