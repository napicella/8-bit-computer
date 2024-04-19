# note: the none.lib is vended by the cc65 compiler in the /github/cc65/lib folder

.DEFAULT_GOAL := all
.PHONY: all install build

export PATH := $(HOME)/github/cc65/bin:$(PATH)
BUILD_DIR=./bin

build:
	mkdir -p $(BUILD_DIR)
	rm -f $(BUILD_DIR)/*

	# compile C code
	cc65 -t none --cpu 65sc02 -g -o $(BUILD_DIR)/main.s main_lcd.c

	# add the crt0.s to the none.lib
	ca65 --cpu 65sc02 -o $(BUILD_DIR)/crt0.o crt0.s
	cp none.lib $(BUILD_DIR)/
	ar65 a $(BUILD_DIR)/none.lib $(BUILD_DIR)/crt0.o
	
	# assemble all
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/lcd.o lcd.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/counter.o counter.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/um245.o um245.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/vectors.o vectors.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/interrupt.o interrupt.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/wait.o wait.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/main.o $(BUILD_DIR)/main.s
	
	# link them
	ld65 -C load.cfg -m $(BUILD_DIR)/blink.map --dbgfile $(BUILD_DIR)/debug_loader -o $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/vectors.o $(BUILD_DIR)/wait.o $(BUILD_DIR)/lcd.o $(BUILD_DIR)/counter.o $(BUILD_DIR)/um245.o $(BUILD_DIR)/main.o $(BUILD_DIR)/none.lib


install:
	minipro -p AT28C64B -w $(BUILD_DIR)/kernel.bin

all: build install



