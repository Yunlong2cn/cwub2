#include "PluginMap.h"


// 插件过滤
typedef int (__cdecl * MYINITPLUGIN) (wchar_t *);
MYINITPLUGIN PluginOldProc = NULL;
int __cdecl MyInitPlugin(wchar_t * path)
{
    int index = FindPlugin(path);
	int state = GetPrivateProfileInt(L"插件管理",path, -1, tzIniPath);
    if(state==-1)
    {
        state = GetPrivateProfileInt(L"插件管理",path, -1, tzUACPath);
    }
    if(state==-1)
    {
        //未读取到数据
		if(index>=0)
		{
			state = plugin_map[index].suggestion!=1;
		}
		else
		{
			if(index>=-2) state = 1;//未知插件+微博插件
			else state = 0;//广告插件
		}

        //保存配置
        SetPrivateProfileInt(L"插件管理",path, state, configPath);
    }
    if(index>=-1) arraylist_append(path, state ,index);

    if (state==0)  return 1;

    // 调用原来的函数
    if(PluginOldProc) return PluginOldProc(path);
    return 1;
}

