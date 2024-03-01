// A version of the blinking program that calls an assembly function passing a parameter with the fast call convention.  
// The function turn on the leds passed as parameter
// defined in assembly 

#include <stdint.h>

// led(255) turns on all 8 leds
// led(128) turns on the eight led and turns off all the others
extern void __fastcall__  led(uint8_t);

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

int pow(uint8_t x, uint8_t n)
{
    uint8_t i; /* Variable used in loop counter */
    uint8_t number = 1;

    for (i = 0; i < n; ++i)
        number *= x;

    return(number);
}

void main(void) {
    uint8_t i = 0;
    while(1==1) {
        for (i = 0; i <= 7; i++)
        {
            led(pow(2, i));
            spin();
            spin();
            spin();
            spin();
        }
    }
}

