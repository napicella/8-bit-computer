# note: the none.lib is vended by the cc65 compiler in the /github/cc65/lib folder

build-lcd:
	rm -f main.o crt0.o io.o vectors.o interrupt.o wait.o lcd.o kernel.bin
	cc65 -t none --cpu 65sc02 -g -o main.s main_lcd.c

	ca65 --cpu 65sc02 crt0.s
	ar65 a none.lib crt0.o
	
	ca65 --cpu 65sc02 -g lcd.s
	ca65 --cpu 65sc02 vectors.s
	ca65 --cpu 65sc02 interrupt.s
	ca65 --cpu 65sc02 wait.s
	ca65 --cpu 65sc02 -g main.s
	
	ld65 -C load.cfg -m blink.map --dbgfile debug_loader -o kernel.bin interrupt.o vectors.o wait.o lcd.o main.o none.lib

install:
	minipro -p AT28C64B -w kernel.bin

