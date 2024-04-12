#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cc65.h>

extern void __fastcall__  serial_write(char*);
extern void __fastcall__  serial_read(char*);

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

Disk *  disk_open(const char *path, size_t blocks);
void    disk_close(Disk *disk);
uint8_t disk_read(Disk *disk, size_t block, char *data);
uint8_t disk_write(Disk *disk, size_t block, char *data);

void main(void) {
    Disk* disk = malloc(sizeof(Disk));
    char* data = malloc(255 *sizeof(char));
    disk_read(disk, 1, data);
}

uint8_t disk_read(Disk *disk, size_t block, char *data) {
    // RD_CMD_START = <START_HEADING>R<SECTOR><END_HEADING>
    // <SECTOR> - 8 bits
    // startOfHeading = 0b00000001
	// endOfHeading   = 0b00000011
	// readOp         = 0b00000100
    
    uint8_t cmd[5] = {1, (uint8_t)'R', 1, 3, 0};
    serial_write(cmd);
    serial_read(data);
    return 0;
}
