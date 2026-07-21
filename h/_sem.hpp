//
// _sem: interna klasa jezgra koja predstavlja semafor.
// Ime MORA biti _sem (C API: typedef _sem* sem_t) zbog app.lib.
//
// Semafor = vrednost (broj resursa) + red niti koje cekaju (blocked).
//   wait:   ako val>0 -> val--; inace blokiraj nit (u red + dispatch)
//   signal: ako ima nit u redu -> odblokiraj je; inace val++
//

#ifndef _SEM_HPP
#define _SEM_HPP

#include "../lib/hw.h"
#include "list.hpp"        // red niti koje cekaju: List<_thread>

class _thread;             // forward-decl (cuvamo samo pokazivace)

class _sem {
public:
    // sve operacije su staticke (rade nad prosledjenom ruckom)
    static int open(_sem** handle, unsigned init);
    static int close(_sem* handle);
    static int wait(_sem* id);
    static int signal(_sem* id);
    static int wait_n(_sem* id, unsigned n);
    static int signal_n(_sem* id, unsigned n);

    // _sem objekti se prave u jezgru -> alokacija preko mem.lib (kao _thread)
    void* operator new(size_t size);
    void  operator delete(void* p);

private:
    long long     val;       // vrednost (broj resursa); moze pasti <0 = broj blokiranih
    bool          closed;    // da li je semafor zatvoren
    List<_thread> blocked;   // red niti koje cekaju na ovom semaforu

    // pomocne (interne) operacije blokiranja/odblokiranja tekuce/prve niti
    static void block(_sem* s);     // blokiraj tekucu nit na semaforu s
    static void unblock(_sem* s);   // odblokiraj prvu nit iz reda semafora s
};

#endif // _SEM_HPP
