#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "devices.h"

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
        printf("[RAM] reading addr: %04x - value: %02x \n", addr, val);
        return val;
    } else {
        printf("[RAM] reading addr: %04x - out of range\n", addr);
        return 0; // or handle out of range access accordingly
    }
}

// Implement the Write method
void Ram_Write(uint16_t addr, uint8_t val, Ram *ram) {
    if (addr < RAM_SIZE) {
        printf("[RAM] writing addr: %04x - value: %02x \n", addr, val);
        ram->data[addr] = val;
    } else {
        printf("[RAM] writing addr: %04x - out of range\n", addr);
        // or handle out of range access accordingly
    }
}


// Function to create a Ram object and initialize the Space interface
Ram* CreateRam() {
    Ram* ram = malloc(sizeof(Ram));
    return ram;
}