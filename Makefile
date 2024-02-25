build-blink-subroutines:
	cl65 -C load.cfg --cpu 65C02 --no-target-lib blink_subroutines.s

install-blink-subroutines:
	minipro -p AT28C64B -w blink_subroutines

build-ram-full:
	cl65 -C load.cfg --cpu 65SC02 --no-target-lib ram_test_full.s

install-ram-full:
	minipro -p AT28C64B -w ram_test_full
