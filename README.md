Resourses for an 8 bit computer based on the 6502 microprocessor


### Components
Micro processor and memory
- [W65C02S Microprocessor](./datasheets/W65C02S-cpu.pdf)
- [AT28C64B 64K (8K x 8) ParallelEEPROM](./datasheets/28C64-eeprom.pdf)

Peripherals

- [W65C22S Interface Adapter](./datasheets/W65C22S6TPG-14-interface-adapter.pdf)

Address decoding logic
- [74HCT14 hex inverter](./datasheetps/74HC_HCT14-inverter.pdf)
- [HD74HC11P triple input AND gate](./datasheets/HD74HC11P-triple-input-and-gate.pdf)

Clock Module
- [555 timer](./datasheets/lm555-timer.pdf)
- 0.1 and 1 Microfarad capacitors
- 1K and 10K resistors
- 1M variable resistor
- Push buttons, switch and leds

### Memory mapping
![](./imgs/memory-mapping.jpg)

### Hello world
The 8 bit computer running a simple program which turns on eight leds, first the ones in a even position and then the ones in a odd position, looping forever.

![](./imgs/leds.gif)
