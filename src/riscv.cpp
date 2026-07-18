//
// Implementacija Riscv::init - postavlja stvec na prekidnu rutinu.
//

#include "../h/riscv.hpp"

// Definisana u asembleru (trapHandler.S).
extern "C" void trapEntry();

void Riscv::init() {
    // stvec dobija adresu naše prekidne rutine.
    // trapEntry je poravnat na 4 bajta (.align 4) -> najniža 2 bita = 0
    // -> MODE = 0 = direktni režim (jedna rutina za sve uzroke).
    w_stvec((uint64)&trapEntry);
}
