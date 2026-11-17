#ifndef QUEUE_H
#define QUEUE_H
#include <stddef.h>
typedef struct QueueNode QueueNode;
typedef struct {
    QueueNode *head;
    size_t count;
} PriorityQueue;
typedef struct {
    unsigned char priority;
    unsigned long id;
    char text[128];
} Message;
void queue_init(PriorityQueue *queue);
void queue_clear(PriorityQueue *queue);
size_t queue_size(const PriorityQueue *queue);
int queue_push(PriorityQueue *queue, const Message *message);
int queue_pop_first(PriorityQueue *queue, Message *out);
int queue_pop_exact(PriorityQueue *queue, unsigned priority, Message *out);
int queue_pop_at_least(PriorityQueue *queue, unsigned min_priority, Message *out);
#endif
