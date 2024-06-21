#ifndef VREMU6522_WRAPPER_H
#define VREMU6522_WRAPPER_H

#define IntRequested vrEmu6522_IntRequested
#define IntCleared   vrEmu6522_IntCleared
#define IntLow       vrEmu6522_IntLow
#define IntHigh      vrEmu6522_IntHigh
#include "vrEmu6522.h"
#undef IntRequested
#undef IntCleared
#undef IntLow
#undef IntHigh

#endif // VREMU6522_WRAPPER_H