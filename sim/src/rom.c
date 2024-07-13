#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "devices.h"
#include "log.h"

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
    uint16_t offset = addr - ROM_START_ADDR;
    log_debug("[ROM] offset addr: %04x", offset);
    if (offset < ROM_SIZE) {
        uint8_t val = rom->data[offset];
        log_debug("[ROM] reading addr: %04x - value: %02x", addr, val);
        return val;
    } else {
        log_debug("[ROM] reading addr: %04x - out of range", addr);
        exit(EXIT_FAILURE);        
    }
}

// Implement the Write method (panics as the ROM is read-only)
void Rom_Write(uint16_t addr, uint8_t val, Rom* rom) {
    log_debug("Address: %04x", addr);
    log_debug("ROM is read-only\n");
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
        log_error("load %ld instead of %d", bytesRead, ROM_SIZE);
        perror("error reading file");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

Rom* CreateRom(char* imagePath) {
    Rom* rom = (Rom*) malloc(sizeof(Rom));
    LoadFromFile(rom, imagePath);

    return rom;
}
