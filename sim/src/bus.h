#include "devices.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define BUS_SIZE 0x10000


typedef struct {
    Ram*   ram;
    Rom*   rom;
    Spy*   spy;
    Um245* um245;
} Bus;


uint8_t Bus_Read(uint16_t addr, Bus *bus);
void Bus_Write(uint16_t addr, uint8_t val, Bus *bus);