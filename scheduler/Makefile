.DEFAULT_GOAL := build
export PATH := $(HOME)/github/cc65/bin:$(PATH)

BUILD_DIR=./bin

build:
	mkdir -p $(BUILD_DIR)
	rm -f $(BUILD_DIR)/*
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/bios.o bios.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/main.o main.s
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/scheduler.o scheduler.s
	ld65 -C load.cfg \
	  -o $(BUILD_DIR)/scheduler.bin \
	  $(BUILD_DIR)/bios.o \
	  $(BUILD_DIR)/main.o \
	  $(BUILD_DIR)/scheduler.o

build-simple:
	mkdir -p $(BUILD_DIR)
	ca65 --cpu 65sc02 -g -o $(BUILD_DIR)/main.o main_simple.s
	ld65 -C load.cfg \
	  -Ln $(BUILD_DIR)/scheduler.lbl \
	  -m $(BUILD_DIR)/blink.map \
	  --dbgfile $(BUILD_DIR)/debug_loader \
	  -o $(BUILD_DIR)/scheduler.bin \
	  $(BUILD_DIR)/main.o