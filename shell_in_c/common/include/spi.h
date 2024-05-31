#ifndef SPI_H
#define SPI_H

extern void __fastcall__ spiBegin(void);
extern void __fastcall__ spiEnd(void);
extern void __fastcall__ spiInit(void);
extern void __fastcall__ spiWrite(char byte);
extern char __fastcall__ spiRead(void);

#endif