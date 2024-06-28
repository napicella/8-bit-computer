#ifndef DEVICES_HEADER_H
#define DEVICES_HEADER_H

#include "vremu6522_wrapper.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Define the Ram type
//
#define RAM_SIZE (0x7FFF + 1)

typedef struct {
  uint8_t data[RAM_SIZE];
} Ram;

Ram* CreateRam();
uint16_t Ram_Start();
uint32_t Ram_Length();
uint8_t Ram_Read(uint16_t addr, Ram *ram);
void Ram_Write(uint16_t addr, uint8_t val, Ram *ram);

// Define the Rom type
//
//#define ROM_START_ADDR 0xE000
#define ROM_START_ADDR 0x8C00
//#define ROM_SIZE 0x2000
#define ROM_SIZE 0x8000

typedef struct {
  uint8_t data[ROM_SIZE];
} Rom;

Rom* CreateRom();
uint16_t Rom_Start();
uint32_t Rom_Length();
uint8_t Rom_Read(uint16_t addr, Rom *rom);
void Rom_Write(uint16_t addr, uint8_t val, Rom *rom);

// Define the Spy type
//
typedef struct {
} Spy;

Spy* CreateSpy();
uint16_t Spy_Start();
uint32_t Spy_Length();
uint8_t Spy_Read(uint16_t addr, Spy *spy);
void Spy_Write(uint16_t addr, uint8_t val, Spy *spy);

// Define the Um245 type
//
#define UM_START_ADDR 0x8400
#define UM_SIZE 0x0800
#define UART_WRITE  0b1000010000000001
#define UART_READ   0b1000010000000010
#define UART_STATUS 0b1000010000000100

#define UART_WRITE_BUSY_MASK   0b00000001
#define UART_READ_NO_DATA_MASK 0b00000010

typedef struct {
  uint8_t* data;
  size_t size;
  size_t capacity;
} CmdBuffer;

typedef struct {
  int outFd;
  int inFd;
} Um245;

Um245* CreateUm245();
uint16_t Um245_Start();
uint32_t Um245_Length();
uint8_t Um245_Read(uint16_t addr, Um245 *spy);
void Um245_Write(uint16_t addr, uint8_t val, Um245 *spy);

VrEmu6522* Create6522();
uint16_t Via6522_Start();
uint32_t Via6522_Length();


#endif