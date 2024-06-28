#include "devices.h"

VrEmu6522* Create6522() {
    return vrEmu6522New(VIA_65C22);
}

uint16_t Via6522_Start() {
    return 0x8000;
}
uint32_t Via6522_Length() {
    return 0x0F;
}