//
// Korak 9 test: C++ API (Thread preko nasledjivanja + run(), Semaphore).
// Semafori se prave preko POKAZIVACA + new (kao u javnim testovima), a NE kao
// globalni objekti - da se ne generise __cxa_atexit (nema stdlib runtime-a).
//

#include "../lib/hw.h"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/_thread.hpp"

static void haltQemu() {
    *(volatile uint32*)0x100000 = 0x5555;
}

// globalni POKAZIVACI (nemaju destruktor -> nema __cxa_atexit)
static Semaphore* prazno;
static Semaphore* puno;
static volatile int bafer = -1;
static volatile bool gotovo = false;

// Nit preko NASLEDJIVANJA: izvedi klasu i preklopi run().
class Proizvodjac : public Thread {
protected:
    void run() override {
        for (int i = 1; i <= 5; i++) {
            prazno->wait();
            bafer = i;
            putc('P'); putc('0' + i);
            puno->signal();
            Thread::dispatch();
        }
    }
};

class Potrosac : public Thread {
protected:
    void run() override {
        for (int i = 1; i <= 5; i++) {
            puno->wait();
            int x = bafer;
            putc('C'); putc('0' + x);
            prazno->signal();
            Thread::dispatch();
        }
        gotovo = true;
    }
};

int main() {
    Riscv::init();

    _thread mainThread(nullptr, nullptr, nullptr);
    _thread::running = &mainThread;

    prazno = new Semaphore(1);   // 1 slobodno mesto; global new -> mem_alloc
    puno   = new Semaphore(0);   // nema podatka

    kernelprintString("Start:\n");

    Proizvodjac p;
    Potrosac    c;
    p.start();       // tek start() stvarno kreira nit
    c.start();

    while (!gotovo) {
        thread_dispatch();
    }

    kernelprintString("\nGotovo!\n");
    haltQemu();
    return 0;
}
