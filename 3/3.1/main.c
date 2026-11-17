#include "permissions.h"
#include <stdio.h>
#include <string.h>
static int read_line(const char *prompt,char *buf,size_t n) {
    printf("%s",prompt); fflush(stdout);
    if (!fgets(buf,(int)n,stdin)) return 0;
    size_t len=strlen(buf);
    if (len && buf[len-1]=='\n') { buf[len-1]=0; return 1; }
    if (feof(stdin)) return 1;
    int ch;while ((ch=getchar())!='\n'&&ch!=EOF) {}
    puts("Слишком длинная строка.");return -1;
}
static void show(mode_t mode) {
    char letters[10], bits[12]; perms_format(mode,letters,bits);
    printf("Буквенное: %s\nЦифровое: %03o\nБитовое: %s\n",letters,(unsigned)(mode&0777),bits);
}
int main(void) {
    mode_t current=0; int available=0;
    char line[1024];
    for (;;) {
        puts("\n1 — Ввести права (755 / rwxr-xr-x)");
        puts("2 — Показать права файла через stat");
        puts("3 — Изменить текущие права (например, u+x,g-w или 640)");
        puts("4 — Показать текущие права");
        puts("0 — Выход");
        int status=read_line("Выбор: ",line,sizeof(line));
        if (!status) break;
        if (status<0) continue;
        if (!strcmp(line,"0")) break;
        if (!strcmp(line,"1")) {
            status=read_line("Права: ",line,sizeof(line));if (!status) break;if(status<0)continue;
            if (perms_parse(line,&current)) {available=1;show(current);} else puts("Неверное обозначение прав.");
        } else if (!strcmp(line,"2")) {
            status=read_line("Имя или путь к файлу: ",line,sizeof(line));if (!status)break;if(status<0)continue;
            if (perms_from_file(line,&current)) {available=1;show(current);puts("Для сравнения выполните: ls -l <имя_файла>");}
            else perror("stat");
        } else if (!strcmp(line,"3")) {
            if (!available) {puts("Сначала задайте права или выберите файл.");continue;}
            status=read_line("Изменение: ",line,sizeof(line));if(!status)break;if(status<0)continue;
            if (perms_modify(&current,line)) {show(current);puts("Изменено только в памяти. Права реального файла не затронуты.");}
            else puts("Неверная команда. Примеры: u+x, g-w, a=r, u+x,g-w, 755");
        } else if (!strcmp(line,"4")) {
            if (available)show(current);else puts("Права пока не заданы.");
        } else puts("Неизвестная команда.");
    }
    return 0;
}
