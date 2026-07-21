//
// _sem - implementacija semafora.
// Obicni wait/signal su samo wait_n/signal_n sa n=1 (DRY + ispravno mesanje).
//

#include "../h/_sem.hpp"
#include "../h/_thread.hpp"
#include "../h/scheduler.hpp"
#include "../h/MemoryAllocator.hpp"   // nas alokator (blokovi)

// _sem objekti se prave u jezgru -> alokacija alokatorom DIREKTNO.
// size je u BAJTOVIMA -> konverzija u BLOKOVE (nas alokator radi u blokovima).
void* _sem::operator new(size_t size) {
    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::_mem_alloc(blocks);
}
void  _sem::operator delete(void* p)  { MemoryAllocator::_mem_free(p); }

// --- interne pomocne operacije ---

// Blokiraj TEKUCU nit na semaforu s: oznaci blokiranom, ubaci u red, predji na drugu.
// Kad se nit kasnije probudi (unblock/close), izvrsavanje se vraca ovde.
void _sem::block(_sem* s) {
    _thread* t = _thread::running;
    t->setBlocked(true);
    t->setSemResult(0);          // podrazumevano; konacni rezultat postavlja onaj ko budi
    s->blocked.addLast(t);
    _thread::dispatch();          // promena konteksta; vraca se OVDE kad nit opet dobije procesor
}

// Odblokiraj PRVU nit iz reda semafora s (uspeh): vrati je u spremne.
void _sem::unblock(_sem* s) {
    _thread* t = s->blocked.removeFirst();
    if (t == nullptr) { return; }
    t->setBlocked(false);
    t->setSemResult(0);          // 0 = probudio signal (uspeh)
    Scheduler::put(t);
}

// --- javne operacije ---

int _sem::open(_sem** handle, unsigned init) {
    if (handle == nullptr) { return -1; }
    _sem* s = new _sem();         // operator new -> mem.lib; konstruktor pravi prazan red
    if (s == nullptr) { *handle = nullptr; return -1; }
    s->val = (long long)init;
    s->closed = false;
    *handle = s;
    return 0;
}

int _sem::close(_sem* handle) {
    if (handle == nullptr) { return -1; }
    if (handle->closed)    { return -1; }
    handle->closed = true;

    // deblokiraj SVE niti koje su cekale -> njihov wait vraca gresku (-1)
    _thread* t;
    while ((t = handle->blocked.removeFirst()) != nullptr) {
        t->setBlocked(false);
        t->setSemResult(-1);      // -1 = semafor zatvoren dok je nit cekala
        Scheduler::put(t);
    }

    delete handle;                // operator delete -> mem.lib
    return 0;
}

int _sem::wait(_sem* id) {
    return wait_n(id, 1);         // obicni wait = wait_n sa n=1
}

int _sem::signal(_sem* id) {
    return signal_n(id, 1);       // obicni signal = signal_n sa n=1
}

int _sem::wait_n(_sem* id, unsigned n) {
    if (id == nullptr || n == 0) { return -1; }
    if (id->closed)              { return -1; }

    if (id->val >= (long long)n) {
        id->val -= (long long)n;  // ima dovoljno resursa -> uzmi n i produzi
        return 0;
    }

    // nema dovoljno -> zapamti koliko trazi i blokiraj se
    _thread* t = _thread::running;
    t->setSemAmount((long long)n);
    block(id);
    return (int)t->getSemResult();   // 0 (signal) ili -1 (close)
}

int _sem::signal_n(_sem* id, unsigned n) {
    if (id == nullptr || n == 0) { return -1; }
    if (id->closed)              { return -1; }

    id->val += (long long)n;      // dodaj n jedinica

    // Probudi niti FIFO redom, dok se trazena kolicina prve moze zadovoljiti.
    while (!id->blocked.isEmpty()) {
        _thread* first = id->blocked.peekFirst();
        if (first->getSemAmount() <= id->val) {
            id->val -= first->getSemAmount();
            unblock(id);          // skida first iz reda, result=0, u scheduler
        } else {
            break;                // prva ne moze -> stani (FIFO, bez preskakanja)
        }
    }
    return 0;
}
