#include <_heap.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "disk.h"
#include "fs.h"
#include "io.h"
#include "lcd.h"
#include "spi.h"
#include "um245.h"

extern uint16_t __fastcall__ wozmon_run();
bool fs_inode_print(Inode* inode_ptr);

Disk* disk;
FileSystem* fs;
bool fs_mounted = false;

void info();
void shell_fs_print_info();
void shell_fs_create();
void shell_fs_write();
void shell_fs_mount();
void shell_fs_read();
void shell_fs_stat(char* buff);
void shell_fs_find();
void shell_spi();

int my_strncmp(const char* s1, const char* s2, size_t count) {
  int currentIndex = 0;
  while (currentIndex < count) {
    if (s1[currentIndex] == '\0' && s2[currentIndex] == '\0') {
      return 0;
    }
    if (s1[currentIndex] == '\0' || s2[currentIndex] == '\0') {
      if (s1[currentIndex] == '\0') {
        return -1;
      }
      if (s2[currentIndex] == '\0') {
        return 1;
      }
    }

    if (s1[currentIndex] == s2[currentIndex]) {
      currentIndex++;
      continue;
    }

    return s1[currentIndex] - s2[currentIndex];
  }
}

void main(void) {
  char motd[] = "*\n**\n**** pactvm6502 v0.1\n**\n*\n";
  const size_t buff_size = 64;
  char currChar;
  int currentIndex = 0;
  char* buff = (char*)malloc(sizeof(char) * buff_size);
  int i = 0;
  bool fmtResult;

  disk = (Disk*)malloc(sizeof(Disk));
  if (disk == NULL) {
    *((uint8_t*)0xA000) = 'N';
    return;
  }

  fs = (FileSystem*)malloc(sizeof(Disk));
  if (fs == NULL) {
    *((uint8_t*)0xA000) = 'N';
    return;
  }

  spiInit();
  serial_init();
  serial_writeline(motd);
  serial_writeline("\n> ");

  while (true) {
    currChar = serial_readbyte();
    buff[currentIndex] = currChar;
    serial_writebyte(currChar);  // echo
    currentIndex++;
    if (currChar == '\r') {
      // terminal sends LF on Enter key
      // write newline
      serial_writebyte('\n');

      if (my_strncmp(buff, "info", 4) == 0) {
        info();
      } else if (my_strncmp(buff, "spi", 3) == 0) {
        shell_spi();
      } else if (my_strncmp(buff, "fsfmt", 5) == 0) {
        fmtResult = fs_format(disk);
        if (!fmtResult) {
          serial_writeline("error fmt");
        }
      } else if (my_strncmp(buff, "fsmnt", 5) == 0) {
        serial_writeline("ACK mnt\n");
        fs_mount(fs, disk);
        fs_mounted = true;
      } else if (my_strncmp(buff, "fscreate", 8) == 0) {
        serial_writeline("ACK create\n");
        shell_fs_create();
      } else if (my_strncmp(buff, "fsread", 6) == 0) {
        shell_fs_read(buff);
      } else if (my_strncmp(buff, "fswrite", 7) == 0) {
        shell_fs_write(buff);
      } else if (my_strncmp(buff, "fsstat", 6) == 0) {
        shell_fs_stat(buff);
      } else if (my_strncmp(buff, "fsfind", 6) == 0) {
        shell_fs_find();
      } else if (my_strncmp(buff, "ledon", 5) == 0) {
        ledOn();
      } else if (my_strncmp(buff, "ledoff", 6) == 0) {
        ledOff();
      } else if (my_strncmp(buff, "wozmon", 6) == 0) {
        wozmon_run();
      } else {
        serial_writeline("cmd not found:\n");
      }
      currentIndex = 0;
      memset(buff, 0, buff_size);
      serial_writeline("\n> ");
    }
  }
}

void info() {
  size_t memAvail = __heapmemavail();
  char* data = (char*)malloc(sizeof(char) * 25);
  sprintf(data, "heap_available: %d\n", memAvail);

  serial_writeline(data);
  free(data);
  shell_fs_print_info();
}

void shell_fs_print_info() {
  fs_info_res* res;
  char* data;

  data = (char*)malloc(sizeof(char) * 512);
  if (data == NULL) {
    return;
  }
  res = (fs_info_res*)malloc(sizeof(fs_info_res));
  if (res == NULL) {
    return;
  }

  shell_fs_mount();
  fs_info(fs, res);
  sprintf(data, "fs_tot_blocks %lu\nfree_blocks: %lu\ninode_reserved: %lu\n\r",
          res->total_blocks, res->free_blocks, res->reserved_for_inodes);
  serial_writeline(data);
  // fs_inodes_walk(fs, fs_inode_print);
  free(data);
  free(res);
}

void shell_fs_mount() {
  if (!fs_mounted) {
    serial_writeline("mounting fs\n");
    fs_mount(fs, disk);
    fs_mounted = true;
  } else {
    serial_writeline("fs already mounted\n");
  }
}

void shell_fs_create() {
  int inode;
  char* data;

  serial_writeline("create request\n");
  shell_fs_mount();

  data = (char*)malloc(sizeof(char) * 128);
  inode = fs_create(fs);
  if (inode == -1) {
    serial_writeline("failed to get an inode\n");
    return;
  }
  sprintf(data, "inode number: %d\n", inode);
  serial_writeline(data);
}

void shell_fs_write(char* buff) {
  ssize_t write_res;
  int scan_res;

  char* data;
  short unsigned int inode_read = 0;
  int i = 0;

  // Space for 63 characters plus string terminator
  data = (char*)malloc(64 * sizeof(char));
  memset(data, 0, 64);

  // Note: scanf "%[Number]c" type specifier does not seem to work
  scan_res = sscanf(buff, "fswrite %hu", &inode_read);
  if (scan_res < 1 || scan_res == EOF) {
    serial_writeline_f("invalid fswrite format: %d", scan_res);
    return;
  }

  serial_writebyte('\n');
  i = 0;
  while (i < 63) {
    data[i] = serial_readbyte();
    serial_writebyte(data[i]);  // echo
    if (i > 2) {
      if (data[i - 2] == 'E' && data[i - 1] == 'O' && data[i] == 'F') {
        break;
      }
    }
    i++;
  }
  if (strlen(data) - 3 <= 0) {
    return;
  }

  serial_writeline_f("input inode: %hu\n", inode_read);
  shell_fs_mount();

  write_res = fs_write(fs, inode_read, data, strlen(data) - 3, 0);
  if (write_res != 0) {
    serial_writeline_f("error: %d\n", write_res);
  }
}

void shell_fs_read(char* buff) {
  char* data;
  ssize_t read_res;
  ssize_t stat_res;
  size_t inode_read;
  int scan_res;
  int i = 0;

  scan_res = sscanf(buff, "fsread %d", &inode_read);
  if (scan_res != 1 || scan_res == EOF) {
    serial_writeline_f("invalid fsread format");
    return;
  }

  shell_fs_mount();

  stat_res = fs_stat(fs, inode_read);
  if (stat_res < 0) {
    serial_writeline("error reading file");
    return;
  }
  if (stat_res == 0) {
    return;
  }

  data = (char*)malloc(stat_res * sizeof(char));
  if (data == NULL) {
    return;
  }
  read_res = fs_read(fs, inode_read, data, stat_res, 0);
  if (read_res != 0) {
    serial_writeline_f("failed to read %d\n", read_res);
  }
  for (i = 0; i < stat_res; i++) {
    serial_writebyte(data[i]);
  }

  free(data);
}

void shell_fs_stat(char* buff) {
  int inode_read;
  int stat_res = -1;
  int scan_res = 0;

  scan_res = sscanf(buff, "fsstat %d", &inode_read);
  if (scan_res != 1 || scan_res == EOF) {
    serial_writeline_f("invalid fsstat format");
    return;
  }
  shell_fs_mount();

  stat_res = fs_stat(fs, inode_read);
  if (stat_res < 0) {
    serial_writeline("error reading file");
    return;
  }
  serial_writeline_f("size: %d", stat_res);
}

bool fs_inode_print(Inode* inode_ptr) {
  int index = 0;
  int i = 0;

  char* data = (char*)malloc(256 * sizeof(char));
  index = sprintf(data,
                  "inode\n  name: %s\n  size: %lu\n  blocks:", inode_ptr->name,
                  inode_ptr->size);

  for (i = 0; i < POINTERS_PER_INODE; i++) {
    index += sprintf(&data[index], "%lu ", inode_ptr->direct[i]);
  }
  sprintf(&data[index], "\n");
  serial_writeline(data);

  free(data);
}

void shell_fs_find() {
  struct tablec_bucket* bucket;
  int inode;
  shell_fs_mount();
  // tablec_set(fs->inodes_name_map, "root", (void *)42);

  // bucket = tablec_get(fs->inodes_name_map, "root");
  // if (bucket == NULL) {
  //   return;
  // }
  inode = fs_find(fs, "root");
  serial_writeline_f("[TableC]: Value of the key \"root\": %d\n", (int)inode);
}

void shell_spi() {
  char c;
  spiBegin();
  c = serial_readbyte();
  while(c != '|') {
    spiWrite(c);
    c = serial_readbyte();
  }
  spiEnd();
}