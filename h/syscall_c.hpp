//
// C API - proceduralni interfejs sistemskih poziva.
// Za sada samo opsta syscall() + memorija; ostalo dodajemo kad zatreba.
//

#ifndef SYSCALL_C_HPP
#define SYSCALL_C_HPP
#include "../lib/hw.h"


//fasada, do 4 arg i izvrsi ecall
uint64 syscall(uint64 code, uint64 argument1 = 0, uint64 argument2 = 0,
                            uint64 argument3 = 0, uint64 argument4 = 0);

//size u B
void* mem_alloc(size_t size);
int   mem_free(void* ptr);

// --- Niti ---
class _thread;
typedef _thread* thread_t;   // "rucka" niti (ime _thread je propisano C API-jem)

class _sem;
typedef _sem* sem_t;         // rucka semafora; implementacija dolazi u Tasku 3

int  thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);
void thread_dispatch();
int  thread_exit();

// --- Semafori ---
int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
int sem_wait_n(sem_t id, unsigned n);
int sem_signal_n(sem_t id, unsigned n);

// --- Konzola (preko console.lib) ---
char getc();
void putc(char c);

// --- Vreme (stub za 20p; postoji radi povezivanja C++ Thread::sleep) ---
int time_sleep(time_t t);


#endif // SYSCALL_C_HPP
