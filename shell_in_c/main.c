#include <_heap.h>
#include <cc65.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "fs.h"
#include "lcd.h"
#include "um245.h"
// #include "io.h"

extern uint16_t __fastcall__ wozmon_address();
extern uint16_t __fastcall__ wozmon_run();

Disk* disk;
FileSystem* fs;

void info();
void shell_fs_print_info();
void shell_fs_create();
// void shell_led_on();
// void shell_led_off();
void sampleString();

void main(void) {
  char motd[] = "*\n**\n**** pactvm6502 v0.1\n**\n*\n";
  char currChar;
  int currentIndex = 0;
  char* buff = (char*)malloc(sizeof(char) * 20);
  serial_writeline(motd);

  serial_writeline("\n> ");
  while (true) {
    currChar = serial_readbyte();
    buff[currentIndex] = currChar;
    serial_writebyte(currChar);  // echo
    currentIndex++;
    if (currChar == '\r') {
      // terminal sends LF on Enter key

      // write newline
      serial_writebyte('\n');
      currentIndex = 0;

      if (strncmp(buff, "info\r", 5) == 0) {
        info();
        serial_writeline("\n> ");
        continue;
      }
      if (strncmp(buff, "fsinfo\r", 8) == 0) {
        shell_fs_print_info();
        serial_writeline("\n> ");
        continue;
      }
      if (strncmp(buff, "fscreate\r", 10) == 0) {
        shell_fs_create();
        serial_writeline("\n> ");
        continue;
      }
      // if (strncmp(buff, "ledon\r", 7) == 0) {
      //   shell_led_on();
      //   serial_writeline("\n> ");
      //   continue;
      // }
      // if (strncmp(buff, "ledoff\r", 8) == 0) {
      //   shell_led_off();
      //   serial_writeline("\n> ");
      //   continue;
      // }
      if (strncmp(buff, "wozmon\r", 7) == 0) {
        wozmon_run();
      }
    }
  }
}

void info() {
  size_t memAvail = __heapmemavail();
  char* data = (char*)malloc(sizeof(char) * 25);
  sprintf(data, "heap_available: %d\n", memAvail);

  serial_writeline(data);
  free(data);
}

void shell_fs_print_info() {
  disk = (Disk*)malloc(sizeof(Disk));
  if (disk == NULL) {
    *((uint8_t*)0xA000) = 'N';
    return;
  }

  fs = (FileSystem*)malloc(sizeof(Disk));
  if (fs == NULL) {
    *((uint8_t*)0xA000) = 'N';
    return;
  }
  fs_format(disk);
  fs_mount(fs, disk);
  fs_info(fs);
}

void shell_fs_create() {
  int inode;
  char* data = (char*)malloc(sizeof(char) * 128);
  inode = fs_create(fs);
  if (inode == -1) {
    serial_writeline("failed to get an inode\n");
    return;
  }
  sprintf(data, "inode number: %d\n", inode);
  serial_writeline(data);
}

// void shell_led_on() {
//   ledOn();
// }

// void shell_led_off() {
//   ledOff();
// }


void sampleString() {
  char* data = (char*)malloc(sizeof(char) * 128);
  int x = 1024;
  if (data == NULL) {
    return;
  }

  sprintf(data, "%d %d %x", x, x, x);
  *((uint8_t*)0xA000) = data[0];
  *((uint8_t*)0xA000) = data[1];
  *((uint8_t*)0xA000) = data[2];
  *((uint8_t*)0xA000) = data[3];

  serial_writeline(data);
  free(data);
}