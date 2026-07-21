//
// _thread - KORAK 3 (kostur): definicija running, konstruktor, geteri/seteri.
// dispatch/createThread/exit/threadWrapper/contextSwitch/operator new -> koraci 4-6.
//

#include "../h/_thread.hpp"
#include "../h/MemoryAllocator.hpp"   // nas alokator (blokovi)
#include "../h/riscv.hpp"        // Riscv::mc/ms_sstatus, popSppSpie (prelazak u korisnicki rezim)
#include "../h/syscall_c.hpp"    // syscall()
#include "../h/syscall_enum.hpp" // CALL_THREAD_EXIT
#include "../h/scheduler.hpp"    // Scheduler::put / get

// definicija statickog clana (jedna jedina tekuca nit)
_thread* _thread::running = nullptr;

// _thread objekti se prave UNUTAR jezgra -> alociramo alokatorom DIREKTNO
// (ne globalni new, koji bi zvao mem_alloc syscall -> ugnezdjeni trap).
// size je u BAJTOVIMA (sizeof(_thread)); nas alokator radi u BLOKOVIMA -> konverzija.
void* _thread::operator new(size_t size) {
    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::_mem_alloc(blocks);
}

void _thread::operator delete(void* p) {
    MemoryAllocator::_mem_free(p);
}

_thread::_thread(Body body, void* arg, void* stack)
    : body(body), arg(arg), stack(stack),
      context({0, 0}),
      finished(false),
      blocked(false),
      semResult(0),
      semAmount(0)
{
    // Postavi POCETNI kontekst: nit prvi put krece od threadWrapper-a.
    // Glavna (main) nit ima body==nullptr i stack==nullptr -> preskoci;
    // njen kontekst se popuni sam pri prvom contextSwitch-u sa nje.
    if (body != nullptr && stack != nullptr) {
        uint64 sp = (uint64)stack;
        sp &= ~((uint64)0xF);                 // poravnaj sp na 16 (zahtev RISC-V)
        context.ra = (uint64)&threadWrapper;  // prva "povratna adresa" = threadWrapper
        context.sp = sp;                      // svez stek niti
    }
}

// Kroz threadWrapper prolazi svaka nit pri PRVOM pokretanju.
void _thread::threadWrapper() {
    // Prelazak iz sistemskog u KORISNICKI rezim pre poziva tela niti:
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);    // SPP=0 -> sret vraca u korisnicki rezim
    Riscv::ms_sstatus(Riscv::SSTATUS_SPIE);   // SPIE=1 -> posle sret prekidi dozvoljeni
    Riscv::popSppSpie();                       // sepc=ra; sret -> nastavlja OVDE, u korisnickom rezimu

    // Telo niti (izvrsava se u korisnickom rezimu):
    if (running != nullptr && running->body != nullptr) {
        running->body(running->arg);
    }

    // Telo se vratilo -> ugasi nit (sistemski poziv iz korisnickog rezima).
    syscall(CALL_THREAD_EXIT);

    // Ovamo se ne bi smelo doci:
    while (true) {}
}

bool _thread::isFinished()        { return finished; }
void _thread::setFinished(bool f) { finished = f; }
bool _thread::isBlocked()         { return blocked; }
void _thread::setBlocked(bool b)  { blocked = b; }

_thread::Context* _thread::getContext() { return &context; }

long long _thread::getSemResult()          { return semResult; }
void      _thread::setSemResult(long long r){ semResult = r; }
long long _thread::getSemAmount()          { return semAmount; }
void      _thread::setSemAmount(long long n){ semAmount = n; }

// Napravi nit: alociraj objekat (operator new -> mem.lib), postavi kontekst
// (konstruktor), i ubaci je u red spremnih.
int _thread::createThread(_thread** handle, Body body, void* arg, void* stack) {
    if (handle == nullptr || body == nullptr || stack == nullptr) {
        return -1;
    }
    _thread* t = new _thread(body, arg, stack);
    if (t == nullptr) {
        *handle = nullptr;
        return -1;
    }
    *handle = t;
    Scheduler::put(t);
    return 0;
}

// Ustupi procesor sledecoj spremnoj niti (sinhrona promena konteksta).
void _thread::dispatch() {
    _thread* old = running;

    // staru nit vrati u spremne SAMO ako nije zavrsila ni blokirana
    if (old != nullptr && !old->isFinished() && !old->isBlocked()) {
        Scheduler::put(old);
    }

    _thread* next = Scheduler::get();
    if (next == nullptr) {
        // nema drugih spremnih -> ostani na staroj
        running = old;
        return;
    }

    running = next;
    if (old != next) {
        promenaKonteksta(old->getContext(), next->getContext());
    }
}

// Ugasi tekucu nit: oznaci je zavrsenom pa predji na sledecu.
// dispatch nece vratiti zavrsenu nit u spremne, pa se ova vise nikad ne izvrsava.
int _thread::exit() {
    running->setFinished(true);
    dispatch();
    return 0;   // do ovde se prakticno ne dolazi
}
