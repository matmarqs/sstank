#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_DECLARE(name, prefix, type, max_size)                     \
    typedef struct {                                                    \
        type buffer[max_size];                                          \
        int start;                                                      \
        int size;                                                       \
    } name;                                                             \
    void prefix##_init(name *q);                                        \
    int prefix##_enqueue(name *q, type item);                           \
    int prefix##_dequeue(name *q, type *out);                           \
    int prefix##_is_empty(name *q);                                     \
    int prefix##_is_full(name *q);

#define QUEUE_IMPLEMENT(name, prefix, type, max_size)                   \
    void prefix##_init(name *q) {                                       \
        q->start = 0; q->size = 0;                                      \
    }                                                                   \
    int prefix##_enqueue(name *q, type item) {                          \
        if (q->size >= max_size) return 0;                              \
        q->buffer[(q->start + q->size) % max_size] = item;              \
        q->size++;                                                      \
        return 1;                                                       \
    }                                                                   \
    int prefix##_dequeue(name *q, type *out) {                          \
        if (q->size == 0) return 0;                                     \
        *out = q->buffer[q->start];                                     \
        q->start = (q->start + 1) % max_size;                           \
        q->size--;                                                      \
        return 1;                                                       \
    }                                                                   \
    int prefix##_is_empty(name *q) { return q->size == 0; }             \
    int prefix##_is_full(name *q) { return q->size >= max_size; }

#endif
