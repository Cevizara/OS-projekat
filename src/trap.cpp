//
// C deo prekidne rutine. Za sada samo prepoznaje uzrok i (za ecall)
// pomera sepc za 4 da se ne bi ecall izvrsavao u beskonacnoj petlji.
// Kasnije ce ovde biti razgranati skok po kodu sistemskog poziva.
//

#include "../h/riscv.hpp"
#include "../h/print.hpp"
#include "../lib/mem.h"

extern "C" void handleTrap(uint64* regs) {
    uint64 scause = Riscv::r_scause();

    if (scause == 0x08UL || scause == 0x09UL) {
        uint64 kod=regs[10]
        uint64 ret  = (uint64)-1;

        switch(kod){
            case CALL_MEM_ALLOC:{
                size_t blocks=(size_t) regs[11];
                ret=(uint64)_mem_alloc(blocks * MEM_BLOCK_SIZE)
                break;
            }
            case CALL_MEM_FREE: {
                ret = (uint64)__mem_free((void*)regs[11]);
                break;
            }
            default:
                ret = (uint64)-1;    // nepoznat/neimplementiran poziv
                break;
        }
        regs[10]= ret;

        // preskoci ecall instrukciju (4 bajta) da se ne ponavlja
        uint64 sepc = Riscv::r_sepc();
        Riscv::w_sepc(sepc + 4);
    } else {
        kernelprintString("[trap] NEPOZNAT uzrok, scause=");
        kernelprintInteger(scause);
        kernelprintString("\n");
    }
}
