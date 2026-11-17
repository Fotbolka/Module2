#include "queue.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static int input_unsigned(const char *prompt,unsigned min,unsigned max,unsigned *out){
    char line[64];
    for(;;){
        printf("%s",prompt);fflush(stdout);
        if(!fgets(line,sizeof(line),stdin))return 0;
        size_t n=strlen(line);if(!n)return 0;
        if(line[n-1]!='\n' && !feof(stdin)) {int ch;while((ch=getchar())!='\n' && ch!=EOF){}puts("Слишком длинный ввод.");continue;}
        errno=0;char *end;unsigned long x=strtoul(line,&end,10);
        while(*end==' '||*end=='\n'||*end=='\t')++end;
        if(errno==0 && end!=line && !*end && line[0]!='-' && x>=min && x<=max){*out=(unsigned)x;return 1;}
        printf("Введите целое число от %u до %u.\n",min,max);
    }
}
static void print_message(const Message *m){
    printf("ID=%lu | priority=%u | %s\n",m->id,(unsigned)m->priority,m->text);
}
int main(void){
    PriorityQueue q;queue_init(&q);
    srand((unsigned)time(NULL));unsigned long next_id=1;
    for(;;){
        puts("\n===== ОЧЕРЕДЬ С ПРИОРИТЕТОМ =====");
        printf("Элементов: %zu\n",queue_size(&q));
        puts("1 — Добавить сообщение вручную");
        puts("2 — Сгенерировать сообщения со случайным приоритетом");
        puts("3 — Извлечь первое сообщение (максимальный приоритет)");
        puts("4 — Извлечь сообщение с указанным приоритетом");
        puts("5 — Извлечь сообщение с приоритетом не ниже заданного");
        puts("0 — Выход");
        unsigned choice;if(!input_unsigned("Выбор: ",0,5,&choice))break;
        if(choice==0)break;
        if(choice==1){
            unsigned p;if(!input_unsigned("Приоритет [0..255]: ",0,255,&p))break;
            Message m={0};m.priority=(unsigned char)p;m.id=next_id++;
            printf("Текст: ");fflush(stdout);
            if(!fgets(m.text,sizeof(m.text),stdin))break;
            size_t n=strlen(m.text);if(n && m.text[n-1]=='\n')m.text[n-1]=0;
            else if(!feof(stdin)){int ch;while((ch=getchar())!='\n'&&ch!=EOF){}}
            if(!queue_push(&q,&m))puts("Недостаточно памяти.");
        } else if(choice==2){
            unsigned n;if(!input_unsigned("Сколько сообщений [1..1000]: ",1,1000,&n))break;
            for(unsigned i=0;i<n;i++){
                Message m={0};m.priority=(unsigned char)(rand()%256);m.id=next_id++;
                snprintf(m.text,sizeof(m.text),"Сгенерированное сообщение %lu",m.id);
                if(!queue_push(&q,&m)){puts("Недостаточно памяти.");break;}
                print_message(&m);
            }
        } else {
            unsigned p=0;
            if(choice!=3 && !input_unsigned("Приоритет [0..255]: ",0,255,&p))break;
            Message m={0};int ok=(choice==3)?queue_pop_first(&q,&m):(choice==4)?queue_pop_exact(&q,p,&m):queue_pop_at_least(&q,p,&m);
            if(ok){puts("Извлечено:");print_message(&m);}else puts("Подходящего сообщения в очереди нет.");
        }
    }
    queue_clear(&q);return 0;
}
