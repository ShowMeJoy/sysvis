#pragma once
#include <sys/types.h>

typedef struct {
    pid_t pid;
    pid_t ppid;
    int   alive;
} Process;

void        process_init(void);
Process    *process_get(pid_t pid);
Process    *process_add(pid_t pid, pid_t ppid);
void        process_remove(pid_t pid);
void        process_point_tree(void);
