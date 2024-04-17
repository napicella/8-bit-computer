#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    FILE* f;
} Disk;

void disk_close(Disk* disk) {
    fclose(disk->f);
}

Disk* disk_init() {
    Disk* disk = (Disk*)malloc(sizeof(Disk));
    const char* name = "disk.bin";
    FILE* f = fopen(name, "wb+");
    if (f == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    uint8_t* data = (uint8_t*)malloc(2048 * sizeof(uint8_t));
    memset(data, 0xFF, 2048);
    fwrite(data, sizeof(uint8_t), 2048, f);
    free(data);

    disk->f = f;
    return disk;
}

void disk_write(Disk* disk, uint8_t sector, uint8_t* data, size_t size) {
    fseek(disk->f, sector * 255, SEEK_SET);
    fwrite(data, sizeof(uint8_t), size, disk->f);
}

void disk_read(Disk* disk, uint8_t sector, uint8_t* data, size_t size) {
    fseek(disk->f, sector * 255, SEEK_SET);
    fread(data, sizeof(uint8_t), size, disk->f);
}