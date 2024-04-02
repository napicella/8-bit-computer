package dodosim

import "fmt"

type Acia struct {
	portAOut bool
}

func (a *Acia) Start() uint16 {
	return 0x8000
}

func (a *Acia) Length() uint32 {
	return 0x1FFF
}

func (a *Acia) Read(addr uint16) uint8 {
	fmt.Printf("Read from ACIA at %d\n", addr)
	//panic("Reading from Acia")
	return 0;
}

func (a *Acia) Write(addr uint16, val uint8) {
	fmt.Printf("[ACIA] writing add :%04x - value: %02x \n", addr, val)

	if addr == 0x8002 {
		if val == 0xFF {
			a.portAOut = true
			fmt.Println("setting ACIA port A as output")
		}
		return
	}

	if addr == 0x8000 {
		fmt.Printf("turning leds %08b\n", val)
	}

	fmt.Println("Press the Enter Key to continue")
	fmt.Scanln() // wait for Enter Key
}
