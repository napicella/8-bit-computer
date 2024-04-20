Resourses for an 8 bit computer based on the 6502 microprocessor.


### Components
Check the [datasheets](./datasheets) folder for the list of components.

### Memory mapping
| Address (16 bits)     | Address (hex) | Device | Length                   |
|-----------------------|---------------|--------|--------------------------|
| `0`000 0000 0000 0000 | `0x0000`      | RAM    |                          |
| `0`111 1111 1111 1111 | `0x7FFF`      | RAM    | 0x8000                   |
| `100`0 0000 0000 0000 | `0x8000`      | VIA    |                          |
| `100`1 1111 1111 1111 | `0x9FFF`      | VIA    | 0x1FFF                   |
| 1010 0000 0000 0000   |               | Unused |                          |
| 1011 1111 1111 1111   |               | Unused |                          |
| `111`0 0000 0000 0000 | `0xE000`      | ROM    |                          |
| `111`1 0000 0000 0000 | `0xFFFF`      | ROM    | 0x2000 = 0xFFFF-0xE000+1 |

The bits highlighted in the address are part of the address decoding which 
enable the chip.
For example, if the most significant bit of the address is low, the RAM is enabled, if
the three most significant bits are (100) the VIA is enabled. Address decoding is described
later. 


#### CPU stack (in RAM addresses)

The 65c02 needs a stack for the `jsr` and `rst` instuctions (assembly subroutines).  
The stack boundaries are set in the chip and expected to be between `0x0100` and `0x01FF`.
The stack grows from 0x01FF (top) to 0x0100 (bottom), for a total of 265 bytes.  

| Address (16 bits)   | Address (hex) | Device    | Length |
|---------------------|---------------|-----------|--------|
| 0000 0000 0000 0000 | `0x0000`      | RAM       |        |
| 0000 0001 0000 0000 | `0x0100`      | RAM Stack |        |
| 0000 0001 1111 1111 | `0x01FF`      | RAM Stack | 0xFF   |
| 0111 1111 1111 1111 | `0x7FFF`      | RAM       | 0x8000 |


#### VIA device

The whole address space between `0x8000` and `0x9FFF` is mapped to the VIA to simplify the address decoding, but the VIA only needs the first 16 bits in that address space.

| Address (hex)       | Description                          |
|---------------------|--------------------------------------|
| `0x8000`            | I/O Register B                       |
| `0x8001`            | I/O Register A                       |
| `0x8002`            | Data Direction Register B            |
| `0x8003`            | Data Direction Register A            |
| `0x8004`            | T1 Low Order Latches/Counter         |
| `0x8005`            | T1 High Order Counter                |
| `0x8006`            | T1 Low Order Latches                 |
| `0x8007`            | T1 High Order Latches                |
| `0x8008`            | T2 Low Order Latches/Counter         |
| `0x8009`            | T2 High Order Counter                |
| `0x800a`            | Shift Register                       |
| `0x800b`            | Auxiliary Control Register           |
| `0x800c`            | Peripheral Control Register          |
| `0x800d`            | Interrupt Flag Register              |
| `0x800e`            | Interrupt Enable Register            |
| `0x800f`            | I/O Register A sans Handshake        |
| `0x8010` - `0x9fff` | Mirrors of the sixteen VIA registers |


### Tools
#### Minipro
Download [minipro 0.5](https://gitlab.com/DavidGriffith/minipro/-/releases/0.5). 
Follow the [installation instructions](https://gitlab.com/DavidGriffith/minipro).

After installing, check it works by attaching the programmer to the laptop, then run the `minipro --version`. You should see minipro recognizing the device:

```
 pactvm > minipro --version
Supported programmers: TL866A/CS, TL866II+,T48 (experimental)
Found TL866II+ 04.2.132 (0x284)
Device code: 02092661
Serial code: 5BJVT6MR20ZZJBB0JPAE
minipro version 0.6     A free and open TL866 series programmer
Commit date:    2024-02-11 20:34:27 -0800
Git commit:     77c26fd2e5d8674d65382a3eff4628013483c4d2
Git branch:     master
TL866A/CS:      14184 devices, 44 custom
TL866II+:       16281 devices, 45 custom
Logic:            283 devices, 4 custom
```

__Note__: For Ubuntu 20, version 0.6 (latest as of writing fails during upload):
```
minipro -p AT28C64B -w kernel.bin
Found TL866II+ 04.2.132 (0x284)
Device code: 02092661
Serial code: 5BJVT6MR20ZZJBB0JPAE
Erasing... 0.02Sec OK
Writing Code...  1.67Sec  OK
Reading Code...  0.11Sec  OK
Verification failed at address 0x0000: File=0x00, Device=0xFF
make: *** [Makefile:20: install] Error 1
```

#### Install cc65
```
git clone https://github.com/cc65/cc65.git
cd cc65
make
```
Move the generated binaries somewhere under $PATH.


### Hello world
The 8 bit computer running a simple program which turns on eight leds, first the ones in a even position and then the ones in a odd position, looping forever.  
You do not need cc65 for the hello world.  

Run:
```
python blink-leds.py
```

which generates the image for the ROM in the file `program.bin`.  
Then connect the eeprom to the programmer and run:

```
minipro -p AT28C64B -w program.bin
```

---------------------

## WIP
### Build assemly file with the cc65 compiler suite

You need cc65.


#### Build blink.s

```
cl65 -C load.cfg --cpu 65C02 --no-target-lib blink.s
```

which builds the `blink` binary.


__Note__  
This binary should be exactly the same as the one generated by the python program

```
python blink-leds.py
cl65 -C load.cfg --cpu 65C02 --no-target-lib blink.s
```

compare with:
```
diff <(xxd ./blink) <(xxd ./program.bin)
```
which should return nothing (meaning the binary are the same).

#### Upload to mini pro

```bash
minipro -p AT28C64B -w blink
```


#### Resources
__calling assembly routines from C (calling sequence and fast calls)__ 
https://www.cc65.org/doc/customizing-7.html

__How do hardware timer/time sleep works__
https://www.youtube.com/watch?v=g_koa00MBLg&ab_channel=BenEater


__Reserving memory location in assembly__
```
.zeropage
_led_data:    .res 1, $00

.segment	"CODE"
; you can use _led_data in the code, eg.
lda  #$FF
sta  _led_data
```

__Links__
- cc65 function reference describes the C functions available in the standard library. - https://cc65.github.io/doc/funcref.html
- https://github.com/caseywdunn/eater_6502?tab=readme-ov-file
- https://www.grappendorf.net/projects/6502-home-computer/software-development.html
- https://www.masswerk.at/6502/6502_instruction_set.html#STA
- 65xx timing diagrams - https://laughtonelectronics.com/Arcana/Visualizing%2065xx%20Timing/Visualizing%2065xx%20CPU%20Timing.html
- https://github.com/peternoyes/dodo-sim
- https://mike42.me/blog/2021-07-adding-a-serial-port-to-my-6502-computer
- utils in asm and C for cc65 - https://github.com/tisnik/8bit-fame/tree/master/cc65
- https://github.com/SleepingInsomniac/6502-Breadboard-Computer
- https://cc65.github.io/doc/intro.html
- https://forums.atariage.com/topic/296494-calling-asm-proc-from-c/
- https://github.com/dbuchwald/6502
- https://github.com/sethm/symon
- https://stackoverflow.com/questions/10595467/org-assembly-directive-and-location-counter-in-linker-script
- https://cc65.github.io/doc/
- https://www.tejotron.com/
- https://www.reddit.com/r/beneater/comments/evis0o/6502_and_c_language/
- wozmon - https://gist.github.com/beneater/8136c8b7f2fd95ccdd4562a498758217/revisions
- wozmon instructions - https://youtu.be/HlLCtjJzHVI?si=5ztiLEWpOD5SaC5g
- msbasic - https://github.com/beneater/msbasic/commit/a15c8e0fdf620c800913cbb709df4042ab5c8dad

### How to have ca65 output as raw binary
__You can't.__

From: https://www.reddit.com/r/beneater/comments/mm3m9s/how_to_have_ca65_output_as_raw_binary/

Simple assemblers convert assembler source code file(s) directly into a final binary output file in one step.

More complex development systems convert each source code file into an intermediary object code file, then a linker/locater pulls together and converts all object files into one target binary file. All linkers require some type of configuration file that tells it where your RAM and ROM (aka Flash) memory is located, unless it has some known assumptions about the final target. Though more complicated, this approach is a lot more flexible, because this concept supports any combination of assemblers and compilers that creates the same object code.

Currently this development system only supports assembler and C, but theoretically new compilers could be created for other high-level computer languages too. As long as future compilers create compatible object files then LD65 should be able to support them too.

From a generic point of view of these concepts, there are plenty of books about this subject matter, and likely lots of stuff written about it on the internet. I'll leave it up to you to further investigate and educate yourself. Good luck.

```
C Source Code Files(s) ---> CC65 [compiler] ---> Object Code File(s)
Assembler Source Code File(s) ---> CA65 [assembler] ---> Object Code File(s)
Object Code File(s) and optional Libraries ---> LD65 [linker] and Config File ---> Binary or Hex File
Binary or Hex File ---> Chip Programmer Tool ---> ROM/Flash/EEPROM chip
```

#### ORG directive and VASM

Note that ca65 do not handle the org directive as other assemblers for 6502.

From: https://wiki.cc65.org/doku.php?id=cc65:the_.org_directive

> Most other assemblers for the 6502 handle .ORG different than ca65. The reason is, that these tools usually do not use a linker. Professional development systems separate the job of the assembler and the linker to gain more flexibility and to implement features not possible with just one tool.  
>
>
> If you don't have a separate linker, the assembler is responsible for code translation and code placement. With such an assembler, you use the .ORG directive to place your code at a specific address.

For example:
```
  .org $0000
  NOP
  .org $FFFF
  NOP
```
When compiled:

```
ca65 --cpu 65sc02 -o sample.o sample.s
ld65 -t none -o sample sample.o
```

Produces a 2 bytes ROM image, instead of 64K:
```
> ls -lha ./sample
-rw-r--r-- 1 user domain^users 2 Apr 20 23:19 ./sample
``` 

vasm on the other hand for the same program produces 64K:
```
> /tmp/vasm/vasm6502_oldstyle -Fbin -dotdir -o binary.out sample.s
...

> -rw-r--r-- 1 napicell domain^users 64K Apr 20 23:25 ./binary.out

> hexdump -C binary.out 
00000000  ea 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
0000fff0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 ea  |................|
00010000
```


### Disassemble
```
da65 --cpu 65C02 -g --comments 4 blink_subroutines | less

```

### Incremental levels of abstractions
Before we start. 6502 reads from `0x1ffc` (low byte) and `0x1ffd` (high byte) the address where to read the first instruction. We need to write at that address where that is. In this project, the first instruction is
at location 0 in the ROM, which is 0xE000 (1110000000000000) as seen by the CPU (see memory mapping)

- `blink-leds.py` creates the binary by specifing the 6502 instructions in binary. No assembler.
The program does not RAM. It blinks without a wait, so run it with a slow clock. The python script creates
a binary that exactly fits the rom and sets the address `0x1ffc` and `0x1ffd` to 0xE000
- `blink.s` it's the assembly version of the previous program. 
- `ram_test.s` is the first program that needs a ram to be installed. It verfies the ram works by storing something in ram and reading it back
- `blink_subroutines.s` runs the blink program with subroutines. This requires a ram since the `RTS` function (return sub routine) needs to know where to return to. This info is stored in the stack by the `JSR` (jump sub routine). The assemply also use `PHX` and `PLX` (push/pull x register to stack) to showcase how a subroutine can avoid interfere with registers being set outside of the subroutine.




