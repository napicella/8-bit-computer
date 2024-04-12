package sim

import (
	"fmt"
)

type Rom [0x1FFF + 1]uint8

func (rom *Rom) Start() uint16 {
	return 0xE000
}

func (rom *Rom) Length() uint32 {
	return 0x1FFF + 1
}

func (rom *Rom) Read(addr uint16) uint8 {
	val := rom[addr-0xE000]
	fmt.Printf("[ROM] reading add :%04x - value: %02x \n", addr, val)
	return val
}

func (rom *Rom) Write(addr uint16, val uint8) {
	fmt.Println("Address: ", addr)
	panic("ROM is readonly")
}
