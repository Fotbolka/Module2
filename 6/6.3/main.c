#include "loader.h"
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int input(const char *prompt,char *buf,size_t n){
    printf("%s",prompt);fflush(stdout);
    if(!fgets(buf,(int)n,stdin))return 0;
    size_t len=strlen(buf);
    if(len && buf[len-1]=='\n')buf[len-1]=0;
    else if(!feof(stdin)) {int ch;while((ch=getchar())!='\n'&&ch!=EOF){}return -1;}
    return 1;
}
static int number(const char *prompt,double *out){
    char s[128];
    for(;;){int st=input(prompt,s,sizeof(s));if(!st)return 0;if(st<0){puts("Слишком длинный ввод.");continue;}
        errno=0;char *end;double x=strtod(s,&end);
        while(isspace((unsigned char)*end))end++;
        if(errno==0 && end!=s && !*end && isfinite(x)){*out=x;return 1;}
        puts("Введите конечное число (десятичная точка).");
    }
}
static int choice(size_t max,int *out){
    char s[128];
    for(;;){int st=input("Выбор: ",s,sizeof(s));if(!st)return 0;if(st<0)continue;
        errno=0;char *end;long x=strtol(s,&end,10);
        while(isspace((unsigned char)*end))end++;
        if(errno==0 && end!=s && !*end && x>=0 && (unsigned long)x<=max){*out=(int)x;return 1;}
        puts("Некорректный номер операции.");
    }
}
int main(int argc,char **argv){
    const char *folder=(argc==2)?argv[1]:"./plugins";
    if(argc>2){fprintf(stderr,"Использование: %s [папка_с_плагинами]\n",argv[0]);return 1;}
    PluginList plugins;loader_init(&plugins);
    if(!loader_directory(&plugins,folder)){loader_close(&plugins);return 1;}
    if(!plugins.count){puts("В каталоге нет пригодных плагинов .so.");loader_close(&plugins);return 1;}
    for(;;){
        puts("\n===== КАЛЬКУЛЯТОР С ПЛАГИНАМИ =====");
        for(size_t i=0;i<plugins.count;i++)printf("%zu — %s (%s)\n",i+1,plugins.items[i].api->name,plugins.items[i].api->symbol);
        puts("0 — Выход");
        int selected;if(!choice(plugins.count,&selected)||!selected)break;
        double a,b,result;
        if(!number("Первое число: ",&a)||!number("Второе число: ",&b))break;
        CalcPlugin const *api=plugins.items[(size_t)selected-1].api;
        if(!api->calculate(a,b,&result)||!isfinite(result))puts("Операция невозможна (деление на ноль или переполнение).");
        else printf("Результат: %.10g\n",result);
    }
    loader_close(&plugins);return 0;
}
