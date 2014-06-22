#pragma once

#include <stdlib.h>
#include <string.h>

struct Plugin
{
    int index;
    bool enable;
	wchar_t c_str[32];
};


struct Arry{
    Plugin data[60];
    int length;
}PluginList;


void PluginListNew()
{
    //PluginList.data = (Plugin *) malloc(sizeof(Plugin)*60);
    PluginList.length = 0;
}