#include <stdint.h>

extern void __fastcall__  lcd_init();
extern void __fastcall__  lcd_print(char*);

void main(void) {
    char str[] = "Hi mom";

    lcd_init();
    lcd_print(str);
}
