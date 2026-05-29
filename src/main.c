#include <stdio.h>
#include "tracer.h"
#include "syscalls.h"
#include "eventbuf.h"

static EventBuf g_events;

static void on_event(const SyscallEvent *ev) {
    eventbuf_push(&g_events, ev);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [args...]\n", argv[0]);
        return 1;
    }
    eventbuf_init(&g_events);
    int ret = tracer_run(argv[1], argv + 1, on_event);
    SyscallEvent ev;
    while (eventbuf_pop(&g_events, &ev))
        syscall_print(&ev);
    
    return ret;
}