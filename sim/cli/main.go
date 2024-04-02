package main

import (
	"fmt"
	"os"

	dodosim "github.com/peternoyes/dodo-sim"
)

func main() {
	bus := new(dodosim.Bus)
	bus.New()

	ram := new(dodosim.Ram)
	bus.Add(ram)

	rom := new(dodosim.Rom)
	bus.Add(rom)

	acia := new(dodosim.Acia)
	bus.Add(acia)

	dat, err := os.ReadFile("/home/napicella/github/8-bit-computer/leds_in_c/kernel.bin")
	//dat, err := os.ReadFile("/home/napicella/github/8-bit-computer/ram_test_full")
	if err != nil {
		fmt.Println(err)
		return
	}
	for i, b := range dat {
		rom[i] = b
	}

	bus.BuildMap()

	cpu := new(dodosim.Cpu)
	cpu.Reset(bus)

	dodosim.BuildTable()

	for {
		before := cpu.PC
		opcode := bus.Read(cpu.PC)

		cpu.PC++
		cpu.Status |= dodosim.Constant
		dodosim.Execute(cpu, bus, opcode)

		// fmt.Println("Press the Enter Key to continue")
		// fmt.Scanln() // wait for Enter Key

		if before == cpu.PC {
			if cpu.PC != 13209 {
				fmt.Printf("Failure. Trap at %b\n", cpu.PC)
				panic("error")
			}
			return
		}
	}

	panic("loop ended")
}
