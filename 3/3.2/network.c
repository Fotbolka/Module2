#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int parse_ipv4(const char *text,uint32_t *addr) {
    if (!text || !addr) return 0;
    uint32_t ip=0; const char *p=text;
    for (int i=0;i<4;i++) {
        if (*p<'0'||*p>'9') return 0;
        unsigned part=0, digits=0;
        while (*p>='0'&&*p<='9') {
            part=part*10+(unsigned)(*p-'0'); if(part>255 || ++digits>3) return 0; ++p;
        }
        ip=(ip<<8)|part;
        if(i!=3) {if(*p++!='.') return 0;} else if(*p) return 0;
    }
    *addr=ip; return 1;
}
void ipv4_text(uint32_t ip,char buf[16]) {
    snprintf(buf,16,"%u.%u.%u.%u",(unsigned)(ip>>24),(unsigned)((ip>>16)&255),(unsigned)((ip>>8)&255),(unsigned)(ip&255));
}
int mask_valid(uint32_t mask) { uint32_t inverted=~mask; return (inverted & (inverted+1u))==0u; }
int same_subnet(uint32_t gateway,uint32_t dest,uint32_t mask) { return (gateway&mask)==(dest&mask); }
uint32_t random_ip(void) {uint32_t r=0;for(int i=0;i<4;i++) r=(r<<8)|(uint32_t)(rand()&255);return r;}
