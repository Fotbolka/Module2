#include "permissions.h"
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
/* Только девять основных rwx-битов; специальные suid/sgid/sticky в условии не оговорены. */
static const mode_t flags[9]={0400,0200,0100,0040,0020,0010,0004,0002,0001};
static const char symbols[]="rwxrwxrwx";
int perms_parse(const char *s,mode_t *out) {
    if (!s || !out) return 0;
    size_t n=strlen(s); mode_t m=0;
    if (n==4 && s[0]=='0') {++s;--n;}
    if (n==3 && s[0]>='0' && s[0]<='7') {
        for (size_t i=0;i<3;i++) { if (s[i]<'0'||s[i]>'7') return 0; m=(mode_t)((m<<3)|(s[i]-'0')); }
        *out=m; return 1;
    }
    if (n==10 && strchr("-dlcbps",s[0])) {++s;--n;}
    if (n!=9) return 0;
    for (size_t i=0;i<9;i++) {
        if (s[i]==symbols[i]) m|=flags[i];
        else if (s[i]!='-') return 0;
    }
    *out=m; return 1;
}
int perms_from_file(const char *filename,mode_t *out) {
    struct stat st;
    if (!filename||!out||stat(filename,&st)!=0) return 0;
    *out=st.st_mode & 0777; return 1;
}
void perms_format(mode_t m,char letters[10],char bits[12]) {
    size_t j=0;
    for (size_t i=0;i<9;i++) {
        letters[i]=(m&flags[i]) ? symbols[i]:'-';
        bits[j++]=(m&flags[i])?'1':'0';
        if (i==2 || i==5) bits[j++]=' ';
    }
    letters[9]=0; bits[j]=0;
}
/* Поддерживаются u/g/o/a [+/-/=] [rwx], а также разделённые запятыми команды. */
int perms_modify(mode_t *perms,const char *command) {
    if (!perms||!command||!*command) return 0;
    mode_t tmp=*perms & 0777;
    if (perms_parse(command,&tmp)) { *perms=tmp; return 1; }
    const char *p=command;
    while (*p) {
        int classes=0;
        while (*p=='u'||*p=='g'||*p=='o'||*p=='a') {
            classes|=(*p=='u')?4:(*p=='g')?2:(*p=='o')?1:7; ++p;
        }
        if (classes==0) classes=7; /* Упрощение: без учёта umask. */
        char op=*p++;
        if (op!='+'&&op!='-'&&op!='=') return 0;
        int rwx=0;
        while (*p=='r'||*p=='w'||*p=='x') {
            rwx|=(*p=='r')?4:(*p=='w')?2:1; ++p;
        }
        mode_t mask=0,selected=0;
        if (classes&4) {mask|=0700; selected|=(mode_t)rwx<<6;}
        if (classes&2) {mask|=0070; selected|=(mode_t)rwx<<3;}
        if (classes&1) {mask|=0007; selected|=(mode_t)rwx;}
        if (op=='+') tmp|=selected;
        else if (op=='-') tmp&=~selected;
        else tmp=(tmp&~mask)|selected;
        if (*p==',') {++p; if (!*p) return 0;}
        else if (*p) return 0;
    }
    *perms=tmp; return 1;
}
