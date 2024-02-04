Resourses for an 8 bit computer based on the 6502 microprocessor


### Components
_Micro processor and memory_
- [W65C02S Microprocessor](./datasheets/w65c02s-micro-processor.pdf)
- [AT28C64B 64K (8K x 8) ParallelEEPROM](./datasheets/28C64-eeprom.pdf)

_Peripherals_
- [W65C22S Interface Adapter](./datasheets/W65C22S6TPG-14-interface-adapter.pdf)

_Address decoding logic_
- [74HCT14 hex inverter](./datasheets/74HC_HCT14-inverter.pdf)
- [HD74HC11P triple input AND gate](./datasheets/HD74HC11P-triple-input-and-gate.pdf)

_Clock Module (for debugging)_  
The module is used for debugging, genereting a slow clock pulse or allowing to execute one instruction at the time
- [555 timer](./datasheets/lm555-timer.pdf)
- 0.1 and 1 Microfarad capacitors
- 1K and 10K resistors
- 1M variable resistor
- Push buttons, switch and leds

_Clock Module_
- [1 MHz oscillator](./datasheets/1MHz-oscillator-AEL9700CS.pdf)

### Memory mapping
<img src="./imgs/memory-mapping.jpg" width="380">


### Hello world
The 8 bit computer running a simple program which turns on eight leds, first the ones in a even position and then the ones in a odd position, looping forever.

Run:
```
python blink-leds.py
```

which generates the image for the ROM in the file `program.bin`.  
Then connect the eeprom to the programmer and run:

```
minipro -p AT28C64B -w program.bin
```

![](./imgs/leds.gif)


---------------------
## WIP

### Upload to mini pro
```bash
vasm6502_oldstyle -Fbin -dotdir blink.s
hexdump -C a.out
minipro -p AT28C64B -w a.out
```


### Install cc65 compiler
```
git clone https://github.com/cc65/cc65.git
cd cc65
make
```