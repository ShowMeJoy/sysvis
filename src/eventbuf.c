#include "eventbuf.h"

void eventbuf_init(EventBuf *eb) {
    eb->head = 0;
    eb->tail = 0;
    eb->count = 0;
}

int eventbuf_push(EventBuf *eb, const SyscallEvent *ev) {
    eb->buf[eb->tail] = *ev;
    if (eb->count == EVENTBUF_SIZE) {
        eb->head = (eb->head + 1) % EVENTBUF_SIZE;
    } else {
        eb->count++;

    }
    eb->tail = (eb->tail + 1) % EVENTBUF_SIZE;
    return 1;
}

int eventbuf_pop(EventBuf *eb, SyscallEvent *out) {
    if (eb->count == 0) return 0;
    *out = eb->buf[eb->head];
    eb->head = (eb->head + 1) % EVENTBUF_SIZE;
    eb->count--;
    return 1;
}