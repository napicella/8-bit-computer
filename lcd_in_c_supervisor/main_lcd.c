#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cc65.h>
#include <_heap.h>
#include <string.h>

#include "disk.h"
#include "um245.h"
#include "fs.h"
#include "lcd.h"

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
    // size_t h = _heapmemavail();
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
        return;
    }
    if (data == NULL) {
        *((uint8_t*) 0xA000) = 'N';
        return;
    }
    formatted = fs_format(disk);
    if (!formatted) {
        *((uint8_t*) 0xA000) = 'F';
        return;
    }
    filename = (char*)malloc(INODE_NAME_MAX * sizeof(char));
    fs_root_read(disk, filename);
    lcd_init();
    lcd_clear();
    lcd_print(filename);
    // *((uint8_t*) 0xA000) = filename[0];
    // *((uint8_t*) 0xA000) = filename[1];
    free(filename);
    free(disk);
    free(data);
}

