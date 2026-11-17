#include "network.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(int argc,char **argv) {
    if (argc!=4) {
        fprintf(stderr,"Использование: %s <шлюз> <маска> <число_пакетов>\nПример: %s 192.168.1.1 255.255.255.0 100\n",argv[0],argv[0]);
        return 1;
    }
    uint32_t gateway,mask;
    if(!parse_ipv4(argv[1],&gateway)||!parse_ipv4(argv[2],&mask)||!mask_valid(mask)) {
        fputs("Ошибка: неверный IPv4-адрес или маска (маска должна быть непрерывной).\n",stderr);return 1;
    }
    errno=0; char *end; unsigned long count=strtoul(argv[3],&end,10);
    if(errno==ERANGE || end==argv[3] || *end || argv[3][0]=='-' || count<1 || count>1000000UL) {
        fputs("Число пакетов должно быть от 1 до 1000000.\n",stderr);return 1;
    }
    srand((unsigned)time(NULL));
    unsigned long local=0,other=0;
    for(unsigned long i=0;i<count;i++) {
        uint32_t destination=random_ip();char text[16];ipv4_text(destination,text);
        int inside=same_subnet(gateway,destination,mask);
        if(inside) ++local;else ++other;
        printf("Пакет %lu: %s → %s\n",i+1,text,inside?"своя подсеть":"другая сеть");
    }
    printf("\nВсего: %lu\nСвоя подсеть: %lu (%.2f%%)\nДругие сети: %lu (%.2f%%)\n",count,local,100.0*local/count,other,100.0*other/count);
    return 0;
}
