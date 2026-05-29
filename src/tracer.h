#pragma once
#include <sys/types.h>
#include "syscalls.h"

typedef void (*event_cb)(const SyscallEvent *ev);

int tracer_run(const char *path, char *const argv[], event_cb cb);
