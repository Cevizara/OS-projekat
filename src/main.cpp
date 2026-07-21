//
// main jezgra: inicijalizuj, napravi prvu nit nad userMain (korisnicki kod iz
// test/userMain.cpp), pa vrti dispatch dok userMain ne zavrsi.
//

#include "../lib/hw.h"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/_thread.hpp"

// definisana u korisnickom kodu (test/userMain.cpp)
extern void userMain();

static void haltQemu() {
    *(volatile uint32*)0x100000 = 0x5555;
}

static volatile bool userMainDone = false;

static void userMainWrapper(void*) {
    userMain();
    userMainDone = true;
}

int main() {
    Riscv::init();

    // glavna (boot) nit predstavlja tekuci kontekst
    _thread mainThread(nullptr, nullptr, nullptr);
    _thread::running = &mainThread;

    // prva korisnicka nit nad userMain
    thread_t userThread;
    int r = thread_create(&userThread, userMainWrapper, nullptr);
    if (r < 0 || userThread == nullptr) {
        kernelprintString("GRESKA: ne mogu da napravim userMain nit\n");
        haltQemu();
        return -1;
    }

    while (!userMainDone) {
        thread_dispatch();
    }

    kernelprintString("\nKernel finished\n");
    haltQemu();
    return 0;
}
