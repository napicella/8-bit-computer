#ifndef IO_H
#define IO_H

extern void __fastcall__  ledOff();
extern void __fastcall__  ledOn();
// led(255) turns on all 8 leds
// led(128) turns on the eight led and turns off all the others
extern void __fastcall__  led(uint8_t);
extern uint16_t __fastcall__  millis();

#endif