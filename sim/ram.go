package dodosim

import "fmt"

type Ram [0x7FFF + 1]uint8

func (ram *Ram) Start() uint16 {
	return 0x0
}

func (ram *Ram) Length() uint32 {
	return 0x7FFF + 1
}

func (ram *Ram) Read(addr uint16) uint8 {
	val := ram[addr]
	fmt.Printf("[RAM] reading add :%04x - value: %02x \n", addr, val)
	return val
}

func (ram *Ram) Write(addr uint16, val uint8) {
	fmt.Printf("[RAM] writing add :%04x - value: %02x \n", addr, val)
	ram[addr] = val
}
