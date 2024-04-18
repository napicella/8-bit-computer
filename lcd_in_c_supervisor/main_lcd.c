#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cc65.h>
#include <_heap.h>
#include <string.h>

#include "disk.h"
#include "um245.h"
#include "fs.h"

extern void __fastcall__  lcd_init();
extern void __fastcall__  lcd_print(char*);
extern void __fastcall__  lcd_clear();

void setMode() {
    uint8_t i = 0;
    char cmdMode[] = {'c', 'm','d','m','o','d','e'};
    for (i = 0; i < 7; i++) {
        serial_write(cmdMode[i]);
    }
}

ssize_t fs_root_read(Disk* disk, char *data) {
  Inode *inode_ptr;
  Block *block;
    
  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return false;
  }
  memset(block, 0, sizeof(Block));
  disk_read(disk, 1, block->data);
  inode_ptr = &(block->inodes[0]);
  
  strcpy(data, inode_ptr->name);
}

void main(void) {
    size_t size = BLOCK_SIZE *sizeof(uint8_t);
    Disk* disk = (Disk*)malloc(sizeof(Disk));
    char* data = (char*)malloc(size);
    uint16_t i =0;
    bool formatted;
    char* filename;
    char mex[10] = {'c', 'i', 'a', 'o', '\0'};

    // size_t h = _heapmemavail();
   setMode();


    // x = (uint16_t)__heapptr;
    // if (__heapptr == NULL ){
    //     *((uint8_t*) 0xA000) = 'Z';
        
    //     // *((uint8_t*) 0xA000) = x & 0x00FF;
    //     // *((uint8_t*) 0xA000) = x & 0xFF00;
    //     return;
    // } else {
    //     *((uint8_t*) 0xA000) = h;
    // }


    if (disk == NULL) {
        *((uint8_t*) 0xA000) = 'N';
        // x = (uint16_t)data;
        // a[0] = ((uintptr_t) data & (uintptr_t) 0x00ff);
        // a[0] = ((uintptr_t) data & (uintptr_t) 0xff00);
        //*((uint8_t*) 0xA000) = x & 0xFF00;
        return;
    }

    if (data == NULL) {
        *((uint8_t*) 0xA000) = 'N';
        // x = (uint16_t)data;
        // a[0] = ((uintptr_t) data & (uintptr_t) 0x00ff);
        // a[0] = ((uintptr_t) data & (uintptr_t) 0xff00);
        //*((uint8_t*) 0xA000) = x & 0xFF00;
        return;
    }

    formatted = fs_format(disk);
    if (!formatted) {
        *((uint8_t*) 0xA000) = 'F';
    }
    filename = (char*)malloc(INODE_NAME_MAX * sizeof(char));
    fs_root_read(disk, filename);
    lcd_init();
    lcd_clear();
    
    lcd_print(mex);
    lcd_print(filename);
    free(filename);

    // disk_read(disk, 1, data);

    // data[0] = 'h';
    // data[1] = 'e';
    // data[2] = 'l';
    // data[3] = 'l';
    // data[4] = 'o';
    // data[5] = ' ';
    // data[6] = 'm';
    // data[7] = 'o';
    // data[8] = 'm';
    // for(i=9;i<BLOCK_SIZE;i++) {
    //     data[i] = 'b';
    // }
    // disk_write(disk, 1, data);
    free(disk);
    free(data);
    setMode();
}

