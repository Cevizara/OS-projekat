//
// Korak 5g test: dve korisnicke niti koje se smenjuju (sinhrona promena konteksta).
// Svaka ispisuje svoje slovo preko putc (syscall) i ustupa procesor (thread_dispatch).
//

#include "../lib/hw.h"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/_thread.hpp"

static void haltQemu() {
    *(volatile uint32*)0x100000 = 0x5555;
}

static volatile bool doneA = false;
static volatile bool doneB = false;

// Tela niti se izvrsavaju u KORISNICKOM rezimu -> ispis ide preko putc (syscall),
// NE preko kernelprintString (koji je privilegovan).
static void workerA(void*) {
    for (int i = 0; i < 5; i++) {
        putc('A');
        thread_dispatch();     // dobrovoljno ustupi procesor
    }
    doneA = true;
}

static void workerB(void*) {
    for (int i = 0; i < 5; i++) {
        putc('B');
        thread_dispatch();
    }
    doneB = true;
}

int main() {
    Riscv::init();

    // Glavna nit predstavlja boot kontekst (body=nullptr -> nema pocetni kontekst).
    _thread mainThread(nullptr, nullptr, nullptr);
    _thread::running = &mainThread;

    thread_t tA, tB;
    thread_create(&tA, workerA, nullptr);
    thread_create(&tB, workerB, nullptr);

    kernelprintString("Start:\n");

    // main (sistemski rezim) ustupa procesor dok se obe niti ne zavrse
    while (!(doneA && doneB)) {
        thread_dispatch();
    }

    kernelprintString("\nGotovo!\n");
    haltQemu();
    return 0;
}
