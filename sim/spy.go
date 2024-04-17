package sim

import (
	"fmt"
)

type Spy struct {
	count int
}

func (t *Spy) Start() uint16 {
	return 0xA000
}

func (t *Spy) Length() uint32 {
	return 2
}

func (t *Spy) Read(addr uint16) uint8 {
	panic("not implemented")
}

func (t *Spy) Write(addr uint16, val uint8) {
	t.count++
	if addr == 0xA000 {
		fmt.Printf("[SPY] [W] [DISK_R] COUNT %d VALUE %04x %04d(DEC) %s\n", t.count, val, val, string(val))
		return
	}
	if addr == 0xA001 {
		fmt.Printf("[SPY] [W] [DISK_W] COUNT %d VALUE %04x %04d(DEC) %s\n", t.count, val, val, string(val))
	}
}
