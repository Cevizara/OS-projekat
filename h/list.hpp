//
// Genericka jednostruko ulancana lista (drzi pokazivace na T).
// Koriste je Scheduler (red spremnih) i semafori (red blokiranih).
// Cvorovi (Elem) se alociraju UNUTAR jezgra -> direktno preko mem.lib
// (NE preko sistemskog poziva mem_alloc, jer bismo pravili ugnezdjeni trap).
// U koraku 10 ce se __mem_alloc/__mem_free zameniti nasim MemoryAllocator-om.
//

#ifndef LIST_HPP
#define LIST_HPP

#include "../lib/hw.h"
#include "../lib/mem.h"

template<typename T>
class List {
private:
    struct Elem {
        T* data;
        Elem* next;

        Elem(T* data, Elem* next) : data(data), next(next) {}

        void* operator new(size_t size) { return __mem_alloc(size); }
        void  operator delete(void* p)  { __mem_free(p); }
    };

    Elem* head;
    Elem* tail;

public:
    List() : head(nullptr), tail(nullptr) {}

    // lista poseduje svoje cvorove -> zabranjujemo kopiranje (izbegava dvostruko brisanje)
    List(const List<T>&) = delete;
    List<T>& operator=(const List<T>&) = delete;

    bool isEmpty() const { return head == nullptr; }

    void addFirst(T* data) {
        Elem* e = new Elem(data, head);
        head = e;
        if (tail == nullptr) { tail = head; }
    }

    void addLast(T* data) {
        Elem* e = new Elem(data, nullptr);
        if (tail != nullptr) {
            tail->next = e;
            tail = e;
        } else {
            head = tail = e;
        }
    }

    T* removeFirst() {
        if (head == nullptr) { return nullptr; }
        Elem* e = head;
        head = head->next;
        if (head == nullptr) { tail = nullptr; }
        T* ret = e->data;
        delete e;
        return ret;
    }

    T* peekFirst() const { return head ? head->data : nullptr; }
    T* peekLast()  const { return tail ? tail->data : nullptr; }

    // izbaci odredjeni element po vrednosti pokazivaca (za semafore / modifikacije)
    void removeElem(T* data) {
        Elem* prev = nullptr;
        Elem* cur = head;
        while (cur != nullptr && cur->data != data) {
            prev = cur;
            cur = cur->next;
        }
        if (cur == nullptr) { return; }              // nije nadjen
        if (prev == nullptr) { head = cur->next; }   // bio prvi
        else { prev->next = cur->next; }
        if (cur == tail) { tail = prev; }
        delete cur;
    }

    int size() const {
        int n = 0;
        for (Elem* c = head; c != nullptr; c = c->next) { n++; }
        return n;
    }
};

#endif // LIST_HPP
