#pragma once
#include <sys/types.h>

typedef struct {
    pid_t       pid;
    int         entering;   /* 1 = вход, 0 = выход из syscall*/
    long        sysno;
    const char *name;
    long        args[6];
    long        retval;
    char        str_arg[256];   /* предвычитанный строковой аргумент */
} SyscallEvent;

const char   *syscall_name(long sysno);
void          syscall_print(const SyscallEvent *ev);

