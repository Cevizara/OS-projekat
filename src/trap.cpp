//
// C deo prekidne rutine: razgranati skok po uzroku (scause) i kodu poziva.
// Za ecall: pozovi odgovarajucu uslugu jezgra, upisi povratnu vrednost u a0, sepc+=4.
//

#include "../h/riscv.hpp"
#include "../h/print.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"      // __putc (za CALL_PUTC)
#include "../h/syscall_enum.hpp"
#include "../h/_thread.hpp"
#include "../h/_sem.hpp"

extern "C" void handleTrap(uint64* regs) {
    uint64 scause = Riscv::r_scause();

    // Sacuvaj sepc i sstatus na ULAZU (kao lokalne, na steku ove niti).
    // Ako se unutar obrade desi promena konteksta, druge niti ce promeniti
    // globalne sepc/sstatus; ove lokalne kopije nam cuvaju NASE vrednosti.
    uint64 sepc    = Riscv::r_sepc();
    uint64 sstatus = Riscv::r_sstatus();

    if (scause == 0x08UL || scause == 0x09UL) {   // ecall (korisnicki / sistemski)
        uint64 kod = regs[10];                     // a0 = kod sistemskog poziva
        uint64 ret = (uint64)-1;

        switch (kod) {
            case CALL_MEM_ALLOC: {
                size_t blocks = (size_t)regs[11];  // a1 = broj blokova (ABI nivo)
                ret = (uint64)MemoryAllocator::_mem_alloc(blocks);
                break;
            }
            case CALL_MEM_FREE: {
                ret = (uint64)MemoryAllocator::_mem_free((void*)regs[11]);   // a1 = pokazivac
                break;
            }
            case CALL_THREAD_CREATE: {
                _thread**     handle = (_thread**)regs[11];       // a1 = &handle
                _thread::Body body   = (_thread::Body)regs[12];   // a2 = telo
                void*         arg    = (void*)regs[13];           // a3 = argument
                void*         stack  = (void*)regs[14];           // a4 = vrh steka
                ret = (uint64)_thread::createThread(handle, body, arg, stack);
                break;
            }
            case CALL_THREAD_DISPATCH: {
                _thread::dispatch();
                ret = 0;
                break;
            }
            case CALL_THREAD_EXIT: {
                ret = (uint64)_thread::exit();
                break;
            }
            case CALL_SEM_OPEN: {
                _sem**   handle = (_sem**)regs[11];        // a1 = &handle
                unsigned init   = (unsigned)regs[12];      // a2 = init
                ret = (uint64)_sem::open(handle, init);
                break;
            }
            case CALL_SEM_CLOSE: {
                ret = (uint64)_sem::close((_sem*)regs[11]); // a1 = rucka
                break;
            }
            case CALL_SEM_WAIT: {
                ret = (uint64)_sem::wait((_sem*)regs[11]);
                break;
            }
            case CALL_SEM_SIGNAL: {
                ret = (uint64)_sem::signal((_sem*)regs[11]);
                break;
            }
            case CALL_SEM_WAIT_N: {
                _sem*    id = (_sem*)regs[11];              // a1 = rucka
                unsigned n  = (unsigned)regs[12];          // a2 = n
                ret = (uint64)_sem::wait_n(id, n);
                break;
            }
            case CALL_SEM_SIGNAL_N: {
                _sem*    id = (_sem*)regs[11];
                unsigned n  = (unsigned)regs[12];
                ret = (uint64)_sem::signal_n(id, n);
                break;
            }
            case CALL_TIME_SLEEP: {
                // STUB za 20p (nema pravog uspavljivanja - Task 4).
                // Samo jednom ustupi procesor da ne bude potpuni no-op.
                _thread::dispatch();
                ret = 0;
                break;
            }
            case CALL_GETC: {
                ret = (uint64)__getc();
                break;
            }
            case CALL_PUTC: {
                __putc((char)regs[11]);   // a1 = znak; koristimo console.lib
                ret = 0;
                break;
            }
            default:
                ret = (uint64)-1;    // nepoznat / neimplementiran poziv
                break;
        }

        regs[10] = ret;                          // povratna vrednost -> a0
        Riscv::w_sepc(sepc + 4);                 // vrati SACUVANI sepc + 4 (preskoci ecall)
        Riscv::w_sstatus(sstatus);               // vrati SACUVANI sstatus (SPP/SPIE ove niti)
    }
    else if (scause == 0x8000000000000001UL) {   // tajmer (softverski prekid)
        // Za 20 poena: samo POTVRDI prekid, BEZ promene konteksta (nema preotimanja).
        // Bez ovoga isti prekid odmah ponovo okida -> beskonacna petlja.
        Riscv::mc_sip(Riscv::SIP_SSIP);
    }
    else if (scause == 0x8000000000000009UL) {   // spoljasnji prekid (konzola)
        console_handler();                        // iz console.lib (za getc/putc kasnije)
    }
    else {
        // Neobradjen izuzetak (npr. ilegalna instrukcija scause=2 - Test 7).
        // Ispisi dijagnostiku i ZAUSTAVI emulator. Ovo NIJE regularan zavrsetak
        // (program je pukao), pa zadovoljava ocekivanje Testa 7.
        kernelprintString("[trap] neobradjen izuzetak\n");
        kernelprintString("  scause="); kernelprintInteger(scause);            kernelprintString("\n");
        kernelprintString("  sepc=");   kernelprintInteger(sepc);              kernelprintString("\n");
        kernelprintString("  stval=");  kernelprintInteger(Riscv::r_stval());  kernelprintString("\n");
        *(volatile uint32*)0x100000 = 0x5555;   // zaustavi QEMU
        while (true) {}                          // osiguranje dok se ne ugasi
    }
}
