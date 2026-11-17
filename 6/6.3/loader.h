#ifndef LOADER_H
#define LOADER_H
#include <stddef.h>
#include "plugin_api.h"
typedef struct {
    void *handle;
    const CalcPlugin *api;
} LoadedPlugin;
typedef struct {
    LoadedPlugin *items;
    size_t count;
    size_t capacity;
} PluginList;
void loader_init(PluginList *plugins);
int loader_directory(PluginList *plugins,const char *folder);
void loader_close(PluginList *plugins);
#endif
