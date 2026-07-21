//
// C++ API - implementacija. Svaka klasa je tanak omotac oko C API-ja:
// drzi samo rucku (myHandle) i prosledjuje pozive C funkcijama.
//

#include "../h/syscall_cpp.hpp"

// --- globalni new/delete -> tvoj alokator (mem_alloc/mem_free) ---
void* operator new(size_t size)        { return mem_alloc(size); }
void* operator new[](size_t size)      { return mem_alloc(size); }

void operator delete(void* ptr) noexcept            { if (ptr) mem_free(ptr); }
void operator delete[](void* ptr) noexcept          { if (ptr) mem_free(ptr); }
void operator delete(void* ptr, size_t) noexcept    { if (ptr) mem_free(ptr); }
void operator delete[](void* ptr, size_t) noexcept  { if (ptr) mem_free(ptr); }

// --- Thread ---

Thread::Thread(void (*body)(void*), void* arg)
    : myHandle(nullptr), body(body), arg(arg) {}

Thread::Thread()
    : myHandle(nullptr), body(nullptr), arg(nullptr) {}

Thread::~Thread() {
    // Za 20p nema thread_destroy sistemskog poziva; ciscenje se ne radi ovde.
}

int Thread::start() {
    if (myHandle != nullptr) { return -1; }        // vec pokrenuta
    if (body != nullptr) {
        // Konstruisana sa pokazivacem na funkciju -> telo je ta funkcija (run se ignorise).
        return thread_create(&myHandle, body, arg);
    }
    // Izvedena klasa sa run() -> podmetni threadWrapper koji ce pozvati run().
    return thread_create(&myHandle, &Thread::threadWrapper, this);
}

void Thread::dispatch() { thread_dispatch(); }

int Thread::sleep(time_t time) { return time_sleep(time); }

// Most izmedju C tela niti i objektnog run(): arg = pokazivac na Thread objekat.
void Thread::threadWrapper(void* arg) {
    Thread* self = (Thread*)arg;
    if (self != nullptr) { self->run(); }
}

// --- Semaphore ---

Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    if (myHandle != nullptr) {
        sem_close(myHandle);
        myHandle = nullptr;
    }
}

int Semaphore::wait()   { return myHandle ? sem_wait(myHandle)   : -1; }
int Semaphore::signal() { return myHandle ? sem_signal(myHandle) : -1; }

// --- PeriodicThread (postoji radi povezivanja; pravi rad je Task 4) ---

PeriodicThread::PeriodicThread(time_t period) : Thread(), period(period) {}

void PeriodicThread::terminate() { period = 0; }

// --- Console (fasada oko getc/putc) ---

char Console::getc()        { return ::getc(); }
void Console::putc(char c)  { ::putc(c); }
