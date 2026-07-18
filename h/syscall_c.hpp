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


#endif // SYSCALL_C_HPP
