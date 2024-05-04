/* fs.h: SimpleFS file system */

#ifndef FS_H
#define FS_H

#include "disk.h"
#ifndef DISK_LOCAL
#include "types.h"
#else
#include <stddef.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* File System Constants */

#define MAGIC_NUMBER        (0xf0f03410)
#define INODES_PER_BLOCK    (2) //(128)               /* Number of inodes per block */
#define POINTERS_PER_INODE  (5)// (45)                 /* Number of direct pointers per inode */
#define POINTERS_PER_BLOCK  (2)  //(1024)              /* Number of pointers per block */
#define INODE_NAME_MAX      (64)
#define BLOCK_NUM           (64)
#define INODE_BLOCKS        (5)
#define INODE_TYPE_FILE     (0)
#define INODE_TYPE_DIR      (1)

/* File System Structures */

typedef struct SuperBlock SuperBlock;
struct SuperBlock {
    uint32_t    magic_number;                   /* File system magic number */
    uint32_t    blocks;                         /* Number of blocks in file system */
    uint32_t    inode_blocks;                   /* Number of blocks reserved for inodes */
    uint32_t    inodes;                         /* Number of inodes in file system */
};

typedef struct Inode      Inode;
struct Inode {
    char        name[INODE_NAME_MAX];
    uint32_t    valid;                          /* Whether or not inode is valid */
    uint32_t    size;                           /* Size of file */
    uint32_t    direct[POINTERS_PER_INODE];     /* Direct pointers */
    // uint32_t    indirect;                    /* Indirect pointers */
    uint32_t    type;                           /* Type of the inode (file 0, dir 1)*/
};

typedef union  Block      Block;
union Block {
    SuperBlock  super;                          /* View block as superblock */
    Inode       inodes[INODES_PER_BLOCK];       /* View block as inode */
    uint32_t    pointers[POINTERS_PER_BLOCK];   /* View block as pointers */
    char        data[BLOCK_SIZE];               /* View block as data */
};

typedef struct FileSystem FileSystem;
struct FileSystem {
    Disk        *disk;                          /* Disk file system is mounted on */
    bool        *free_blocks;                   /* Free block bitmap */
    SuperBlock   meta_data;                     /* File system meta data */
};

typedef struct fs_info_res {
  int free_blocks;
  int total_blocks;
  int reserved_for_inodes;
} fs_info_res;

/* File System Functions */

void    fs_debug(Disk *disk);
bool    fs_format(Disk *disk);

bool    fs_mount(FileSystem *fs, Disk *disk);
void    fs_unmount(FileSystem *fs);

ssize_t fs_create(FileSystem *fs);
bool    fs_remove(FileSystem *fs, size_t inode_number);
// fs_stat returns the logical size of the given inode_number, in bytes. Note that zero is a valid logical size for an inode. On failure, it returns -1.
ssize_t fs_stat(FileSystem *fs, size_t inode_number);

ssize_t fs_read(FileSystem *fs, size_t inode_number, char *data, size_t length, size_t offset);
ssize_t fs_write(FileSystem *fs, size_t inode_number, char *data, size_t length, size_t offset);

void fs_info(FileSystem *fs, fs_info_res* res);
bool fs_info_inodes(FileSystem *fs, void (*visitor)(Inode*));
#endif