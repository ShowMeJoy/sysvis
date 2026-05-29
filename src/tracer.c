#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "tracer.h"
#include "syscalls.h"
#include "process.h"

#define MAX_PIDS 512
static struct { pid_t pid; int entering; } pstate[MAX_PIDS];

static int get_entering(pid_t pid) {
    for (int i = 0; i < MAX_PIDS; ++i) 
        if (pstate[i].pid == pid)
            return pstate[i].entering;
    return 1; /* default entering */
}

static void set_entering(pid_t pid, int val) {
    for (int i = 0; i < MAX_PIDS; ++i) {
        if (pstate[i].pid == pid || pstate[i].pid == 0) {
            pstate[i].pid       = pid;
            pstate[i].entering  = val;
            return;
        }
    }
}

static void remove_pstate(pid_t pid) {
    for (int i = 0; i < MAX_PIDS; ++i)
    if (pstate[i].pid == pid) { pstate[i].pid = 0; return; }
}

static void peek_string(pid_t pid, long addr, char *buf, size_t size) {
    size_t i = 0;
    while (i < size - 1) {
        errno = 0;
        long word = ptrace(PTRACE_PEEKDATA, pid, (void *)(addr + i), NULL);
        if (word == -1 && errno) break;
        size_t copy = sizeof(long);
        if (i + copy > size - 1) copy = size - 1 - i;
        memcpy(buf + i, &word, copy);
        for (size_t j = 0; j < copy; ++j) 
            if (buf[i + j] == '\0') return;
        i += sizeof(long);
    }
    buf[i < size ? i : size - 1] = '\0';
}

static int str_arg_index(long sysno) {
    switch (sysno) {
        case SYS_open: case SYS_stat: case SYS_lstat: case SYS_access:
        case SYS_mkdir: case SYS_rmdir: case SYS_unlink: case SYS_creat:
        case SYS_chdir: case SYS_readlink: case SYS_execve:
            return 0;
        case SYS_openat: case SYS_rename:
            return 1;
        default:
            return -1;
    }
}

int tracer_run(const char *path, char *const argv[], event_cb cb) {
    process_init();

    memset(pstate, 0, sizeof(pstate));

    pid_t root = fork();
    if (root < 0) { perror("fork"); return -1; }

    if (root == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(path, argv);
        perror("execvp");
        _exit(1);
    }

    process_add(root, getpid());    // регистрируем root-процесс в нашей таблице

    int wstatus;
    waitpid(root, &wstatus, 0);     // ждём первую остановку (после exec)

    long opts = PTRACE_O_TRACESYSGOOD
              | PTRACE_O_TRACEFORK
              | PTRACE_O_TRACEVFORK
              | PTRACE_O_TRACECLONE;
    ptrace(PTRACE_SETOPTIONS, root, NULL, opts);
    ptrace(PTRACE_SYSCALL, root, NULL, NULL);   // запускаем, остановись на след. syscall

    while (1) {
        // -1 means we are waiting for any process
        pid_t pid = waitpid(-1, &wstatus, 0);
        if (pid < 0) {
            if (errno == ECHILD) break;
            perror("waitpid");
            break;
        }

        if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
            process_remove(pid);
            remove_pstate(pid);
            continue;
        }

        if (!WIFSTOPPED(wstatus)) {
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            continue;
        }

        int sig = WSTOPSIG(wstatus);
        
        /* new child process from fork/vfork/clone */
        if (wstatus >> 8 == (SIGTRAP | (PTRACE_EVENT_FORK  << 8)) ||
            wstatus >> 8 == (SIGTRAP | (PTRACE_EVENT_VFORK << 8)) ||
            wstatus >> 8 == (SIGTRAP | (PTRACE_EVENT_CLONE << 8))) {
            unsigned long child_pid;
            ptrace(PTRACE_GETEVENTMSG, pid, NULL, &child_pid);
            process_add((pid_t)child_pid, pid);
            ptrace(PTRACE_SETOPTIONS, (pid_t)child_pid, NULL, opts);
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            continue;
        }

        /* syscall stop: bit 7 set due to PTRACE_O_TRACESYSGOOD */
        if (sig == (SIGTRAP | 0x80)) {
            struct user_regs_struct regs;
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);

            int entering = get_entering(pid);
            set_entering(pid, !entering);

            SyscallEvent ev = {0};
            ev.pid      = pid;
            ev.entering = entering;
            ev.sysno    = regs.orig_rax;
            ev.name     = syscall_name(ev.sysno);
            ev.args[0]  = regs.rdi;
            ev.args[1]  = regs.rsi;
            ev.args[2]  = regs.rdx;
            ev.args[3]  = regs.r10;
            ev.args[4]  = regs.r8;
            ev.args[5]  = regs.r9;
            ev.retval   = entering ? 0 : (long)regs.rax;

            int idx = str_arg_index(ev.sysno);
            if (idx >= 0 && ev.args[idx])
                peek_string(pid, ev.args[idx], ev.str_arg, sizeof(ev.str_arg));
            cb(&ev);

            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            continue;
        }

        /* pass real signals through, drop SIGTRAP/SIGSTOP */
        int deliver = (sig == SIGTRAP || sig == SIGSTOP) ? 0 : sig;
        ptrace(PTRACE_SYSCALL, pid, NULL, (void *)(long)deliver);
    }

    return 0;
}