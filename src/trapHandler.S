.extern handleTrap
.global trapEntry
.align 4
trapEntry:
    #napravi 256B mesta na steku tekuce niti(32 registra * 8B)
    addi sp, sp, -256

    #sacuvaj x1 (ra) i x3 do x31 (preskacemo x0=zero i x2=sp)
    .irp index, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    #a0 = pokazivac na sacuvane registre tj niz
    mv a0, sp

    call handleTrap

    #vrati sve registre nazad (i eventualno izmenjen a0 = povratna vrednost)
    .irp index, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    #return iz prekida: pc <- sepc, rezim <- SPP, SIE <- SPIE
    sret