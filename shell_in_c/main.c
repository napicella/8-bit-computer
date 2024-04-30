#include <_heap.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "disk.h"
#include "fs.h"
#include "io.h"
#include "lcd.h"
#include "um245.h"

extern uint16_t __fastcall__ wozmon_run();

Disk* disk;
FileSystem* fs;
bool fs_mounted;

void serial_fprintline(const char* fmt, ...);

void info();
void shell_fs_print_info();
void shell_fs_create();
void shell_fs_write();
void shell_fs_mount();
void shell_fs_read();
void shell_fs_stat(char* buff);

void main(void) {
  char motd[] = "*\n**\n**** pactvm6502 v0.1\n**\n*\n";
  const size_t buff_size = 64;
  char currChar;
  int currentIndex = 0;
  char* buff = (char*)malloc(sizeof(char) * buff_size);

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
      currentIndex = 0;

      if (strncmp(buff, "info", 4) == 0) {
        info();
      } else if (strncmp(buff, "fsfmt", 5) == 0) {
        fs_format(disk);
      } else if (strncmp(buff, "fsmnt", 5) == 0) {
        fs_mount(fs, disk);
        fs_mounted = true;
      } else if (strncmp(buff, "fscreate", 8) == 0) {
        shell_fs_create();
      } else if (strncmp(buff, "fsread", 6) == 0) {
        shell_fs_read(buff);
      } else if (strncmp(buff, "fswrite", 7) == 0) {
        shell_fs_write(buff);
      } else if (strncmp(buff, "fsstat", 6) == 0) {
        shell_fs_stat(buff);
      } else if (strncmp(buff, "ledon", 5) == 0) {
        ledOn();
        serial_writeline("\n> ");
      } else if (strncmp(buff, "ledoff", 6) == 0) {
        ledOff();
      } else if (strncmp(buff, "wozmon", 6) == 0) {
        wozmon_run();
      } else {
        serial_writeline("command not found\n");
      }

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
  shell_fs_mount();
  fs_info(fs);
}

void shell_fs_mount() {
  if (!fs_mounted) {
    fs_mount(fs, disk);
    fs_mounted = true;
  }
}

void shell_fs_create() {
  int inode;
  char* data;

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
  ssize_t res;
  char* log_buff;
  char data[21];
  int inode_read = 0;
  int scan_res;
  int i;
  for (i = 0; i < 21; i++) {
    data[i] = '\0';
  }

  // scanf "c" type specifier
  // Single character: Reads the next character. If a width different from 1 is
  // specified, the function reads width characters and stores them in the
  // successive locations of the array passed as argument. No null character is
  // appended at the end.
  scan_res = sscanf(buff, "fswrite %d %20c", &inode_read, data);
  serial_fprintline("input inode: %d", inode_read);
  if (scan_res < 1 || scan_res == EOF) {
    serial_fprintline("invalid fswrite format: %d", scan_res);
    return;
  }

  shell_fs_mount();

  res = fs_write(fs, inode_read, data, strlen(data), 0);
  if (res != 0) {
    log_buff = (char*)malloc(sizeof(char) * 16);
    sprintf(log_buff, "error: %d\n", res);
    serial_writeline(log_buff);
    free(log_buff);
  }
}

void shell_fs_read(char* buff) {
  char* data;
  ssize_t read_res;
  ssize_t stat_res;
  int inode_read;
  int scan_res;

  scan_res = sscanf(buff, "fsread %d", &inode_read);
  if (scan_res != 1 || scan_res == EOF) {
    serial_fprintline("invalid fsread format");
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
  read_res = fs_read(fs, inode_read, data, stat_res, 0);
  if (read_res != 0) {
    serial_fprintline("failed to read %d\n", read_res);
  }
  serial_writeline(data);
  free(data);
}

void shell_fs_stat(char* buff) {
  int inode_read;
  int stat_res = -1;
  int scan_res = 0;

  scan_res = sscanf(buff, "fsstat %d", &inode_read);
  if (scan_res != 1 || scan_res == EOF) {
    serial_fprintline("invalid fsstat format");
    return;
  }
  shell_fs_mount();

  stat_res = fs_stat(fs, inode_read);
  if (stat_res < 0) {
    serial_writeline("error reading file");
    return;
  }
  serial_fprintline("size: %d", stat_res);
}