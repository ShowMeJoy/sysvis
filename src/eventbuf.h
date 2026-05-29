#pragma once
#include "syscalls.h"

#define EVENTBUF_SIZE 1024

typedef struct {
    SyscallEvent buf[EVENTBUF_SIZE]; 
    int head;
    int tail;
    int count;
} EventBuf;

void eventbuf_init(EventBuf *eb);
int eventbuf_push(EventBuf *eb, const SyscallEvent *ev);
int eventbuf_pop(EventBuf *eb, SyscallEvent *out);