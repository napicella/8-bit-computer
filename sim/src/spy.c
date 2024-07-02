#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "devices.h"

// Implement the Start method
uint16_t Spy_Start() {
    return 0x8010;
}

// Implement the Length method
uint32_t Spy_Length() {
    return 5;
}

// Implement the Read method
uint8_t Spy_Read(uint16_t addr, Spy *spy) {
    printf("[SPY] reading addr: %04x", addr);
    return 0;
}

// Implement the Write method
void Spy_Write(uint16_t addr, uint8_t val, Spy *spy) {
    printf("[SPY] writing addr: %04x - value %d (dec) %c (s) \n", addr, val, (char)val);
}


// Function to create a Ram object and initialize the Space interface
Spy* CreateSpy() {
    Spy* spy = malloc(sizeof(Spy));

    return spy;
}