// A version of the blinking program that controls the VIA by calling functions (ledOff and ledOff)
// defined in assembly 
#include <stdint.h>


extern void __fastcall__  ledOff();
extern void __fastcall__  ledOn();
extern uint16_t __fastcall__  millis();

uint16_t lastcalled = 0;
uint16_t current = 0;

uint8_t delay() {
    current = millis();
    if (current - lastcalled > 25) {
    lastcalled = current;
        return 0;
    }
    return 1;
}

void main(void) {
    uint8_t on = 0; 
    while(1==1) {
        if (delay() == 0) {
            if (on == 0) {
                ledOn();
                on = 1;
            } else {
                ledOff();
                on = 0;
            }
        }
    }
}



