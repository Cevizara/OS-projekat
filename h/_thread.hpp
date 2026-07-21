//
// _thread: interna klasa jezgra koja predstavlja nit (PCB).
// Ime MORA biti _thread (C API: typedef _thread* thread_t) zbog app.lib.
// KORAK 3 = kostur: podaci + interfejs. "Meso" metoda dolazi u koracima 4-6.
//

#ifndef _THREAD_HPP
#define _THREAD_HPP

#include "../lib/hw.h"

class _thread {
public:
    // tip tela niti: funkcija koja prima void* arg
    using Body = void (*)(void*);

    // zamrznuto stanje niti: gde je stala (ra) i na kom steku (sp)
    struct Context {
        uint64 ra;
        uint64 sp;
    };

    _thread(Body body, void* arg, void* stack);

    // --- stanja (trivijalno, KORAK 3) ---
    bool isFinished();
    void setFinished(bool f);
    bool isBlocked();
    void setBlocked(bool b);
    Context* getContext();

    // --- protokol blokiranja na semaforu (KORAK 8) ---
    // semafor upisuje niti rezultat cekanja (0=probudio signal, -1=zatvoren/greska),
    // a wait_n pamti koliko jedinica nit trazi
    long long getSemResult();
    void      setSemResult(long long r);
    long long getSemAmount();
    void      setSemAmount(long long n);

    // --- meso (dolazi kasnije) ---
    static int  createThread(_thread** handle, Body body, void* arg, void* stack); // KORAK 5
    static void dispatch();                                                        // KORAK 5
    static int  exit();                                                            // KORAK 6
    static void threadWrapper();                                                   // KORAK 5
    static void promenaKonteksta(Context* oldC, Context* newC);                     // KORAK 4 (asm, mangled)

    // alokacija _thread objekata preko mem.lib (ne globalni new) -- KORAK 5
    void* operator new(size_t size);
    void  operator delete(void* p);

    // pokazivac na tekucu (running) nit -- jedan jedini
    static _thread* running;

private:
    Body    body;
    void*   arg;
    void*   stack;
    Context context;
    bool    finished;
    bool    blocked;
    long long semResult;   // rezultat wait-a (postavlja semafor)
    long long semAmount;   // koliko jedinica nit trazi (za wait_n)
};

#endif // _THREAD_HPP
