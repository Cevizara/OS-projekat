//
// Interni (kernel) ispis preko __putc iz console.lib.
// Prefiks "kernel" da se NE sudari sa printString iz testova (printing.cpp).
//

#ifndef PRINT_HPP
#define PRINT_HPP

#include "../lib/hw.h"

extern void kernelprintString(char const *string);
extern void kernelprintInteger(uint64 integer);

#endif // PRINT_HPP
