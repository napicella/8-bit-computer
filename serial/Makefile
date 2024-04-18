build:
	go build -o bin/serial .

run: build
	bin/serial

run-serial: build
	bin/serial -d /dev/ttyUSB0