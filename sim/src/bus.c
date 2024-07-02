#include "bus.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "devices.h"
#include "vremu6522_wrapper.h"

int between(uint16_t val, uint16_t start, uint32_t length) {
  if (val >= start && val < start + length) {
    return 1;
  }
  return 0;
}

// Implement the Read method for Bus
uint8_t Bus_Read(uint16_t addr, Bus* bus) {
  printf("[BUS] [R] %x\n", addr);

  if (between(addr, Ram_Start(), Ram_Length())) {
    return Ram_Read(addr, bus->ram);
  }
  if (between(addr, Rom_Start(), Rom_Length())) {
    return Rom_Read(addr, bus->rom);
  }
  if (between(addr, Spy_Start(), Spy_Length())) {
    return Spy_Read(addr, bus->spy);
  }
  if (between(addr, Um245_Start(), Um245_Length())) {
    return Um245_Read(addr, bus->um245);
  }
  if (between(addr, Via6522_Start(), Via6522_Length())) {
    return vrEmu6522Read(bus->via, addr & 0xFF);
  }
  fprintf(stderr, "no device associated to address %04x\n", addr);
  exit(EXIT_FAILURE);
}

// Implement the Write method for Bus
void Bus_Write(uint16_t addr, uint8_t val, Bus* bus) {
  printf("[BUS] [W] %x\n", addr);

  if (between(addr, Ram_Start(), Ram_Length())) {
    Ram_Write(addr, val, bus->ram);
    return;
  }
  if (between(addr, Rom_Start(), Rom_Length())) {
    Rom_Write(addr, val, bus->rom);
    return;
  }
  if (between(addr, Spy_Start(), Spy_Length())) {
    Spy_Write(addr, val, bus->spy);
    return;
  }
  if (between(addr, Um245_Start(), Um245_Length())) {
    Um245_Write(addr, val, bus->um245);
    return;
  }
  if (between(addr, Via6522_Start(), Via6522_Length())) {
    vrEmu6522Write(bus->via, addr & 0xFF, val);
    return;
  }
  fprintf(stderr, "no device associated to address %04x\n", addr);
  exit(EXIT_FAILURE);
}