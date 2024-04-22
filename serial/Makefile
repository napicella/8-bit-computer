build:
	go build -o bin/serial .

run-pipe: build
	bin/serial -p

run-serial: build
	bin/serial -d /dev/ttyUSB0