//
// Korak 2 test: proveri ceo lanac memorije
// mem_alloc -> syscall -> ecall -> handleTrap -> __mem_alloc (mem.lib) -> nazad.
//

#include "../lib/hw.h"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

static void haltQemu() {
    *(volatile uint32*)0x100000 = 0x5555;
}

int main() {
    Riscv::init();                 // stvec -> trapEntry (da bi ecall radio)

    kernelprintString("=== Test memorije ===\n");

    // granice heap-a (samo za uvid da je pokazivac u tom opsegu)
    kernelprintString("HEAP_START = "); kernelprintInteger((uint64)HEAP_START_ADDR); kernelprintString("\n");
    kernelprintString("HEAP_END   = "); kernelprintInteger((uint64)HEAP_END_ADDR);   kernelprintString("\n");

    // 1) alociraj 100 bajtova
    void* p = mem_alloc(100);
    kernelprintString("mem_alloc(100) -> "); kernelprintInteger((uint64)p); kernelprintString("\n");

    if (p != nullptr) {
        // 2) upisi i procitaj -> dokaz da je memorija upotrebljiva
        int* a = (int*)p;
        a[0] = 42;
        a[1] = 1234;
        kernelprintString("a[0] = "); kernelprintInteger((uint64)a[0]); kernelprintString("\n");
        kernelprintString("a[1] = "); kernelprintInteger((uint64)a[1]); kernelprintString("\n");

        // 3) oslobodi
        int r = mem_free(p);
        kernelprintString("mem_free -> "); kernelprintInteger((uint64)r); kernelprintString("\n");
    } else {
        kernelprintString("GRESKA: mem_alloc vratio nullptr\n");
    }

    kernelprintString("=== Kraj ===\n");
    haltQemu();
    return 0;
}
