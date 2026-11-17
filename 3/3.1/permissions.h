#ifndef PERMISSIONS_H
#define PERMISSIONS_H
#include <stddef.h>
#include <sys/types.h>
int perms_parse(const char *s, mode_t *out);
int perms_from_file(const char *filename, mode_t *out);
int perms_modify(mode_t *perms,const char *command);
void perms_format(mode_t perms,char letters[10],char bits[12]);
#endif
