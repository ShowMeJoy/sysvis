
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "process.h"

#define MAX_PROCS 512

static Process procs[MAX_PROCS];

void process_init(void) {
    memset(procs, 0, sizeof(procs));
}

Process *process_get(pid_t pid) {
    for (int i = 0; i < MAX_PROCS; ++i) {
        if (procs[i].alive && procs[i].pid == pid)
            return &procs[i];
    }
    return NULL;
}

Process *process_add(pid_t pid, pid_t ppid) {
    for (int i = 0; i < MAX_PROCS; ++i) {
        if (!procs[i].alive) {
            procs[i].pid   = pid;
            procs[i].ppid  = ppid;
            procs[i].alive = 1;
            return &procs[i];
         }
    }
    return NULL;
}

void process_remove(pid_t pid) {
    for (int i = 0; i < MAX_PROCS; ++i) {
        if (procs[i].alive && procs[i].pid == pid) {
            procs[i].alive = 0;
            return;
        }
    }
}

void process_point_tree(void) {
    printf("Process:\n");
    for (int i = 0; i < MAX_PROCS; ++i)
        if (procs[i].alive)
            printf("  pid=%-6d ppid=%d\n", procs[i].pid, procs[i].ppid);
}
