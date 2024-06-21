// both the vrEmu6502 and vrEmu6522 libraries define an enumeration with the same enumerator name IntRequested. 
// When both header files are included, this results in a name clash.
// To resolve this, we need to avoid having the same name in both enumerations. One approach is to use a technique called 
// namespacing within the headers to ensure that the enumerators are unique. 
// Since we cannot change the library code, we work it around using wrapper headers for each library, modifying the 
// enumerators to be unique by using #define or other techniques.
// The source file will then include the wrapper header and use the new enum definition, e.g. vrEmu6502_IntRequested
// instead of IntRequested

#ifndef VREMU6502_WRAPPER_H
#define VREMU6502_WRAPPER_H

#define IntRequested vrEmu6502_IntRequested
#define IntCleared   vrEmu6502_IntCleared
#define IntLow       vrEmu6502_IntLow
#define IntHigh      vrEmu6502_IntHigh
#include "vrEmu6502.h"
#undef IntRequested
#undef IntCleared
#undef IntLow
#undef IntHigh

#endif // VREMU6502_WRAPPER_H