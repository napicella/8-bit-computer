#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "bus.h"
#include "devices.h"
#include "vrEmu6502.h"

Bus* bus;

uint8_t My6502MemoryReadFunction(uint16_t addr, bool isDbg) {
  return Bus_Read(addr, bus);
}

void My6502MemoryWriteFunction(uint16_t addr, uint8_t val) {
  Bus_Write(addr, val, bus);
}

/* fill rom with something that makes sense here */
int main() {
  bus = malloc(sizeof(Bus));
  bus->ram = CreateRam();
  bus->rom = CreateRom();
  bus->spy = CreateSpy();
  bus->um245 = CreateUm245();

  // /* create a new WDC 65C02. */
  VrEmu6502 *my6502 = vrEmu6502New(CPU_W65C02, My6502MemoryReadFunction, My6502MemoryWriteFunction);

  if (my6502) {
    /* if you want to interrupt the CPU, get a handle to its IRQ "pin" */
    vrEmu6502Interrupt *irq = vrEmu6502Int(my6502);

    /* reset the cpu (technically don't need to do this as vrEmu6502New does
     * reset it) */
    //vrEmu6502Reset(my6502);

    while (1) {
      /* call me once for each clock cycle (eg. 1,000,000 times per second for a
       * 1MHz clock) */
      //uint8_t opCode = vrEmu6502GetNextOpcode(my6502);
      //printf("%s\n", vrEmu6502OpcodeToMnemonicStr(my6502, opCode));
      vrEmu6502Tick(my6502);

      uint8_t op = vrEmu6502GetCurrentOpcode(my6502);
      const char* opString = vrEmu6502OpcodeToMnemonicStr(my6502, op);
      printf("[OP] %s\n", opString); 
      if (strcmp(opString, "brk") == 0) {
        break;
      }

      //   /* interrupt it? */
      //   if (myHardwareWantsAttention) {
      //     *irq = IntRequested;

      //     /* at some point, the hardware will be happy and it will need to
      //     release
      //      * the interrupt */
      //   }
    }

    vrEmu6502Destroy(my6502);
  }

  return 0;
}