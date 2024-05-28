#ifndef UM245_H
#define UM245_H

#include <stdint.h>

extern void __fastcall__ serial_init(void);

extern void __fastcall__  serial_writebyte(char);
/*
 * serial_writeline writes until the string terminator (\0, that is a 0, the NULL byte)
 */
extern void __fastcall__  serial_writeline(char*);

/*
 * serial_writeline_f is like serial_writeline but accepts a format string.
 * Example usage:
 * serial_writeline_f("find_node block num %d\n", block_num); 
 */
void serial_writeline_f(const char *fmt, ...);

extern uint8_t __fastcall__  serial_readbyte();
/*
 * serial_readline read until the string terminator (\0, that is a 0, the NULL byte)
 */
extern void    __fastcall__  serial_readline(char*);

#endif