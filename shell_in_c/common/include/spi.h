#ifndef SPI_H
#define SPI_H

extern void spiBegin(void);
extern void spiEnd(void);
extern void spiInit(void);
extern void __fastcall__ spiWrite(char byte);
extern char __fastcall__ spiRead(void);

#endif