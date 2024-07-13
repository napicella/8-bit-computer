#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "devices.h"
#include "log.h"

// Implement the Start method
uint16_t Ram_Start() {
    return 0x0;
}

// Implement the Length method
uint32_t Ram_Length() {
    return RAM_SIZE;
}

// Implement the Read method
uint8_t Ram_Read(uint16_t addr, Ram *ram) {
    if (addr < RAM_SIZE) {
        uint8_t val = ram->data[addr];
        log_debug("[RAM] reading addr: %04x - value: %02x", addr, val);
        return val;
    } else {
        log_debug("[RAM] reading addr: %04x - out of range", addr);
        exit(EXIT_FAILURE);
    }
}

// Implement the Write method
void Ram_Write(uint16_t addr, uint8_t val, Ram *ram) {
    if (addr < RAM_SIZE) {
        log_debug("[RAM] writing addr: %04x - value: %02x", addr, val);
        ram->data[addr] = val;
    } else {
        log_debug("[RAM] writing addr: %04x - out of range", addr);
        exit(EXIT_FAILURE);
    }
}


// Function to create a Ram object and initialize the Space interface
Ram* CreateRam() {
    Ram* ram = malloc(sizeof(Ram));
    return ram;
}