#include <stdio.h>
#include <stdint.h>

#define MAX_BUF 4
typedef struct {
    uint8_t buffer[MAX_BUF];
    int start;
    int size;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rg) {
    rg->start = 0;
    rg->size = 0;
}

int ring_buffer_enqueue(ring_buffer_t *rg, uint8_t c) {
    if (rg->size == MAX_BUF) {
        // it's full
        return 0;
    }
    rg->buffer[(rg->start + rg->size) % MAX_BUF] = c;
    rg->size++;
    return 1;
}

int ring_buffer_dequeue(ring_buffer_t *rg, uint8_t *c) {
    if (rg->size == 0) {
        // it's empty
        return 0;
    }
    *c = rg->buffer[rg->start];
    rg->start = (rg->start + 1) % MAX_BUF;
    rg->size--;
    return 1;
}

void ring_buffer_print_all(ring_buffer_t *rg) {
    for (int i = 0; i < rg->size; i++) {
        printf("%c ", rg->buffer[(rg->start + i) % MAX_BUF]);
    }
    printf("\n");
}

void enqueue(ring_buffer_t *buf, uint8_t c) {
    int status;
    status = ring_buffer_enqueue(buf, c);
    if (status) {
        printf("++enq++: %c\n", c);
    }
    else {
        printf("could not enqueue: %c\n", c);
    }
    ring_buffer_print_all(buf);
}

void dequeue(ring_buffer_t *buf) {
    uint8_t c;
    int status;
    status = ring_buffer_dequeue(buf, &c);
    if (status) {
        printf("--DEQ--: %c\n", c);
    }
    else {
        printf("could not dequeue\n");
    }
    ring_buffer_print_all(buf);
}

int main() {
    ring_buffer_t buf;
    ring_buffer_init(&buf);

    enqueue(&buf, 'a');
    enqueue(&buf, 'b');
    enqueue(&buf, 'c');
    enqueue(&buf, 'd');
    dequeue(&buf);
    enqueue(&buf, 'z');
    dequeue(&buf);
    enqueue(&buf, 'q');
    enqueue(&buf, 'j');
    dequeue(&buf);
    dequeue(&buf);
    dequeue(&buf);
    dequeue(&buf);
    dequeue(&buf);
    enqueue(&buf, 'k');
    enqueue(&buf, 'l');
    enqueue(&buf, 'u');
    enqueue(&buf, 'q');
    enqueue(&buf, 'j');
    dequeue(&buf);
    dequeue(&buf);
    enqueue(&buf, 'q');
    enqueue(&buf, 'q');
    enqueue(&buf, 'j');
    enqueue(&buf, 'j');
}
