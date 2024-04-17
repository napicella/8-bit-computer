#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cc65.h>
#include <_heap.h>

extern void __fastcall__  serial_write(uint8_t);
extern void __fastcall__  serial_read(char*);
extern uint8_t __fastcall__  serial_read_byte();

/* Disk Structure */

typedef uint8_t bool;
typedef struct Disk Disk;
struct Disk {
    int     fd;         /* File descriptor of disk image        */
    size_t  blocks;     /* Number of blocks in disk image       */
    size_t  reads;      /* Number of reads to disk image        */
    size_t  writes;     /* Number of writes to disk image       */
    bool    mounted;    /* Whether or not disk is mounted       */
};

/* Disk Functions */

//Disk *  disk_open(const char *path, size_t blocks);
//void    disk_close(Disk *disk);
uint8_t disk_read(size_t block, int *data);
uint8_t disk_write(uint8_t block, int *data);

#define BLOCK_SIZE 512

void main(void) {
    size_t size = BLOCK_SIZE *sizeof(uint8_t);
    Disk* disk = malloc(sizeof(Disk));
    int* data = malloc(size);
    uint16_t i =0;
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

    disk_read(1, data);

    data[0] = 'h';
    data[1] = 'e';
    data[2] = 'l';
    data[3] = 'l';
    data[4] = 'o';
    data[5] = ' ';
    data[6] = 'm';
    data[7] = 'o';
    data[8] = 'm';
    for(i=9;i<BLOCK_SIZE;i++) {
        data[i] = 'b';
    }
    disk_write(1, data);
    free(disk);
    free(data);
}

uint8_t disk_read(size_t block, int *data) {
    // RD_CMD_START = <START_HEADING>R<SECTOR><END_HEADING>
    // <SECTOR> - 8 bits
    // startOfHeading = 0b00000001
	// endOfHeading   = 0b00000011
	// readOp         = 0b00000100
    
    uint8_t cmd[4] = {1, (uint8_t)'R', 1, 3};
    uint16_t i = 0;
    char out;
    for (i = 0; i < 4; i++) {
        serial_write(cmd[i]);
    }
    for (i = 0; i < 20; i++) {
        //*(data + i) = serial_read_byte();
        out = serial_read_byte();
        data[i] = out;
        //*((uint8_t*) 0xA000) = 1;
    }
    return 0;
}

uint8_t disk_write(uint8_t block, int *data) {
    // WR_CMD_START = <START_HEADING>W<SECTOR><DATA><END_HEADING>
    // <SECTOR> - 8 bits
    
    uint8_t cmdStart[3] = {1, (uint8_t)'W', 1};
    uint8_t cmdEnd[1] = {3};
    uint16_t i = 0;
    uint8_t x[1] = {97};

    //data[0] = 'c';
    // *((uint8_t*) 0xA001) = data[0];
    // *((uint8_t*) 0xA001) = data[1];
    // *((uint8_t*) 0xA001) = data[2];
    // *((uint8_t*) 0xA001) = data[3];

    for (i = 0; i < 3; i++) {
        serial_write(cmdStart[i]);
    }
    
    
    for (i = 0; i < BLOCK_SIZE; i++) {
        serial_write(data[i]);
    }
    
    for (i = 0; i < 1; i++) {
        serial_write(cmdEnd[i]);
    }
    
    return 0;
}

