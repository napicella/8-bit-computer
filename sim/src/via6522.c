#include "devices.h"

VrEmu6522* Create6522() {
    return vrEmu6522New(VIA_65C22);
}