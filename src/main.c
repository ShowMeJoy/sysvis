#include <stdio.h>
#include "tracer.h"
#include "syscalls.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [args...]\n", argv[0]);
        return 1;
    }
    return tracer_run(argv[1], argv + 1, syscall_print);
}