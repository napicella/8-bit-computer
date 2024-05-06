#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "disk.h"

const char disk_path[] = "/tmp/disk";

ssize_t disk_read(Disk* disk, size_t block, char* data) {
  FILE* fptr;
  fptr = fopen(disk_path, "r");

  // checking if the file is opened successfully
  if (fptr == NULL) { 
    return -1;
  }
  fseek(fptr,block*BLOCK_SIZE ,SEEK_SET);
  fread(data, BLOCK_SIZE, 1, fptr);
  fclose(fptr);
}

ssize_t	disk_write(Disk *disk, size_t block, char *data) {
  FILE* fptr;
  fptr = fopen(disk_path, "w");

  // checking if the file is opened successfully
  if (fptr == NULL) { 
    return -1;
  }
  fseek(fptr,block*BLOCK_SIZE ,SEEK_SET);
  fwrite(data, BLOCK_SIZE, 1, fptr);
  fclose(fptr);

  return 0;
}

void fs_debug_print(const char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  vprintf(fmt, arg);
  va_end(arg);
}
