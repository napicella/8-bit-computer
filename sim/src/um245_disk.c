// #include <fcntl.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <poll.h>

// #include "devices.h"

// #define startOfHeading 0b00000001
// #define endOfHeading 0b00000011
// #define readOp 0b00000100
// #define writeOp 0b00001000

// void cmd_buffer_init(CmdBuffer* buf) {
//   buf->data = (uint8_t*)malloc(64 * sizeof(uint8_t));
//   buf->size = 0;
//   buf->capacity = 64;
// }

// void cmd_buffer_append(CmdBuffer* buf, uint8_t val) {
//   if (buf->size >= buf->capacity) {
//     buf->capacity *= 2;
//     buf->data = (uint8_t*)realloc(buf->data, buf->capacity * sizeof(uint8_t));
//   }
//   buf->data[buf->size++] = val;
// }

// void cmd_buffer_destroy(CmdBuffer* buf) { free(buf->data); }


// Um245* CreateUm245() {
//   Um245* um245 = malloc(sizeof(Um245));

//   int fd;
//   char* fifoOut = "/tmp/fifo_out";
//   //mkfifo(fifoOut, 0666);
//   fd = open(fifoOut, O_CREAT | O_WRONLY);
//   um245->outFd = fd;

//   char* fifoIn = "/tmp/fifo_in";
//   //mkfifo(fifoIn, 0666);
//   fd = open(fifoIn, O_CREAT | O_RDONLY | O_NONBLOCK);
//   um245->inFd = fd;

//   return um245;
// }

// uint16_t Um245_Start() { return UM_START_ADDR; }

// uint32_t Um245_Length() { return UM_SIZE; }

// uint8_t canReadFromPipe(int fd) {
//   struct pollfd ufds[1]; 
//   ufds[0].fd = fd;
//   ufds[0].events = POLLIN;
//   if (poll(ufds, 1, 0)==1) {
//     // data available
//     return 1;
//   }
//   return 0;

//   // // file descriptor struct to check if POLLIN bit will be set
//   // // fd is the file descriptor of the pipe
//   // struct pollfd fds {
//   //   .fd = fd, .events = POLLIN
//   // };
//   // // poll with no wait time
//   // int res = poll(&fds, 1, 0);

//   // // if res < 0 then an error occurred with poll
//   // // POLLERR is set for some other errors
//   // // POLLNVAL is set if the pipe is closed
//   // if (res < 0 || fds.revents & (POLLERR | POLLNVAL)) {
//   //   // an error occurred, check errno
//   // }
//   // return fds.revents & POLLIN;
// }

// uint8_t Um245_Read(uint16_t addr, Um245* um) {
//   if (addr == UART_READ) {
//     uint8_t data[1];
//     int n = read(um->inFd, data, 1);
//     if (n == -1) {
//       printf("Error reading from pipe in um245\n");
//       exit(1);
//     }
//     if (n == 0) {
//       printf("EOF reading from pipe um245\n");
//     }
//     return data[0];
//   }

//   if (addr == UART_STATUS) {
//     uint8_t canRead = canReadFromPipe(um->inFd);

//     if (!canRead) {
//       return UART_READ_NO_DATA_MASK;
//     }
//     return 0;
//   }

//   printf("Invalid read address for um245\n");
//   exit(1);
// }

// void Um245_Write(uint16_t addr, uint8_t val, Um245* um) {
//   if (addr != UART_WRITE) {
//     printf("Invalid write address for um245\n");
//     exit(1);
//   }

//   uint8_t data[1];
//   data[0] = val;
//   write(um->outFd, data, 1);
// }

// void Um245_Close(Um245* um) {
//   close(um->outFd);
//   close(um->inFd);
// }