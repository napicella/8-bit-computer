#include <stdint.h>

extern void __fastcall__  lcd_init();
extern void __fastcall__  lcd_print(char*);
extern void __fastcall__  lcd_clear();

void main(void) {
    char str[] = "Hi mom";
    
    lcd_init();
    lcd_print(str);
}
