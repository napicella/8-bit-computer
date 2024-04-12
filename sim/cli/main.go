package main

import (
	"flag"
	"fmt"
	"os"

	"github.com/napicella/8-bit-computer/sim"
)

var imagePath string

func main() {
	flag.StringVar(&imagePath, "i", "", "path of the image to laod in ROM")
	flag.Parse()
	if imagePath == "" {
		fmt.Print("-i <rom image path> flag required \n\n")
		flag.Usage()
		os.Exit(1)
	}

	bus := new(sim.Bus)
	bus.New()

	ram := new(sim.Ram)
	bus.Add(ram)

	rom := new(sim.Rom)
	bus.Add(rom)

	spy := new(sim.Spy)
	bus.Add(spy)

	// um245 := sim.NewUm245()
	// bus.Add(um245)

	dat, err := os.ReadFile(imagePath)
	if err != nil {
		fmt.Println(err)
		return
	}
	copy(rom[:], dat)

	bus.BuildMap()

	cpu := new(sim.Cpu)
	cpu.Reset(bus)

	sim.BuildTable()

	for {
		opcode := bus.Read(cpu.PC)

		cpu.PC++
		cpu.Status |= sim.Constant
		sim.Execute(cpu, bus, opcode)

		if cpu.Status&sim.Interrupt != 0 {
			// assuming BRK instuction
			return
		}
	}
}
