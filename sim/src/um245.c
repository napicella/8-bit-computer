#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#include "devices.h"
#include "vremu6522_wrapper.h"

// VIA_PORT_B      -> UM245R_DATA
// VIA_PORT_A [A7] -> UM245R_WE#
// VIA_PORT_A [A6] -> UM245R_RD#
// VIA_PORT_A [A5] -> UM245R_TXE
// VIA_PORT_A [A4] -> UM245R_RXE

#define VIA_REG_PORT_B 0x00
#define VIA_REG_PORT_A 0x01

typedef enum {
  UNDEFINED_STATE,  
  READ_HIGH,
  READ_LOW,
  WRITE_HIGH,
  WRITE_LOW,
} um245Status;

uint8_t pipeRead(Um245* um) {
  uint8_t data[1];
  int n = read(um->inFd, data, 1);
  if (n == -1) {
    printf("Error reading from pipe in um245\n");
    exit(1);
  }
  if (n == 0) {
    printf("EOF reading from pipe um245\n");
  }
  return data[0];
}

void pipeWrite(Um245* um, uint8_t val) {
  uint8_t data[1];
  data[0] = val;
  write(um->outFd, data, 1);
}

void updateState(Um245* um, int val) {
   um->prevState = um->state;
   if(val & (1<<6))  {
    um->state = READ_HIGH;
   } else {
    um->state = READ_LOW;
   }

   if(val & (1<<7))  {
    um->state = WRITE_HIGH;
   } else {
    um->state = WRITE_LOW;
   }
}

Um245* CreateUm245(VrEmu6522* via) {
  Um245* um245 = (Um245*)malloc(sizeof(Um245));

  int fd;
  char* fifoOut = "/tmp/fifo_out";
  // mkfifo(fifoOut, 0666);
  fd = open(fifoOut, O_CREAT | O_WRONLY);
  um245->outFd = fd;

  char* fifoIn = "/tmp/fifo_in";
  // mkfifo(fifoIn, 0666);
  fd = open(fifoIn, O_CREAT | O_RDONLY | O_NONBLOCK);
  um245->inFd = fd;

  um245->prevState = UNDEFINED_STATE;
  um245->state = UNDEFINED_STATE;
  um245->via = via;

  return um245;
}

// only portA
uint16_t Um245_Start() { return 0x8001; }

uint32_t Um245_Length() { return 1; }

uint8_t Um245_Read(uint16_t add, Um245* um)  {
  uint8_t b =  0xFF & add;
  if (b != VIA_REG_PORT_A) {
    return 0;
  }

  // TODO:
  // this means mcu can always read or write (needs to be fixed)
  return 1;
}

void Um245_Write(uint16_t add, uint8_t val, Um245* um) {
  uint8_t b =  0xFF & add;

  if (b != VIA_REG_PORT_A) {
    return;
  }

  updateState(um, val);
  if (um->prevState == READ_HIGH && um->state == READ_LOW) {
    // mcu is requesting to read
    uint8_t d = pipeRead(um);
    vrEmu6522Write(um->via, VIA_REG_PORT_B, d);
    return;
  } 

  if (um->prevState == WRITE_LOW && um->state == WRITE_HIGH) {
    // mcu is requesting to write
    uint8_t d = vrEmu6522Read(um->via, VIA_REG_PORT_B);
    pipeWrite(um, d);
    return;
  }
}

void Um245_Close(Um245* um) {
  close(um->outFd);
  close(um->inFd);
}
