#include "fs.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

#define DATA_BOCK_NUM (BLOCK_NUM - 1 - INODE_BLOCKS)
#define FIRST_INODE_BLOCK (1)
#define LAST_INODE_BLOCK (INODE_BLOCKS)
#define FIRST_DATA_BLOCK (1 + LAST_INODE_BLOCK)
#define LAST_DATA_BLOCK (BLOCK_NUM - 1)

typedef struct find_result {
  Block *block;
  uint32_t block_num;
  int inode_offset;

} find_result;

bool fs_read_block(Disk *disk, Block *block, size_t blockNum);
ssize_t fs_write_data_block(FileSystem *fs, size_t block_num, char *data,
                            size_t length, size_t offset);
bool *fs_get_blocks_bitmap(FileSystem *fs);
int fs_find_empty_block(FileSystem *fs);
find_result *fs_find_inode(FileSystem *fs, size_t inode_number);
bool fs_info_inodes(FileSystem *fs, void (*visitor)(Inode *));
bool fs_find_inode_2(FileSystem *fs, size_t inode_number, Block *block);
int find_first(int32_t *nums, size_t length, int32_t to_find);
int min(int a, int b);

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
    return false;
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
    return false;
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
      return false;
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

find_result *new_find_result() {
  Block *block;
  find_result *result;

  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return NULL;
  }
  result = (find_result *)malloc(sizeof(find_result));
  if (result == NULL) {
    return NULL;
  }
  result->block = block;
  result->block_num = -1;
  result->inode_offset = -1;

  return result;
}

void destroy_find_result(find_result *result) {
  free(result->block);
  free(result);
}

// find_result *fs_find_inode(FileSystem *fs, size_t inode_number) {
//   bool success;
//   find_result *result = new_find_result();
//   if (result == NULL) {
//     return NULL;
//   }
//   size_t block_num = inode_number / INODES_PER_BLOCK;
//   size_t offset = inode_number % INODES_PER_BLOCK;
//   result->inode_offset = offset;
//   result->block_num = FIRST_INODE_BLOCK + block_num;
//   serial_fprintline("find_node block num %lu\n", result->block_num);

//   success = fs_read_block(fs->disk, result->block, block_num);
//   if (!success) {
//     destroy_find_result(result);
//     return NULL;
//   }
//   return result;
// }

bool fs_find_inode_2(FileSystem *fs, size_t inode_number, Block *block) {
  bool success;
  size_t block_num = FIRST_INODE_BLOCK + (inode_number / INODES_PER_BLOCK);

  fs_debug_print("find_node inode num %d\n", inode_number);
  fs_debug_print("find_node block num %d\n", block_num);
  return fs_read_block(fs->disk, block, block_num);
}

int find_first(int32_t *nums, size_t length, int32_t to_find) {
  bool found = false;
  size_t i = 0;

  for (i = 0; i < length && !found; i++) {
    if (nums[i] == to_find) {
      found = true;
    }
  }
  if (found) {
    return i - 1;
  }
  return -1;
}

int direct_pointers_len(Inode *inode_ptr) {
  int index = find_first(inode_ptr->direct, POINTERS_PER_INODE, 0);
  if (index == -1) {
    return POINTERS_PER_INODE;
  }
  return index;
}

ssize_t fs_write_data_block(FileSystem *fs, size_t block_num, char *data,
                            size_t length, size_t offset) {
  Block *data_block = (Block *)malloc(sizeof(Block));
  if (data_block == NULL) {
    return -1;
  }

  fs_read_block(fs->disk, data_block, block_num);
  memcpy(data_block->data + offset, data, length);
  disk_write(fs->disk, block_num, data_block->data);
}

int min(int a, int b) {
  if (a <= b) {
    return a;
  }
  return b;
}

bool fs_format(Disk *disk) {
  Inode *inode_ptr;
  Block *block;
  int i = 0;
  int j = 0;
  int k = 0;

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

      if (i == FIRST_INODE_BLOCK && j == 0) {
        // inode_ptr->valid = true;
        // inode_ptr->size = 10;
        // inode_ptr->name[0] = 'r';
        // inode_ptr->name[1] = 'o';
        // inode_ptr->name[2] = 'o';
        // inode_ptr->name[3] = 't';
        // inode_ptr->name[4] = '\0';
      } else {
        inode_ptr->valid = false;
        inode_ptr->size = 0;
        inode_ptr->name[0] = '\0';
      }

      for (k = 0; k < POINTERS_PER_INODE; k++) {
        inode_ptr->direct[k] = 0;
      }
    }
    disk_write(disk, i, block->data);
  }

  free(block);

  return true;
}

bool fs_mount(FileSystem *fs, Disk *disk) {
  Block *block;

  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return false;
  }
  fs->inodes_name_map = (struct tablec_ht *)malloc(sizeof(struct tablec_ht));
  if (fs->inodes_name_map == NULL) {
    return false;
  }
  fs_read_block(disk, block, 0);

  fs->disk = disk;
  fs->meta_data = block->super;
  fs->free_blocks = fs_get_blocks_bitmap(fs);
  if (fs->free_blocks == NULL) {
    return false;
  }
  tablec_init(fs->inodes_name_map, 128);

  return true;
}

ssize_t fs_create(FileSystem *fs) {
  // find free inode in the fs
  int i = 0;
  int j = 0;
  int k = 0;
  int emptyBlockNum = -1;
  int inode_number;
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

      // emptyBlockNum is the absolute value from the beginning
      // of the disk. The value is stored as is in the inode
      // direct blocks.
      emptyBlockNum = fs_find_empty_block(fs);
      if (emptyBlockNum <= 0) {
        return -1;
      }
      inode_ptr->direct[0] = emptyBlockNum;

      // initialize to zero all the others
      for (k = 1; k < POINTERS_PER_INODE; k++) {
        inode_ptr->direct[k] = 0;
      }
      disk_write(fs->disk, i, block->data);
      fs->free_blocks[emptyBlockNum] = false;
      free(block);

      inode_number = INODES_PER_BLOCK * (i - FIRST_INODE_BLOCK) + j;
      tablec_set(fs->inodes_name_map, "root", (void *)inode_number);

      return inode_number;
    }
  }

  free(block);
  return -1;
}

ssize_t fs_write(FileSystem *fs, size_t inode_number, char *data, size_t length,
                 size_t offset) {
  // find_result *result;
  Block *inode_block;
  Block *data_block;
  Inode *inode_ptr;
  size_t i = 0;
  bool found = false;
  size_t offset_allocations_in_blocks;
  size_t length_allocations_in_blocks;
  size_t total_allocations_in_block;
  size_t blocks_available;
  size_t remaining_from_offset;
  size_t offset_relative_to_block_start;
  int emptyBlockNum = -1;
  size_t data_written = 0;
  size_t remaining = 0;
  uint32_t current_block;
  bool find_2_res;
  size_t block_num = FIRST_INODE_BLOCK + (inode_number / INODES_PER_BLOCK);
  size_t offset_inode = inode_number % INODES_PER_BLOCK;
  bool inode_needs_update = false;

  // result = fs_find_inode(fs, inode_number);
  // if (result == NULL) {
  //   return -1;
  // }
  inode_block = (Block *)malloc(sizeof(Block));
  if (inode_block == NULL) {
    return -1;
  }

  fs_debug_print("fs_write inode num %d %u %x\n", inode_number, inode_number,
                 inode_number);
  find_2_res = fs_find_inode_2(fs, inode_number, inode_block);
  inode_ptr = &(inode_block->inodes[offset_inode]);

  fs_debug_print("block num %d\n", block_num);
  fs_debug_print("offset for the inode%d\n", offset_inode);

  blocks_available = direct_pointers_len(inode_ptr);
  if (blocks_available == 0) {
    // error out for now, since we assume that a fs_create allocates at least
    // one block
    return -2;
  }
  fs_debug_print("direct pointers length %d\n", blocks_available);

  offset_allocations_in_blocks = (offset / BLOCK_SIZE) + 1 - blocks_available;
  remaining_from_offset = BLOCK_SIZE * ((offset / BLOCK_SIZE) + 1) - offset;
  if (length <= remaining_from_offset) {
    length_allocations_in_blocks = 0;
  } else {
    length_allocations_in_blocks =
        ((length - remaining_from_offset) / BLOCK_SIZE) + 1;
  }
  // how many more blocks need to be allocated to fulfill the request
  total_allocations_in_block =
      offset_allocations_in_blocks + length_allocations_in_blocks;
  if (total_allocations_in_block + blocks_available > POINTERS_PER_INODE) {
    // number of blocks after allocation would go over the max
    // POINTERS_PER_INODE
    return -3;
  }

  fs_debug_print("offset allocations in block %d\n",
                 offset_allocations_in_blocks);
  fs_debug_print("length allocations in block %d\n",
                 length_allocations_in_blocks);
  fs_debug_print("total allocations in block %d\n", total_allocations_in_block);
  // allocate blocks for the inode and write the result on disk
  for (i = 0; i < total_allocations_in_block; i++) {
    emptyBlockNum = fs_find_empty_block(fs);
    if (emptyBlockNum < 0) {
      return -4;
    }
    inode_ptr->direct[i + blocks_available] = emptyBlockNum;
    fs->free_blocks[emptyBlockNum] = false;
  }

  // finally write the content to the blocks
  data_written = 0;
  remaining = min(length, remaining_from_offset);
  i = 0;

  while (data_written < length) {
    current_block = inode_ptr->direct[i];
    offset_relative_to_block_start = (offset + data_written) % BLOCK_SIZE;
    fs_debug_print("writing to block %lu\n", current_block);
    fs_debug_print("offset_relative_to_block_start %d\n",
                   offset_relative_to_block_start);
    fs_debug_print("remaining %d\n", remaining);
    fs_write_data_block(fs, current_block, data + data_written, remaining,
                        offset_relative_to_block_start);
    data_written += remaining;
    remaining = min(length - data_written, BLOCK_SIZE);
    i++;
  }

  // here we probably want a truncate option
  // truncate
  inode_ptr->size = offset + length;
  inode_needs_update = true;
  // do not truncate, update size only if increased
  // if (inode_ptr->size < offset + length) {
  //   inode_ptr->size = offset + length;
  //   inode_needs_update = true;
  // }
  if (total_allocations_in_block > 0) {
    inode_needs_update = true;
  }
  // save the inode if needed
  if (inode_needs_update) {
    disk_write(fs->disk, block_num, inode_block->data);
  }

  return 0;
}

ssize_t fs_read(FileSystem *fs, size_t inode_number, char *data, size_t length,
                size_t offset) {
  bool find_res;
  bool read_block_res;
  Block *inode_block;
  Block *data_block;
  Inode *inode_ptr;
  size_t direct_length;
  size_t i, data_to_read, remaining = 0;
  size_t offset_inode = inode_number % INODES_PER_BLOCK;

  fs_debug_print(
      "[fs-read] read request for inode %u - length %u - offset %u\n",
      inode_number, length, offset);

  inode_block = (Block *)malloc(sizeof(Block));
  if (inode_block == NULL) {
    return -1;
  }
  find_res = fs_find_inode_2(fs, inode_number, inode_block);
  if (!find_res) {
    return -1;
  }
  inode_ptr = &(inode_block->inodes[offset_inode]);
  if (!inode_ptr->valid || inode_ptr->size == 0) {
    return 0;
  }
  if (inode_ptr->size < length) {
    return -1;
  }
  data_block = (Block *)malloc(sizeof(Block));
  if (data_block == NULL) {
    return -1;
  }

  // for now, ignore offset
  direct_length = direct_pointers_len(inode_ptr);
  data_to_read = 0;
  remaining = length;
  for (i = 0; i < direct_length && remaining > 0; i++) {
    fs_debug_print("[fs-read] reading data block %lu\n", inode_ptr->direct[i]);
    read_block_res = fs_read_block(fs->disk, data_block, inode_ptr->direct[i]);
    if (!read_block_res) {
      return -1;
    }
    while (remaining > 0) {
      data_to_read = min(BLOCK_SIZE, remaining);
      memcpy(data, data_block->data, data_to_read);
      remaining -= data_to_read;
    }
  }

  free(data_block);
  free(inode_block);

  return 0;
}

ssize_t fs_stat(FileSystem *fs, size_t inode_number) {
  Block *inode_block;
  Inode *inode_ptr;
  bool inode_found;
  size_t offset_inode = inode_number % INODES_PER_BLOCK;

  inode_block = (Block *)malloc(sizeof(Block));
  inode_found = fs_find_inode_2(fs, inode_number, inode_block);
  if (!inode_found) {
    return -1;
  }
  inode_ptr = &(inode_block->inodes[offset_inode]);
  if (!inode_ptr->valid) {
    return -1;
  }
  return inode_ptr->size;
}

void fs_info(FileSystem *fs, fs_info_res *res) {
  uint32_t free_blocks_count = 0;
  uint32_t blocks;
  uint32_t inodes_reserved = 0;
  int i = 0;

  blocks = fs->meta_data.blocks;
  inodes_reserved = fs->meta_data.inode_blocks;
  for (i = 0; i < blocks; i++) {
    if (fs->free_blocks[i]) {
      free_blocks_count++;
    }
  }
  res->total_blocks = blocks;
  res->free_blocks = free_blocks_count;
  res->reserved_for_inodes = inodes_reserved;
}

bool fs_inodes_walk(FileSystem *fs, void (*visitor)(Inode *)) {
  size_t i, j = 0;
  Block *block;
  Inode *inode_ptr;

  block = (Block *)malloc(sizeof(Block));
  if (block == NULL) {
    return false;
  }

  for (i = FIRST_INODE_BLOCK; i <= LAST_INODE_BLOCK; i++) {
    fs_read_block(fs->disk, block, i);

    for (j = 0; j < INODES_PER_BLOCK; j++) {
      inode_ptr = &(block->inodes[j]);
      if (inode_ptr->valid) {
        (*visitor)(inode_ptr);
      }
    }
  }
  free(block);
}

int fs_find(FileSystem *fs) {
  struct tablec_bucket *bucket = tablec_get(fs->inodes_name_map, "root");
  if (bucket == NULL) {
    return -1;
  }
  return (int)bucket->value;
}