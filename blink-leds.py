# The script generates a rom image which contains
# a simple program for the 6502 processor. The 
# program blinks 8 leds by communicating to 
# the W65C22S Interface Adapter. 
code = bytearray([
    0xa9, 0xff,         # lda $ff
    0x8d, 0x02, 0x80,   # sta $8002 

    0xa9, 0x55,         # lda $55
    0x8d, 0x00, 0x80,   # sta $8000 

    0xa9, 0xaa,         # lda $aa
    0x8d, 0x00, 0x80,   # sta $8000

    0x4c, 0x05, 0xE0    # jmp $8005 
])

rom = code + bytearray([0xea] * (8192 - len(code)))
rom[0x1ffc] = 0x00
rom[0x1ffd] = 0xE0

with open("program.bin", "wb") as out:
    out.write(rom)
