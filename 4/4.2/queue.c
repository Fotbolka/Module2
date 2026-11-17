#include "queue.h"
#include <stdlib.h>
struct QueueNode {Message value;struct QueueNode *next;};
void queue_init(PriorityQueue *q){if(q){q->head=NULL;q->count=0;}}
size_t queue_size(const PriorityQueue *q){return q?q->count:0;}
void queue_clear(PriorityQueue *q){
    if(!q)return;
    QueueNode *node=q->head;
    while(node){QueueNode *next=node->next;free(node);node=next;}
    queue_init(q);
}
int queue_push(PriorityQueue *q,const Message *m){
    if(!q||!m)return 0;
    QueueNode *node=malloc(sizeof(*node));if(!node)return 0;
    node->value=*m;node->next=NULL;
    QueueNode **place=&q->head;
    /* Descending priority; equal priorities retain FIFO insertion order. */
    while(*place && (*place)->value.priority>=m->priority)place=&(*place)->next;
    node->next=*place;*place=node;q->count++;return 1;
}
static int pop_matching(PriorityQueue *q,unsigned threshold,int exact,Message *out){
    if(!q||!out||threshold>255)return 0;
    QueueNode **link=&q->head;
    while(*link){
        unsigned priority=(*link)->value.priority;
        if((exact && priority==threshold)||(!exact && priority>=threshold)) {
            QueueNode *victim=*link;
            *out=victim->value;*link=victim->next;
            free(victim);q->count--;return 1;
        }
        link=&(*link)->next;
    }
    return 0;
}
int queue_pop_first(PriorityQueue *q,Message *out) {
    if(!q||!q->head||!out)return 0;
    QueueNode *first=q->head;*out=first->value;
    q->head=first->next;free(first);q->count--;return 1;
}
int queue_pop_exact(PriorityQueue *q,unsigned p,Message *out){return pop_matching(q,p,1,out);}
int queue_pop_at_least(PriorityQueue *q,unsigned p,Message *out){return pop_matching(q,p,0,out);}
