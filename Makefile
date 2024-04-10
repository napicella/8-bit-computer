build-blink-subroutines:
	cl65 -t none -C load.cfg --cpu 65C02 --no-target-lib blink_subroutines.s

install-blink-subroutines:
	minipro -p AT28C64B -w blink_subroutines

build-blink-timer:
	cl65 -t none -C load.cfg --cpu 65C02 --no-target-lib blink_timer.s	

install-blink-timer:
	minipro -p AT28C64B -w blink_timer

build-ram-full:
	cl65 -t none -C load.cfg --cpu 65SC02 --no-target-lib ram_test_full.s

install-ram-full:
	minipro -p AT28C64B -w ram_test_full

build-lcd-8:
	cl65 -t none -C load.cfg --cpu 65SC02 --no-target-lib lcd_8_pin.s

install-lcd-8:
	minipro -p AT28C64B -w lcd_8_pin

build-lcd-4:
	cl65 -t none -C load.cfg --cpu 65SC02 --no-target-lib lcd_4_pin.s

install-lcd-4:
	minipro -p AT28C64B -w lcd_4_pin

build-lcd-4-uart:
	cl65 -t none -C load.cfg --cpu 65SC02 --no-target-lib lcd_4_pin_uart.s

install-lcd-4-uart:
	minipro -p AT28C64B -w lcd_4_pin_uart

