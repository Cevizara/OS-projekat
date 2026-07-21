//
// MemoryAllocator - implementacija (first fit, dvostruko ulancana free-lista).
//

#include "../h/MemoryAllocator.hpp"

MemoryAllocator::FreeBlock* MemoryAllocator::head = nullptr;
bool MemoryAllocator::initialized = false;

uint64 MemoryAllocator::alignUp(uint64 addr, uint64 align) {
    return ((addr + align - 1) / align) * align;
}

uint64 MemoryAllocator::alignDown(uint64 addr, uint64 align) {
    return (addr / align) * align;
}

// Lenja inicijalizacija: ceo heap postaje JEDAN veliki slobodan komad.
void MemoryAllocator::init() {
    uint64 start = alignUp((uint64)HEAP_START_ADDR, MEM_BLOCK_SIZE);
    uint64 end   = alignDown((uint64)HEAP_END_ADDR, MEM_BLOCK_SIZE);

    head = (FreeBlock*)start;
    head->blocks = (end - start) / MEM_BLOCK_SIZE;
    head->prev = nullptr;
    head->next = nullptr;
    initialized = true;
}

// FIRST FIT: prvi slobodan komad >= trazenog; po potrebi ga iseci.
void* MemoryAllocator::_mem_alloc(size_t blocks) {
    if (!initialized) { init(); }
    if (blocks == 0)  { return nullptr; }

    size_t needed = blocks + 1;   // +1 blok za header

    FreeBlock* curr = head;
    while (curr != nullptr && curr->blocks < needed) {
        curr = curr->next;
    }
    if (curr == nullptr) { return nullptr; }

    if (curr->blocks == needed) {
        // tacno odgovara -> izbaci ceo komad iz liste
        if (curr->prev) { curr->prev->next = curr->next; }
        else            { head = curr->next; }
        if (curr->next) { curr->next->prev = curr->prev; }
    } else {
        // veci -> odseci 'needed' sa pocetka, ostatak (remaining) na mesto curr-a
        FreeBlock* remaining = (FreeBlock*)((uint64)curr + needed * MEM_BLOCK_SIZE);
        remaining->blocks = curr->blocks - needed;
        remaining->prev   = curr->prev;
        remaining->next   = curr->next;
        if (curr->prev) { curr->prev->next = remaining; }
        else            { head = remaining; }
        if (curr->next) { curr->next->prev = remaining; }
    }

    AllocHeader* header = (AllocHeader*)curr;
    header->blocks = needed;
    return (void*)((uint64)header + MEM_BLOCK_SIZE);
}

// Vrati komad u sortiranu free-listu, pa spoji sa susedima.
int MemoryAllocator::_mem_free(void* ptr) {
    if (ptr == nullptr) { return -1; }
    if ((uint64)ptr < (uint64)HEAP_START_ADDR ||
        (uint64)ptr >= (uint64)HEAP_END_ADDR) { return -1; }
    if (!initialized) { init(); }

    // header je jedan blok ISPRED vracene adrese
    AllocHeader* header = (AllocHeader*)((uint64)ptr - MEM_BLOCK_SIZE);

    // taj isti prostor sada postaje FreeBlock
    FreeBlock* node = (FreeBlock*)header;
    size_t nodeBlocks = header->blocks;
    node->blocks = nodeBlocks;
    node->prev = nullptr;
    node->next = nullptr;

    // nadji poziciju u sortiranoj listi (prvi cvor sa adresom > node)
    FreeBlock* curr = head;
    FreeBlock* prev = nullptr;
    while (curr != nullptr && (uint64)curr < (uint64)node) {
        prev = curr;
        curr = curr->next;
    }

    // ubaci node izmedju prev i curr
    node->prev = prev;
    node->next = curr;
    if (prev) { prev->next = node; }
    else      { head = node; }
    if (curr) { curr->prev = node; }

    // spoji sa desnim, pa levog sa njim (prev pokriva levo spajanje)
    merge(node);
    if (prev) { merge(prev); }

    return 0;
}

// Ako je 'node' fizicki spojen sa svojim SLEDECIM, spoji ih u jedan.
void MemoryAllocator::merge(FreeBlock* node) {
    FreeBlock* next = node->next;
    if (next != nullptr &&
        (uint64)node + node->blocks * MEM_BLOCK_SIZE == (uint64)next) {
        node->blocks += next->blocks;
        node->next = next->next;
        if (next->next) { next->next->prev = node; }
    }
}
