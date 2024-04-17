#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



extern void __fastcall__  lcd_init();
extern void __fastcall__  lcd_print(char*);
extern void __fastcall__  lcd_clear();
extern void __fastcall__  counter_init();
extern uint32_t __fastcall__  millis();
extern void __fastcall__  serial_write(char*);


uint32_t lastcalled = 0;
uint32_t current = 0;

uint8_t delay() {
    current = millis();
    if (current - lastcalled > 100) {
    lastcalled = current;
        return 1;
    }
    return 0;
}

void main(void) {
    uint32_t time = 0;
    char buf[10];

    counter_init();    
    lcd_init();

    while(1==1) {
        if (delay()) {
            time++;
            lcd_clear();
            itoa(time, buf, 10);
            lcd_print(buf);
            serial_write(buf);
        }
    }
}
