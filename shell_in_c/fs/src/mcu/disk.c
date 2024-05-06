#include "disk.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "um245.h"

#define LOW(x) ((x) & 0xff)
#define HIGH(x) ((x) >> 8)

typedef struct Packet Packet;
struct Packet {
  char* magic;
  uint16_t contentLength;
  char* body;
};

void sendMagic(Packet* p) {
  char* ptr = p->magic;

  while (*ptr != '\0') {
    serial_writebyte(*ptr);
    ptr++;
  }
}

void sendCl(Packet* p) { 
  serial_writebyte(LOW(p->contentLength));
  serial_writebyte(HIGH(p->contentLength));
}

void sendBody(Packet* p) {
  uint16_t i = 0;

  for (i = 0; i < p->contentLength; i++) {
    serial_writebyte(p->body[i]);
  }
}

void sendPacket(Packet* p) {
  sendMagic(p);
  sendCl(p);
  sendBody(p);
}

ssize_t disk_read(Disk* disk, size_t block, char* data) {
  // RD_CMD_START = <MAGIC><SIZE>R<SECTOR>
  // <SECTOR> - 8 bits

  uint16_t i = 0;
  uint8_t body[2];
  Packet* packet;

  packet = (Packet*)malloc(sizeof(Packet));
  packet->magic = "@cmdmode";
  packet->contentLength = sizeof(char) + sizeof(uint8_t);
  body[0] = (uint8_t)'R';
  body[1] = block;
  packet->body = body;

  sendPacket(packet);
  free(packet);

  for (i = 0; i < BLOCK_SIZE; i++) {
    data[i] = serial_readbyte();
  }
  return 0;
}

ssize_t disk_write(Disk* disk, size_t block, char* data) {
  // WR_CMD_START = <MAGIC><SIZE>W<SECTOR><DATA>
  // <SECTOR> - 8 bits

  uint16_t i = 0;
  uint8_t* body;
  Packet* packet;

  body = (uint8_t*)malloc(sizeof(char) + sizeof(uint8_t) * (BLOCK_SIZE + 2));
  if (body == NULL) {
    return DISK_FAILURE;
  }
  packet = (Packet*)malloc(sizeof(Packet));
  if (packet == NULL) {
    return DISK_FAILURE;
  }
  packet->magic = "@cmdmode";
  packet->contentLength = sizeof(char) + sizeof(uint8_t) + BLOCK_SIZE;
  body[0] = (uint8_t)'W';
  body[1] = block;
  memcpy(&body[2], data, BLOCK_SIZE);
  packet->body = body;

  sendPacket(packet);
  free(packet);

  return 0;
}

void fs_debug_print(const char *fmt, ...) {
  char *data = (char *)malloc(128 * sizeof(char));
  va_list arg;
  va_start(arg, fmt);
  vsprintf(data, fmt, arg);
  serial_writeline(data);
  va_end(arg);
  free(data);
}