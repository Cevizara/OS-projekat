//
// MemoryAllocator: uslužna (singleton) klasa - kontinualna alokacija (first fit).
//
// Ideja: ceo heap [HEAP_START_ADDR, HEAP_END_ADDR) delimo na blokove velicine
// MEM_BLOCK_SIZE. Slobodni komadi su ulancani u free-listu (sortiranu po adresi),
// a cvorovi liste (FreeBlock) su smesteni U SAMIM slobodnim komadima -> nula
// dodatne memorije za evidenciju. Alocirani komad na pocetku nosi header sa
// svojom velicinom (da free zna koliko da oslobodi).
//
// Sve velicine su u BLOKOVIMA (ABI nivo). C API mem_alloc bajtove->blokove.
//

#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
private:
    // cvor free-liste (DVOSTRUKO ulancana, sortirana po adresi),
    // smesten na pocetak slobodnog komada
    struct FreeBlock {
        size_t     blocks;   // velicina ovog slobodnog komada (u blokovima)
        FreeBlock* prev;     // prethodni slobodan komad
        FreeBlock* next;     // sledeci slobodan komad
    };

    // header alociranog komada, smesten na njegov pocetak
    // (alociran NIJE u listi -> ne treba mu prev/next, samo velicina)
    struct AllocHeader {
        size_t blocks;       // ukupna velicina komada u blokovima (ukljucujuci header)
    };

    static FreeBlock* head;        // pocetak free-liste
    static bool       initialized;

    static void init();            // lenja inicijalizacija (na prvi poziv)
    static void merge(FreeBlock* node);   // spoji node sa susedima ako su fizicki susedni
    static uint64 alignUp(uint64 addr, uint64 align);
    static uint64 alignDown(uint64 addr, uint64 align);

public:
    static void* _mem_alloc(size_t blocks);  // alociraj 'blocks' blokova; vrati pokazivac ili nullptr
    static int   _mem_free(void* ptr);        // oslobodi; 0 uspeh / -1 greska
};

#endif // MEMORYALLOCATOR_HPP
