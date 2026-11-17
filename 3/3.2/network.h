#ifndef NETWORK_H
#define NETWORK_H
#include <stdint.h>
int parse_ipv4(const char *text,uint32_t *addr);
void ipv4_text(uint32_t ip,char buf[16]);
int mask_valid(uint32_t mask);
int same_subnet(uint32_t gateway,uint32_t destination,uint32_t mask);
uint32_t random_ip(void);
#endif
