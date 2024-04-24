#ifndef UM245_H
#define UM245_H

extern void __fastcall__  serial_writebyte(char);
extern void __fastcall__  serial_writeline(char*);

extern uint8_t __fastcall__  serial_readbyte();
extern void    __fastcall__  serial_readline(char*);

#endif