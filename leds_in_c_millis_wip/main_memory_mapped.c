#include <stdint.h>

// A version of the blinking program that controls the VIA by accessing the memory mapped location
// directly in C, e.g.: 
//    a = (uint8_t*) 0x8002;
//    a[0] = 0xFF;
// writes 0xFF in the memory location 0x8002
void ledOn2() {
    uint8_t *a, *b;
 
    a = (uint8_t*) 0x8002;
    a[0] = 0xFF;

    b = (uint8_t*) 0x8000;
    b[0] = 0xFF;
}

void ledOff2() {
    uint8_t *a, *b;
 
    a = (uint8_t*) 0x8002;
    a[0] = 0xFF;

    b = (uint8_t*) 0x8000;
    b[0] = 0;
}

void spin() {
    int i = 0;
    int j = 0;
    
    while(i<255) {
         i++;
         while(j<255) {
            j++;
         }
    }

    i = 0;
    j = 0;

    while(i<255) {
         i++;
         while(j<255) {
            j++;
         }
    }
}

void main(void) {
    while(1==1) {
        ledOn2();
        spin();
        ledOff2();
        spin();
    }
}
