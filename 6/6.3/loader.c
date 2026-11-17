#define _POSIX_C_SOURCE 200809L
#include "loader.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int suffix_so(const char *name) {
    size_t n=strlen(name);return n>3 && strcmp(name+n-3,".so")==0;
}
void loader_init(PluginList *list) {if(list){list->items=NULL;list->count=0;list->capacity=0;}}
static int add(PluginList *list,void *handle,const CalcPlugin *api) {
    if(list->count==list->capacity){
        size_t cap=list->capacity?list->capacity*2:4;
        LoadedPlugin *items=realloc(list->items,cap*sizeof(*items));if(!items)return 0;
        list->items=items;list->capacity=cap;
    }
    list->items[list->count++]=(LoadedPlugin){handle,api};return 1;
}
int loader_directory(PluginList *list,const char *folder){
    if(!list||!folder)return 0;
    DIR *dir=opendir(folder);if(!dir){perror("opendir");return 0;}
    struct dirent *item;
    while((item=readdir(dir))!=NULL){
        if(!suffix_so(item->d_name))continue;
        size_t n=strlen(folder)+strlen(item->d_name)+2;
        char *path=malloc(n);if(!path){closedir(dir);return 0;}
        snprintf(path,n,"%s/%s",folder,item->d_name);
        void *handle=dlopen(path,RTLD_NOW|RTLD_LOCAL);
        if(!handle){fprintf(stderr,"Не удалось открыть %s: %s\n",path,dlerror());free(path);continue;}
        dlerror();
        PluginEntry entry=NULL;
        /* POSIX dlsym returns a symbol address; memcpy avoids pedantic warnings. */
        void *symbol=dlsym(handle,"calculator_plugin");
        const char *error=dlerror();
        if(!error)memcpy(&entry,&symbol,sizeof(entry));
        if(error||!entry){fprintf(stderr,"Пропуск %s: нет calculator_plugin\n",path);dlclose(handle);free(path);continue;}
        const CalcPlugin *api=entry();
        if(!api||!api->name||!api->symbol||!api->calculate) {
            fprintf(stderr,"Пропуск %s: некорректный дескриптор\n",path);
            dlclose(handle);free(path);continue;
        }
        int repeated=0;
        for(size_t i=0;i<list->count;i++)if(!strcmp(list->items[i].api->symbol,api->symbol))repeated=1;
        if(repeated){fprintf(stderr,"Повтор операции %s в %s\n",api->symbol,path);dlclose(handle);free(path);continue;}
        if(!add(list,handle,api)){dlclose(handle);free(path);closedir(dir);return 0;}
        printf("Загружена операция: %s (%s) из %s\n",api->name,api->symbol,path);
        free(path);
    }
    closedir(dir);return 1;
}
void loader_close(PluginList *list) {
    if(!list)return;
    for(size_t i=0;i<list->count;i++)dlclose(list->items[i].handle);
    free(list->items);loader_init(list);
}
