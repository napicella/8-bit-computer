package sim

import (
	"fmt"
)

type Spy struct {
}

func (t *Spy) Start() uint16 {
	return 0b1010000000000000
}

func (t *Spy) Length() uint32 {
	return 1
}

func (t *Spy) Read(addr uint16) uint8 {
	panic("not implemented")
}

func (t *Spy) Write(addr uint16, val uint8) {
	fmt.Printf("%s\n", string(val))
}
