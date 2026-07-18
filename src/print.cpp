//
// Implementacija internog ispisa. Koristi __putc direktno (sistemski rezim),
// bez sistemskog poziva - sluzi nam za debagovanje jezgra.
//

#include "../h/print.hpp"
#include "../lib/console.h"

void kernelprintString(char const *string) {
    while (*string != '\0') {
        __putc(*string);
        string++;
    }
}

void kernelprintInteger(uint64 integer) {
    char buf[21];                 // 64-bitni broj ima najvise 20 cifara (+1)
    int i = 0;

    // do-while: i za integer==0 ispise "0"
    do {
        buf[i++] = (char)('0' + (integer % 10));
        integer /= 10;            // pun uint64, bez secenja na 32 bita
    } while (integer != 0);

    // cifre su upisane unazad -> ispisujemo od kraja
    while (--i >= 0) {
        __putc(buf[i]);
    }
}
