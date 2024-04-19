#ifndef LCD_H
#define LCD_H

extern void __fastcall__  lcd_init();
extern void __fastcall__  lcd_print(char*);
extern void __fastcall__  lcd_clear();

#endif

// #if defined(LCD_STUB)
// void lcd_init() {}
// lcd_print(char*) {

// }
// void lcd_clear() {}
// #else
// extern void __fastcall__ lcd_init();
// extern void __fastcall__ lcd_print(char*);
// extern void __fastcall__ lcd_clear();
// #endif
