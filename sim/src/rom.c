#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "devices.h"

// Implement the Start method
uint16_t Rom_Start() {
    return ROM_START_ADDR;
}

// Implement the Length method
uint32_t Rom_Length() {
    return ROM_SIZE;
}

// Implement the Read method
uint8_t Rom_Read(uint16_t addr, Rom *rom) {
    uint16_t offset = addr - Rom_Start();
    if (offset < ROM_SIZE) {
        uint8_t val = rom->data[offset];
        printf("[ROM] reading addr: %04x - value: %02x \n", addr, val);
        return val;
    } else {
        printf("[ROM] reading addr: %04x - out of range\n", addr);
        exit(EXIT_FAILURE);        
    }
}

// Implement the Write method (panics as the ROM is read-only)
void Rom_Write(uint16_t addr, uint8_t val, Rom* rom) {
    printf("Address: %04x\n", addr);
    printf("ROM is read-only\n");
    exit(EXIT_FAILURE);
}

void LoadFromFile(Rom *rom, const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read ROM content from file into memory array
    size_t bytesRead = fread(rom->data, sizeof(uint8_t), ROM_SIZE + 1, file);
    if (bytesRead != ROM_SIZE) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

Rom* CreateRom() {
    Rom* rom = malloc(sizeof(Rom));
    char file[256]; 
    strcat(strcpy(file, getenv("HOME")), "/github/8-bit-computer/lcd_in_c/bin/kernel.bin");
    LoadFromFile(rom, file);

    return rom;
}
