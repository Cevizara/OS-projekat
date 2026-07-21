# Promena konteksta izmedju dve niti.
# C++ potpis: static void _thread::promenaKonteksta(Context* oldC, Context* newC);
#   a0 = oldC (stara nit), a1 = newC (nova nit)
#   Context = { uint64 ra; uint64 sp; }  -> ra na offsetu 0, sp na offsetu 8
#
# Posto je metoda clan klase _thread, labela je "mangled" (kodirano) ime:
#   _ZN7_thread16promenaKontekstaEPNS_7ContextES1_
#   ( _thread(7) :: promenaKonteksta(16), param: pokazivac na _thread::Context, pa isti tip )
#
# Sacuva ra+sp tekuce (stare) niti, ucita ra+sp nove, i 'ret' skoci na NOVI ra.

.global _ZN7_thread16promenaKontekstaEPNS_7ContextES1_
.type   _ZN7_thread16promenaKontekstaEPNS_7ContextES1_, @function
_ZN7_thread16promenaKontekstaEPNS_7ContextES1_:
    sd ra, 0 * 8(a0)      # oldC->ra = trenutni ra
    sd sp, 1 * 8(a0)      # oldC->sp = trenutni sp

    ld ra, 0 * 8(a1)      # ra = newC->ra
    ld sp, 1 * 8(a1)      # sp = newC->sp

    ret                    # 'ret' skace na (novi) ra -> teleport u novu nit
