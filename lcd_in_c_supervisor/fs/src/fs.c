#include "fs.h"

#include <string.h>

bool fs_format(Disk *disk) {
  Inode *inode_ptr;
  Block *block;
  uint8_t i = 0;
  uint8_t j = 0;

  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return false;
  }
  memset(block, 0, sizeof(Block));

  block->super.magic_number = MAGIC_NUMBER;
  block->super.blocks = 10;
  block->super.inode_blocks = 1;
  block->super.inodes = 0;

  disk_write(disk, 0, block->data);
  memset(block, 0, sizeof(Block));

  for (i = 0; i < INODES_PER_BLOCK; i++) {
    inode_ptr = &(block->inodes[i]);
    inode_ptr->valid = false;
    inode_ptr->size = 0;
    inode_ptr->name[0] = 'r';
    inode_ptr->name[1] = 'o';
    inode_ptr->name[2] = 'o';
    inode_ptr->name[3] = 't';
    inode_ptr->name[4] = '-';
    inode_ptr->name[5] = i + 65;
    inode_ptr->name[6] = '\0';

    for (j = 0; j < POINTERS_PER_INODE; j++) {
      inode_ptr->direct[j] = 0;
    }
  }

  disk_write(disk, 1, block->data);
  free(block);

  return true;
}