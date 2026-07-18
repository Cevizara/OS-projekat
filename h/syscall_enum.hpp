#ifndef SYSCALL_ENUM_HPP
#define SYSCALL_ENUM_HPP

#include "../lib/hw.h"

enum SyscallCode : uint64 {
    CALL_MEM_ALLOC       = 0x01,
    CALL_MEM_FREE       = 0x02,

    CALL_THREAD_CREATE   = 0x11,  // napravi nit nad funkcijom (arg: handle, telo, arg, stek)
    CALL_THREAD_EXIT     = 0x12,
    CALL_THREAD_DISPATCH = 0x13, //ustupi CPU drugoj niti

    CALL_SEM_OPEN        = 0x21, //napravi semafor sa pocetnom vr init
    CALL_SEM_CLOSE       = 0x22,
    CALL_SEM_WAIT        = 0x23,
    CALL_SEM_SIGNAL      = 0x24,
    CALL_SEM_WAIT_N      = 0x25,
    CALL_SEM_SIGNAL_N    = 0x26,

    CALL_TIME_SLEEP      = 0x31,

    CALL_GETC            = 0x41,
    CALL_PUTC            = 0x42
};


#endif // SYSCALL_ENUM_HPP