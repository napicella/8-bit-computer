export PATH := $(HOME)/github/cc65/bin:$(PATH)
BUILD_DIR=./bin

blink-build:
	cl65 -t none -C load.cfg --cpu 65C02 -o $(BUILD_DIR)/blink --no-target-lib blink.s

blink-install:
	minipro -p AT28C64B -w $(BUILD_DIR)/blink

blink-subroutines-build:
	cl65 -t none -C load.cfg --cpu 65C02 -o $(BUILD_DIR)/blink_sub --no-target-lib blink_subroutines.s

blink-subroutines-install:
	minipro -p AT28C64B -w $(BUILD_DIR)/blink_sub

blink-subroutines-256-install:
	minipro -p AT28C256 -uP -w $(BUILD_DIR)/blink_sub

blink-timer-build:
	cl65 -t none -C load.cfg --cpu 65C02 -o $(BUILD_DIR)/blink_timer --no-target-lib blink_timer.s	

blink-timer-install:
	minipro -p AT28C64B -w $(BUILD_DIR)/blink_timer

ram-full-build:
	cl65 -t none -C load.cfg --cpu 65SC02 -o $(BUILD_DIR)/ram_test_full --no-target-lib ram_test_full.s

ram-full-install:
	minipro -p AT28C64B -w $(BUILD_DIR)/ram_test_full

lcd-8-build:
	cl65 -t none -C load.cfg --cpu 65SC02 $(BUILD_DIR)/lcd_8_pin --no-target-lib lcd_8_pin.s

lcd-8-install:
	minipro -p AT28C64B -w $(BUILD_DIR)/lcd_8_pin

lcd-4-build:
	cl65 -t none -C load.cfg --cpu 65SC02 -o $(BUILD_DIR)/lcd_4_pin --no-target-lib lcd_4_pin.s

lcd-4-install:
	minipro -p AT28C64B -w $(BUILD_DIR)/lcd_4_pin

lcd-4-uart-build:
	cl65 -t none -C load.cfg --cpu 65SC02 -o $(BUILD_DIR)/lcd_4_pin_uart --no-target-lib lcd_4_pin_uart.s

lcd-4-uart-install:
	minipro -p AT28C64B -w $(BUILD_DIR)/lcd_4_pin_uart

lcd-4-uart-256-install:
	minipro -p AT28C256 -uP -w $(BUILD_DIR)/lcd_4_pin_uart

wozmon-build:
	cl65 -t none -C wozmon.cfg --cpu 65SC02 -o $(BUILD_DIR)/wozmon --no-target-lib wozmon.s

wozmon-install:
	minipro -p AT28C64B -uP -w wozmon

wozmon-256-build:
	cl65 -t none -C wozmon-256.cfg --cpu 65SC02 -o $(BUILD_DIR)/wozmon_256 --no-target-lib wozmon.s
	
wozmon-256-install:
	minipro -p AT28C256 -uP -w $(BUILD_DIR)/wozmon_256


