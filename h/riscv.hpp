//
// Riscv: uslužna klasa - omotači oko instrukcija za pristup sistemskim
// (csr) registrima procesora + inicijalizacija jezgra.
// Za sada samo ono što nam treba za prekidni skelet; širiće se kasnije.
//

#ifndef RISCV_HPP
#define RISCV_HPP

#include "../lib/hw.h"

class Riscv
{
public:
    // u init-u postavljamo stvec na nasu prekidnu rutinu
    static void init();

    // statusni registar
    static uint64 r_sstatus();
    static void   w_sstatus(uint64 sstatus);

    // razlog ulaska u prekidnu rutinu
    static uint64 r_scause();

    // saved pc - adresa na koju sret vraca
    static uint64 r_sepc();
    static void   w_sepc(uint64 sepc);

    // adresa prekidne rutine
    static void   w_stvec(uint64 stvec);
};

// --- definicije (posle tela klase, sa kvalifikatorom Riscv:: i inline) ---

inline uint64 Riscv::r_scause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline uint64 Riscv::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

#endif // RISCV_HPP
