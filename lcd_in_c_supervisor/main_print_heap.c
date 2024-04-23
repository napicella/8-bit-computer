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

void main(void) {
  size_t memAvail;
  char* data = (char*)malloc(sizeof(char) * 20);
  char* curr;
  char currChar;
  int i = 0;

  int currentIndex = 0;
  char* buff = (char*)malloc(sizeof(char) * 20);

  
  serial_write('/');
  serial_write('\n');

  while (true) {
    currChar = serial_read_byte();
    buff[currentIndex] = currChar;
    serial_write(currChar);  // echo
    currentIndex++;
    if (currChar == '\r') {
      // terminal sends LF on Enter key

      // write newline
      serial_write('\n');

      if (strncmp(buff, "info\r", 5) == 0) {
        memAvail = __heapmemavail();
        sprintf(data, "heapmem: %x\n\r\0", memAvail);
        currentIndex = 0;
        curr = data;
        while (curr != NULL && *curr != '\0') {
          serial_write(*curr);
          curr = curr + 1;
        }
      }
    }
  }
}
