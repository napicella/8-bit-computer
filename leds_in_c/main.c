extern void __fastcall__  ledOff();
extern void __fastcall__  ledOn();

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

// This is the main function
void main(void) {
    ledOn();
    spin();
    ledOff();
}

