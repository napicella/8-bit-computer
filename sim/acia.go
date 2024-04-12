package sim

type Acia struct{}

func (a *Acia) Start() uint16 {
	return 0x8000
}

func (a *Acia) Length() uint32 {
	return 0x1FFF
}

func (a *Acia) Read(addr uint16) uint8 {
	panic("not implemeted")
}

func (a *Acia) Write(addr uint16, val uint8) {
	panic("not implemeted")
}
