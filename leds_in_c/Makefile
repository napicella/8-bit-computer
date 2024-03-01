# note: the none.lib is vended by the cc65 compiler in the /github/cc65/lib folder

build:
	rm -f main.o crt0.o io.o vectors.o interrupt.o wait.o kernel.bin
	cc65 -t none --cpu 65sc02 -g main.c

	ca65 --cpu 65sc02 crt0.s
	ar65 a none.lib crt0.o
	
	ca65 --cpu 65sc02 -g io.s
	ca65 --cpu 65sc02 vectors.s
	ca65 --cpu 65sc02 interrupt.s
	ca65 --cpu 65sc02 wait.s
	ca65 --cpu 65sc02 -g main.s
	
	ld65 -C load.cfg -m blink.map --dbgfile debug_loader -o kernel.bin interrupt.o vectors.o wait.o io.o main.o none.lib

build-memory-mapped:
	rm -f main.o crt0.o io.o vectors.o interrupt.o wait.o kernel.bin
	cc65 -t none --cpu 65sc02 -g -o main.s main_memory_mapped.c

	ca65 --cpu 65sc02 crt0.s
	ar65 a none.lib crt0.o
	
	ca65 --cpu 65sc02 -g io.s
	ca65 --cpu 65sc02 vectors.s
	ca65 --cpu 65sc02 interrupt.s
	ca65 --cpu 65sc02 wait.s
	ca65 --cpu 65sc02 -g main.s
	
	ld65 -C load.cfg -m blink.map --dbgfile debug_loader -o kernel.bin interrupt.o vectors.o wait.o io.o main.o none.lib

build-fast-call-with-param:
	rm -f main.o crt0.o io.o vectors.o interrupt.o wait.o kernel.bin
	cc65 -t none --cpu 65sc02 -g -o main.s main_fast_call_with_param.c

	ca65 --cpu 65sc02 crt0.s
	ar65 a none.lib crt0.o
	
	ca65 --cpu 65sc02 -g io.s
	ca65 --cpu 65sc02 vectors.s
	ca65 --cpu 65sc02 interrupt.s
	ca65 --cpu 65sc02 wait.s
	ca65 --cpu 65sc02 -g main.s
	
	ld65 -C load.cfg -m blink.map --dbgfile debug_loader -o kernel.bin interrupt.o vectors.o wait.o io.o main.o none.lib

install:
	minipro -p AT28C64B -w kernel.bin

dis:
	da65 --cpu 65C02 -g --comments 4 -o debug_kernel kernel.bin
	gedit debug_kernel &

