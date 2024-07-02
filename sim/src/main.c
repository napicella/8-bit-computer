#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "devices.h"
#include "vremu6502_wrapper.h"
#include "vremu6522_wrapper.h"

Bus* bus;

uint8_t My6502MemoryReadFunction(uint16_t addr, bool isDbg) {
  return Bus_Read(addr, bus);
}

void My6502MemoryWriteFunction(uint16_t addr, uint8_t val) {
  Bus_Write(addr, val, bus);
}

/* fill rom with something that makes sense here */
int main() {
  bus = (Bus*)malloc(sizeof(Bus));
  bus->ram = CreateRam();
  bus->rom = CreateRom();
  bus->spy = CreateSpy();
  bus->um245 = CreateUm245();
  bus->via = Create6522();

  // /* create a new WDC 65C02. */
  VrEmu6502* my6502 = vrEmu6502New(CPU_W65C02, My6502MemoryReadFunction,
                                   My6502MemoryWriteFunction);

  vrEmu6522Interrupt viaIntSignal;

  if (my6502) {
    // to interrupt the CPU, get a handle to its IRQ "pin"
    vrEmu6502Interrupt* irq = vrEmu6502Int(my6502);
    /* reset the cpu (technically don't need to do this as vrEmu6502New does
     * reset it) 
     */
    // vrEmu6502Reset(my6502);

    bool shouldDisplay = true;

    while (1) {
      /* call once for each clock cycle
       *
       * The way it works is that after calling tick the operation has already
       * been performed, thus querying the registers returns their value after
       * the operation has been performed. If the operation takes multiple
       * cycles (which most of the 6502 operations) the subsequent ticks
       * function calls will do nothing until the number of cycles necessary for
       * that operation have been performed, after which the cpu will start
       * fetching the next instruction.
       *
       * In essence, from an external device, the cpu emulation behaves like it
       * runs the operation cycle by cycle. It's only if you poke the emulated
       * hardware that you can realize that the operation has completed after
       * the first tick (cycle)
       *
       */
      vrEmu6502Tick(my6502);
      vrEmu6522Tick(bus->via);

      uint8_t cycle = vrEmu6502GetOpcodeCycle(my6502);
      uint8_t op = vrEmu6502GetCurrentOpcode(my6502);
      const char* opString = vrEmu6502OpcodeToMnemonicStr(my6502, op);
      if (shouldDisplay) {
        uint8_t accRegister = vrEmu6502GetAcc(my6502);
        printf("[OP] %s\n", opString);
        printf("[ACC] %x\n", accRegister);
      }
      // shouldDisplay switch in the cycle where the CPU starts an operation
      shouldDisplay = cycle == 0;

      if (strcmp(opString, "brk") == 0) {
        break;
      }

      viaIntSignal = *(vrEmu6522Int(bus->via));
      if (viaIntSignal == vrEmu6522_IntRequested) {
        *irq = vrEmu6502_IntRequested;
      }

      // printf("Press Any Key to Continue\n");
      // getchar();
    }

    vrEmu6502Destroy(my6502);
  }

  return 0;
}
