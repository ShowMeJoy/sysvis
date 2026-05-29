#include <stdio.h>
#include <sys/syscall.h>
#include "syscalls.h"

static const char *names[] = {
    [SYS_read]          = "read",
    [SYS_write]         = "write",
    [SYS_open]          = "open",
    [SYS_close]         = "close",
    [SYS_stat]          = "stat",
    [SYS_fstat]         = "stat",
    [SYS_lstat]         = "lstat",
    [SYS_lseek]         = "lseek",
    [SYS_mmap]          = "mmap",
    [SYS_mprotect]      = "mprotect",
    [SYS_munmap]        = "munmap",
    [SYS_brk]           = "brk",
    [SYS_ioctl]         = "ioctl",
    [SYS_access]        = "access",
    [SYS_pipe]          = "pipe",
    [SYS_dup]           = "dup",
    [SYS_dup2]          = "dup2",
    [SYS_getpid]        = "pid",
    [SYS_socket]        = "socket",
    [SYS_connect]       = "connect",
    [SYS_accept]        = "accept",
    [SYS_bind]          = "bind",
    [SYS_listen]        = "listen",
    [SYS_clone]         = "clone",
    [SYS_fork]          = "fork",
    [SYS_vfork]         = "vfork",
    [SYS_execve]        = "execve",
    [SYS_exit]          = "exit",
    [SYS_wait4]         = "wait4",
    [SYS_kill]          = "kill",
    [SYS_fcntl]         = "fcntl",
    [SYS_getcwd]        = "getcwd",
    [SYS_chdir]         = "chdir",
    [SYS_rename]        = "rename",
    [SYS_mkdir]         = "mkdir",
    [SYS_rmdir]         = "rmdir",
    [SYS_creat]         = "creat",
    [SYS_unlink]        = "unlink",
    [SYS_readlink]      = "readlink",
    [SYS_getuid]        = "getuid",
    [SYS_getgid]        = "getgid",
    [SYS_geteuid]       = "geteuid",
    [SYS_getegid]       = "getegid",
    [SYS_getppid]       = "getppid",
    [SYS_gettid]        = "gettid",
    [SYS_futex]         = "futex",
    [SYS_openat]        = "openat",
    [SYS_getdents64]    = "getdents64",
    [SYS_exit_group]    = "exit_group",
    [SYS_epoll_create]  = "epoll_create",
    [SYS_epoll_wait]    = "epoll_wait",
    [SYS_epoll_ctl]     = "epoll_ctl",
    [SYS_clock_gettime] = "clock_gettime",
    [SYS_accept4]       = "accept4",
    [SYS_dup3]          = "dup3",
    [SYS_pipe2]         = "pipe2",
    [SYS_epoll_create1] = "epoll_create1",
    [SYS_pread64]       = "pread64",
    [SYS_pwrite64]      = "pwrite64",
    [SYS_readv]         = "readv",
    [SYS_writev]        = "writev",
    [SYS_sendto]        = "sendto",
    [SYS_recvfrom]      = "recvfrom",
    [SYS_setsockopt]    = "setsockopt",
    [SYS_getsockopt]    = "getsockopt",
};

#define NAMES_SIZE ((int)(sizeof(names) / sizeof(names[0])))

const char *syscall_name(long sysno) {
    if (sysno >= 0 && sysno < NAMES_SIZE && names[sysno])
        return names[sysno];
    static char buf[32];
    snprintf(buf, sizeof(buf), "syscall_%ld", sysno);
    return buf;
}

void syscall_print(const SyscallEvent *ev) {
    if (ev->entering) return;

    printf("[%5d %-18s", ev->pid, ev->name);

    switch (ev->sysno) {
        case SYS_read:
        case SYS_pread64:
            printf("(fd=%ld, count=%ld)", ev->args[0], ev->args[2]);
            break;
        case SYS_write:
        case SYS_pwrite64:
            printf("(fd=%ld, count=%ld)", ev->args[0], ev->args[2]);
            break;
        case SYS_open:
            printf("(\"%s\", flags=%#lx)", ev->str_arg, ev->args[1]);
            break;
        case SYS_openat:
            printf("(dfd=%ld, \"%s\", flags=%#lx)", ev->args[0], ev->str_arg, ev->args[2]);
            break;
        case SYS_close:
            printf("(fd=%ld)", ev->args[0]);
            break;
        case SYS_connect:
        case SYS_accept:
        case SYS_accept4:
        case SYS_bind:
        case SYS_listen:
            printf("(fd=%ld)", ev->args[0]);
            break;
        case SYS_socket:
            printf("(domain=%ld, type=%ld, proto=%ld)", ev->args[0], ev->args[1], ev->args[2]);
            break;
        case SYS_fork:
        case SYS_vfork:
            printf("()");
            break;
        case SYS_clone:
            printf("(flags=%#lx)", ev->args[0]);
            break;
        case SYS_execve:
            printf("(\"%s\")", ev->str_arg);
            break;
        case SYS_exit:
        case SYS_exit_group:
            printf("(code=%ld)", ev->args[0]);
            break;
        case SYS_stat:
        case SYS_lstat:
        case SYS_access:
        case SYS_mkdir:
        case SYS_rmdir:
        case SYS_unlink:
        case SYS_creat:
        case SYS_chdir:
            printf("(\"%s\")", ev->str_arg);
            break;
        default:
            printf("(%ld, %ld, %ld)", ev->args[0], ev->args[1], ev->args[2]);
            break;
    }
printf(" = %ld\n", ev->retval);
}
