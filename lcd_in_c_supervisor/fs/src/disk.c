#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "disk.h"
#include "um245.h"

ssize_t disk_read(Disk *disk, size_t block,char *data) {
    // RD_CMD_START = <START_HEADING>R<SECTOR><END_HEADING>
    // <SECTOR> - 8 bits
    // startOfHeading = 0b00000001
	// endOfHeading   = 0b00000011
	// readOp         = 0b00000100
    
    uint8_t cmd[4]; 
    uint16_t i = 0;
    char out;

    cmd[0] = 1;
    cmd[1] = (uint8_t)'R';
    cmd[2] = block;
    cmd[3] = 3;

    for (i = 0; i < 4; i++) {
        serial_write(cmd[i]);
    }
    for (i = 0; i < BLOCK_SIZE; i++) {
        //*(data + i) = serial_read_byte();
        out = serial_read_byte();
        data[i] = out;
        //*((uint8_t*) 0xA000) = 1;
    }
    return 0;
}

ssize_t	disk_write(Disk *disk, size_t block, char *data){
    // WR_CMD_START = <START_HEADING>W<SECTOR><DATA><END_HEADING>
    // <SECTOR> - 8 bits
    
    uint8_t cmdStart[3];
    uint8_t cmdEnd[1] = {3};
    uint16_t i = 0;
    uint8_t x[1] = {97};

    cmdStart[0] = 1;
    cmdStart[1] = (uint8_t)'W';
    cmdStart[2] = block;

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