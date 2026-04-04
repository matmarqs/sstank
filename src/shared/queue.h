#ifndef _QUEUE_H
#define _QUEUE_H

#ifndef DEFINE_QUEUE

#ifndef QUEUE_MAX_SIZE
#define QUEUE_MAX_SIZE 20
#endif // QUEUE_MAX_SIZE

#define DEFINE_QUEUE(name, prefix, type)                            \
    typedef struct {                                                \
        type buffer[QUEUE_MAX_SIZE];                                \
        int start;                                                  \
        int size;                                                   \
    } name;                                                         \
                                                                    \
    void prefix##_init(name *rg) {                                  \
        rg->start = 0;                                              \
        rg->size = 0;                                               \
    }                                                               \
                                                                    \
    int prefix##_enqueue(name *rg, type c) {                        \
        if (rg->size == QUEUE_MAX_SIZE) {                           \
            return 0;                                               \
        }                                                           \
        rg->buffer[(rg->start + rg->size) % QUEUE_MAX_SIZE] = c;    \
        rg->size++;                                                 \
        return 1;                                                   \
    }                                                               \
                                                                    \
    int prefix##_dequeue(name *rg, type *c) {                       \
        if (rg->size == 0) {                                        \
            return 0;                                               \
        }                                                           \
        *c = rg->buffer[rg->start];                                 \
        rg->start = (rg->start + 1) % QUEUE_MAX_SIZE;               \
        rg->size--;                                                 \
        return 1;                                                   \
    }

#endif // DEFINE_QUEUE

#endif // _QUEUE_H
