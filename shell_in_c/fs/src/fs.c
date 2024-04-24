#include "fs.h"

#include <stdio.h>
#include <string.h>

#include "um245.h"

#define BLOCK_NUM (128)
#define INODE_BLOCKS (50)
#define DATA_BOCK_NUM (BLOCK_NUM - 1 - INODE_BLOCKS)
#define FIRST_INODE_BLOCK (1)
#define LAST_INODE_BLOCK (INODE_BLOCKS)
#define FIRST_DATA_BLOCK (1 + LAST_INODE_BLOCK)
#define LAST_DATA_BLOCK (BLOCK_NUM - 1)

int fs_find_empty_block(FileSystem *fs);

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
  block->super.blocks = BLOCK_NUM;
  block->super.inode_blocks = INODE_BLOCKS;
  block->super.inodes = 0;

  disk_write(disk, 0, block->data);

  for (i = FIRST_INODE_BLOCK; i <= LAST_INODE_BLOCK; i++) {
    memset(block, 0, sizeof(Block));

    for (j = 0; j < INODES_PER_BLOCK; j++) {
      inode_ptr = &(block->inodes[j]);
      inode_ptr->valid = false;
      inode_ptr->size = 0;
      // inode_ptr->name[0] = 'r';
      // inode_ptr->name[1] = 'o';
      // inode_ptr->name[2] = 'o';
      // inode_ptr->name[3] = 't';
      // inode_ptr->name[4] = '-';
      // inode_ptr->name[5] = i + 65;
      // inode_ptr->name[6] = '\0';

      // for (j = 0; j < POINTERS_PER_INODE; j++) {
      //   inode_ptr->direct[j] = 0;
      // }
    }
    disk_write(disk, i, block->data);
  }

  free(block);

  return true;
}

bool fs_read_block(Disk *disk, Block *block, size_t blockNum) {
  memset(block, 0, sizeof(Block));
  disk_read(disk, blockNum, block->data);

  return true;
}

bool *fs_get_blocks_bitmap(FileSystem *fs) {
  Block *block;
  Inode *inodePtr;
  bool *blocksBitmap;
  bool readSuccess;
  int currBlock;
  int i;
  int currDirect;

  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return NULL;
  }

  // Initialize all the block as free in the bitmap.
  //
  // For simplicity during access, create a bitmap for each block of the disk,
  // though we only need to look up data block.
  // This way, we can loop up the block number directly in the map.
  //
  // The first [0..FIRST_DATA_BLOCK-1] are gonna be initialized as full (bit set
  // to false), from [FIRST_DATA_BLOCK-1..LAST_DATA_BLOCK] to empty (bit set to
  // true).
  blocksBitmap = (bool *)malloc(BLOCK_NUM * sizeof(bool));
  if (blocksBitmap == NULL) {
    return NULL;
  }
  for (i = 0; i <= FIRST_DATA_BLOCK - 1; i++) {
    blocksBitmap[i] = false;
  }
  for (i = FIRST_DATA_BLOCK; i <= LAST_DATA_BLOCK; i++) {
    blocksBitmap[i] = true;
  }

  for (currBlock = FIRST_INODE_BLOCK; currBlock <= LAST_INODE_BLOCK;
       currBlock++) {
    // inode blocks are stored after the super block
    // that is from 1 til the number of blocks reserved
    // for inodes.
    //
    // Each inode block contains a INODES_PER_BLOCK number
    // of inodes.

    readSuccess = fs_read_block(fs->disk, block, currBlock);
    if (!readSuccess) {
      return NULL;
    }
    for (i = 0; i < INODES_PER_BLOCK; i++) {
      inodePtr = &(block->inodes[i]);
      if (inodePtr->valid) {
        for (currDirect = 0; currDirect < POINTERS_PER_INODE; currDirect++) {
          blocksBitmap[inodePtr->direct[currDirect]] = false;
        }
      }
    }
  }

  free(block);
  return blocksBitmap;
}

bool fs_mount(FileSystem *fs, Disk *disk) {
  Block *block;

  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return false;
  }
  fs_read_block(disk, block, 0);

  fs->disk = disk;
  fs->meta_data = block->super;
  fs->free_blocks = fs_get_blocks_bitmap(fs);
  if (fs->free_blocks == NULL) {
    return false;
  }

  return true;
}

ssize_t fs_create(FileSystem *fs) {
  // find free inode in the fs
  int i = 0;
  int j = 0;
  int emptyBlockNum = -1;
  int inodesLength = fs->meta_data.inode_blocks * INODES_PER_BLOCK;
  Block *block;
  Inode *inode_ptr;

  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return -1;
  }

  for (i = FIRST_INODE_BLOCK; i <= LAST_INODE_BLOCK; i++) {
    fs_read_block(fs->disk, block, i);

    for (j = 0; j < INODES_PER_BLOCK; j++) {
      inode_ptr = &(block->inodes[j]);
      if (inode_ptr->valid) {
        continue;
      }
      inode_ptr->valid = true;
      inode_ptr->size = 0;
      inode_ptr->name[0] = 'r';
      inode_ptr->name[1] = 'o';
      inode_ptr->name[2] = 'o';
      inode_ptr->name[3] = 't';

      emptyBlockNum = fs_find_empty_block(fs);
      if (emptyBlockNum < 0) {
        return -1;
      }

      inode_ptr->direct[0] = emptyBlockNum;
      disk_write(fs->disk, i, block->data);
      fs->free_blocks[emptyBlockNum] = false;
      free(block);

      return INODES_PER_BLOCK * (i - FIRST_INODE_BLOCK) + j;

      // for (j = 0; j < POINTERS_PER_INODE; j++) {
      //    inode_ptr->direct[j] = 0;
      // }

      // inode_ptr->name[0] = 'r';
      // inode_ptr->name[1] = 'o';
      // inode_ptr->name[2] = 'o';
      // inode_ptr->name[3] = 't';
      // inode_ptr->name[4] = '-';
      // inode_ptr->name[5] = i + 65;
      // inode_ptr->name[6] = '\0';
    }
  }

  free(block);
  return -1;
}

int fs_find_empty_block(FileSystem *fs) {
  int blocks = fs->meta_data.blocks;
  int i = 0;
  for (i = 0; i < blocks; i++) {
    if (fs->free_blocks[i]) {
      return i;
    }
  }
  return -1;
}

bool fs_info(FileSystem *fs) {
  int freeBlocksCount = 0;
  int blocks;
  int inodesReserved = 0;
  int i = 0;
  char *data = (char *)malloc(sizeof(char) * 128);
  if (data == NULL) {
    return false;
  }

  blocks = fs->meta_data.blocks;
  inodesReserved = fs->meta_data.inode_blocks;
  for (i = 0; i < blocks; i++) {
    if (fs->free_blocks[i]) {
      freeBlocksCount++;
    }
  }

  sprintf(data, "Blocks %d\nInode_reserved: %d\nFree blocks: %d\n\r", blocks,
          inodesReserved, freeBlocksCount);

  serial_writeline(data);
  free(data);

  return true;
}