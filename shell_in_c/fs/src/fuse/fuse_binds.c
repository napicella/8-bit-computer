
// Example from:
// https://engineering.facile.it/blog/eng/write-filesystem-fuse/
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "fs.h"

Disk *disk;
FileSystem *fs;
bool fs_mounted = false;


static int do_getattr(const char *path, struct stat *st) {
  printf("[getattr] Called\n");
  printf("\tAttributes of %s requested\n", path);

  // GNU's definitions of the attributes
  // (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
  // 		st_uid: 	The user ID of the file’s owner.
  //		st_gid: 	The group ID of the file.
  //		st_atime: 	This is the last access time for the file.
  //		st_mtime: 	This is the time of the last modification to the
  // contents of the file. 		st_mode: 	Specifies the mode of
  // the file. This includes file type information (see Testing File Type) and
  // the file permission bits (see Permission Bits). 		st_nlink:
  // The number of hard links to the file. This count keeps track of how many
  // directories have entries for this file. If the count is ever decremented to
  // zero, then the file itself is discarded as soon
  // as no process still holds it open. Symbolic links are not counted in the
  // total. 		st_size:	This specifies the size of a regular
  // file in bytes. For files that are really devices this field isn’t usually
  // meaningful. For symbolic links this specifies the length of the file name
  // the link refers to.

  st->st_uid = getuid();  // The owner of the file/directory is the user who
                          // mounted the filesystem
  st->st_gid = getgid();  // The group of the file/directory is the same as the
                          // group of the user who mounted the filesystem
  st->st_atime =
      time(NULL);  // The last "a"ccess of the file/directory is right now
  st->st_mtime =
      time(NULL);  // The last "m"odification of the file/directory is right now

  if (strcmp(path, "/") == 0) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;  // Why "two" hardlinks instead of "one"? The answer is
                       // here: http://unix.stackexchange.com/a/101536
  } else {
    int stat_res = fs_stat(fs, 0);
    if (stat_res < 0) {
      return -1;
    }

    st->st_mode = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size = stat_res;
  }
  return 0;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi) {
  printf("--> Getting The List of Files of %s\n", path);

  filler(buffer, ".", NULL, 0);   // Current Directory
  filler(buffer, "..", NULL, 0);  // Parent Directory

  if (strcmp(path, "/") ==
      0)  // If the user is trying to show the files/directories of the root
          // directory show the following
  {
    filler(buffer, "file", NULL, 0);
  }

  return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset,
                   struct fuse_file_info *fi) {
  printf("--> Trying to read %s, %ld, %ld\n", path, offset, size);

  // char file54Text[] = "Hello World From File54!";
  // char file349Text[] = "Hello World From File349!";
  // char *selectedText = NULL;

  // if (strcmp(path, "/file54") == 0)
  //   selectedText = file54Text;
  // else if (strcmp(path, "/file349") == 0)
  //   selectedText = file349Text;
  // else
  //   return -1;
  // memcpy(buffer, selectedText + offset, size);

  int stat_res = fs_stat(fs, 0);
  if (stat_res < 0) {
    return -1;
  }
  
  int res = fs_read(fs, 0, buffer, min(stat_res, size), 0);
  if (res != 0) {
    printf("fs_read error %d\n", res);
    return res;
  }

  return min(stat_res, size);
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,
    .read = do_read,
};

int main(int argc, char *argv[]) {
  if (!fs_mounted) {
    disk = (Disk *)malloc(sizeof(Disk));
    if (disk == NULL) {
      return -1;
    }
    fs = (FileSystem *)malloc(sizeof(Disk));
    if (fs == NULL) {
      return -1;
    }
    fs_mount(fs, disk);
    fs_mounted = true;
  }
  return fuse_main(argc, argv, &operations, NULL);
}