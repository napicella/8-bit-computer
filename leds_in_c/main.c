// A version of the blinking program that controls the VIA by calling functions (ledOff and ledOff)
// defined in assembly 

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

void main(void) {
    while(1==1) {
        ledOn();
        spin();
        spin();
        spin();
        spin();
        ledOff();
        spin();
        spin();
        spin();
        spin();
    }
}

