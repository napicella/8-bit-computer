#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "devices.h"
#include "labelmap.h"
#include "log.h"
#include "vremu6502_wrapper.h"
#include "vremu6522_wrapper.h"

Bus* bus;
bool stepDebuggerEnabled = false;

uint8_t My6502MemoryReadFunction(uint16_t addr, bool isDbg) {
  return Bus_Read(addr, bus);
}

void My6502MemoryWriteFunction(uint16_t addr, uint8_t val) {
  Bus_Write(addr, val, bus);
}

int main() {
  log_set_level(LOG_FATAL);

  char labelFilepath[256];
  strcat(strcpy(labelFilepath, getenv("HOME")),
         "/github/8-bit-computer/scheduler/bin/scheduler.lbl");

  char imageFilepath[256];
  strcat(strcpy(imageFilepath, getenv("HOME")),
         "/github/8-bit-computer/scheduler/bin/scheduler.bin");

  char* labelContent = readFile(labelFilepath);
  char* labelMap[0x10000] = {NULL};
  debuggerLoadLabels(labelContent, labelMap);

  bus = (Bus*)malloc(sizeof(Bus));
  bus->ram = CreateRam();
  bus->rom = CreateRom(imageFilepath);
  bus->spy = CreateSpy();
  bus->um245 = CreateUm245();
  bus->via = Create6522();

  VrEmu6502* emu6502 = vrEmu6502New(CPU_W65C02, My6502MemoryReadFunction,
                                    My6502MemoryWriteFunction);

  if (!emu6502) {
    log_error("failed to create emulator");
    exit(-1);
  }

  vrEmu6522Interrupt viaIntSignal;

  // to interrupt the CPU, get a handle to its IRQ "pin"
  vrEmu6502Interrupt* irq;
  bool instructionCompleted = true;

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
    vrEmu6502Tick(emu6502);
    vrEmu6522Tick(bus->via);

    uint8_t cycle = vrEmu6502GetOpcodeCycle(emu6502);
    uint8_t op = vrEmu6502GetCurrentOpcode(emu6502);
    const char* opString = vrEmu6502OpcodeToMnemonicStr(emu6502, op);

    char buffer[100];
    vrEmu6502DisassembleInstruction(emu6502,
                                    vrEmu6502GetCurrentOpcodeAddr(emu6502), 100,
                                    buffer, NULL, labelMap);

    if (instructionCompleted) {
      log_info("[DIS] %s\n-------------\n", buffer);

      if (strcmp(opString, "nop") == 0 || stepDebuggerEnabled) {
        stepDebuggerEnabled = true;
        // simple debugger loop
        uint8_t currentIndex = 0;
        char line[64];
        printf("> ");

        while (true) {
          char c = getchar();
          line[currentIndex++] = c;
          if (c == '\n') {
            int res = scan(line, emu6502, bus);
            currentIndex = 0;
            memset(line, 0, 64);

            if (res == 0) {
              break;
            }
            printf("> ");
          }
        }
      }
    }
    // instructionCompleted switch in the cycle where the CPU starts an
    // operation
    instructionCompleted = cycle == 0;

    if (strcmp(opString, "brk") == 0) {
      break;
    }

    irq = vrEmu6502Int(emu6502);
    viaIntSignal = *(vrEmu6522Int(bus->via));
    if (viaIntSignal == vrEmu6522_IntRequested) {
      *irq = vrEmu6502_IntRequested;
    } else if(viaIntSignal == vrEmu6522_IntCleared) {
      *irq = vrEmu6502_IntCleared;
    }
  }

  vrEmu6502Destroy(emu6502);
  return 0;
}

// text must be a NIL terminated string
int scan(char* text, VrEmu6502* emu6502, Bus* bus) {
  // supported commands:
  //
  // print-stack
  // peek <MEM>
  // a
  // x
  // y
  // c (continue)
  // r (resume)

  if (strncmp(text, "print-stack", 11) == 0) {
    // stack in 6502 goes from 0x0100 to 0xFF
    for (uint16_t i = 0x1FF; i >= 0x100; i--) {
      uint8_t data = Bus_Read(i, bus);
      log_info("[S] [%x] %x", i, data);
    }
  } else if (strncmp(text, "peek", 4) == 0) {
    uint16_t address;
    int scan_res = sscanf(text, "peek %hx", &address);
    if (scan_res >= 1) {
      uint8_t data = Bus_Read(address, bus);
      log_info("[PEEK] %x", data);
    }
  } else if (strncmp(text, "status", 6) == 0) {
    uint8_t intMask = 0x01 << BitI;
    uint8_t spReg = vrEmu6502GetStatus(emu6502);
    if (spReg & intMask) {
      printf("Interrupts disabled\n");
    } else {
      printf("Interrupts enabled\n");
    }
    
    // Carry bit (C) is the LSB in the spReg
    // const char* status[] = {"N", "V", "-", "B", "D", "I - IRQ disable", "Z", "C"};
  } else if (strncmp(text, "sp", 2) == 0) {
    uint8_t spReg = vrEmu6502GetStackPointer(emu6502);
    uint8_t data = Bus_Read(0x100 + spReg + 1, bus);
    log_info("[SP] %x", 0x100 + spReg);
    log_info("[PEEK] %b", data);
  } else if (strncmp(text, "a", 1) == 0) {
    uint8_t accReg = vrEmu6502GetAcc(emu6502);
    log_info("[A] %x", accReg);
  } else if (strncmp(text, "x", 1) == 0) {
    uint8_t xReg = vrEmu6502GetX(emu6502);
    log_info("[X] %x", xReg);
  } else if (strncmp(text, "y", 1) == 0) {
    uint8_t yReg = vrEmu6502GetY(emu6502);
    log_info("[Y] %x", yReg);
  } else if (strncmp(text, "c", 1) == 0) {
    return 0;
  } else if (strncmp(text, "r", 1) == 0) {
    stepDebuggerEnabled = false;
    return 0;
  } else {
    return -1;
  }

  return -1;
}