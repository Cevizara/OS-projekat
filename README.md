# RISC-V Educational Kernel

A small educational operating-system kernel developed for the Operating Systems 1 course at the University of Belgrade, School of Electrical Engineering. It targets 64-bit RISC-V and runs in QEMU as a statically linked program with a user application.

The current implementation includes a first-fit memory allocator, cooperative threads and context switching, a scheduler, semaphores, trap handling, and layered ABI, C, and C++ system-call interfaces. Timer-based preemption and full thread sleeping are not yet implemented.

## Build and run

The project requires a RISC-V cross-compiler, GNU Make, and `qemu-system-riscv64`. Link or add a test application that defines `void userMain()`, then run:

```sh
make
make qemu
```
