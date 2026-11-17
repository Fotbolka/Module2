#include "contacts.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
struct ListNode {
    Contact value;
    struct ListNode *prev,*next;
};
static int filled(const char *s) {
    if (!s) return 0;
    for (;*s;s++) if (!isspace((unsigned char)*s)) return 1;
    return 0;
}
static int valid(const Contact *c) {
    return c && filled(c->last_name)&&filled(c->first_name) && c->phone_count<=MAX_VALUES
        && c->email_count<=MAX_VALUES && c->social_count<=MAX_VALUES && c->messenger_count<=MAX_VALUES;
}
static int compare(const PhoneBook *b,const Contact *a,const Contact *c) {
    int r;
    if (b->sort_field == SORT_FIRST_NAME) {
        r = strcmp(a->first_name,c->first_name);
        if (!r) r = strcmp(a->last_name,c->last_name);
    } else if (b->sort_field == SORT_WORKPLACE) {
        r = strcmp(a->workplace,c->workplace);
        if (!r) r = strcmp(a->last_name,c->last_name);
        if (!r) r = strcmp(a->first_name,c->first_name);
    } else {
        r = strcmp(a->last_name,c->last_name);
        if (!r) r = strcmp(a->first_name,c->first_name);
    }
    if (!r) r = strcmp(a->middle_name,c->middle_name);
    return b->sort_order == SORT_DESCENDING ? (r > 0 ? -1 : (r < 0 ? 1 : 0)) : r;
}
void phonebook_init(PhoneBook *b) {
    if (b) {
        b->head=NULL;b->tail=NULL;b->count=0;
        b->sort_field=SORT_LAST_NAME;b->sort_order=SORT_ASCENDING;
    }
}
size_t phonebook_count(const PhoneBook *b) {return b?b->count:0;}
static ListNode *node_at(const PhoneBook *b,size_t pos) {
    if (!b||pos>=b->count)return NULL;
    ListNode *node;
    if(pos<b->count/2) {node=b->head;for(size_t i=0;i<pos;i++) node=node->next;}
    else {node=b->tail;for(size_t i=b->count-1;i>pos;i--) node=node->prev;}
    return node;
}
static void detach(PhoneBook *b,ListNode *n) {
    if(n->prev) n->prev->next=n->next; else b->head=n->next;
    if(n->next) n->next->prev=n->prev; else b->tail=n->prev;
    n->prev=NULL;n->next=NULL;b->count--;
}
static void insert_sorted(PhoneBook *b,ListNode *n) {
    ListNode *current=b->head;
    while(current && compare(b,&current->value,&n->value)<=0)current=current->next;
    if (!current) {
        n->prev=b->tail;n->next=NULL;
        if(b->tail)b->tail->next=n;else b->head=n;
        b->tail=n;
    } else {
        n->next=current;n->prev=current->prev;
        if(current->prev)current->prev->next=n;else b->head=n;
        current->prev=n;
    }
    b->count++;
}
int phonebook_add(PhoneBook *b,const Contact *c) {
    if (!b||!valid(c))return 0;
    ListNode *n=malloc(sizeof(*n)); if(!n)return 0;
    n->value=*c;n->prev=n->next=NULL;insert_sorted(b,n);return 1;
}
const Contact *phonebook_get(const PhoneBook *b,size_t pos) {
    ListNode *node=node_at(b,pos);return node?&node->value:NULL;
}
int phonebook_update(PhoneBook *b,size_t pos,const Contact *c) {
    if(!b||!valid(c))return 0;
    ListNode *node=node_at(b,pos);if(!node)return 0;
    Contact copy=*c; /* Copy before detaching in case c refers to existing node. */
    detach(b,node);node->value=copy;insert_sorted(b,node);return 1;
}
int phonebook_delete(PhoneBook *b,size_t pos) {
    ListNode *node=node_at(b,pos);if(!node)return 0;
    detach(b,node);free(node);return 1;
}
void phonebook_free(PhoneBook *b) {
    if(!b)return;
    ListNode *n=b->head;
    while(n){ListNode *next=n->next;free(n);n=next;}
    phonebook_init(b);
}

/* При смене поля сортировки перестраиваем связи узлов без новых выделений памяти. */
int phonebook_sort(PhoneBook *b, SortField field, SortOrder order) {
    if (!b || field < SORT_LAST_NAME || field > SORT_WORKPLACE
        || order < SORT_ASCENDING || order > SORT_DESCENDING) return 0;
    ListNode *old = b->head;
    b->head = b->tail = NULL;
    b->count = 0;
    b->sort_field = field;
    b->sort_order = order;
    while (old) {
        ListNode *next = old->next;
        old->next = old->prev = NULL;
        insert_sorted(b, old);
        old = next;
    }
    return 1;
}
