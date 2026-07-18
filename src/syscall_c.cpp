//
// Implementacija C API-ja. Opsta syscall() izvrsava ecall; mem_alloc/mem_free
// su tanki omotaci oko nje. mem_alloc radi konverziju bajtovi -> blokovi (C != ABI).
//

#include "../h/syscall_c.hpp"
#include "../h/syscall_enum.hpp"

//opsta syscall() izvrsava ecall
uint64 syscall(uint64 code, uint64 argument1, uint64 argument2, uint64 argument3, uint64 argument4) {
    register uint64 a0 asm("a0") = code;
    register uint64 a1 asm("a1") = argument1;
    register uint64 a2 asm("a2") = argument2;
    register uint64 a3 asm("a3") = argument3;
    register uint64 a4 asm("a4") = argument4;

    //ecall->trap->jezgro. a0 prvo ulaz (kod) a posle izlaz(povratna vrednost).
    //a0 i r i w
    asm volatile("ecall"
                 : "+r"(a0)
                 : "r"(a1), "r"(a2), "r"(a3), "r"(a4)
                 : "memory");

    return a0;
}


void* mem_alloc(size_t size) {
    if (size == 0) {
        //nema sta da alociramo
        return nullptr;
    }
    //calc blokova(gornja granica)
    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return (void*)syscall(CALL_MEM_ALLOC, (uint64)blocks);
}

int mem_free(void* ptr) {
    return (int)syscall(CALL_MEM_FREE, (uint64)ptr);
}