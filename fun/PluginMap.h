#include "PluginTable.h"

struct plugin_entry
{
	wchar_t path[16];
	wchar_t name[8];
	wchar_t desc[42];
	int suggestion;//0 建议开启，1为建议禁止，2为可以禁止
};

struct plugin_entry *plugin_map;
int size_plugin_map = 0;

BOOL InitPlugin()
{
    UInt32 outSize = 0; 
	BYTE *outStream = 0;
	if(LZMADec(PluginTable,sizeof(PluginTable),&outStream,&outSize,0)==LZMA_RESULT_OK)
	{
        size_plugin_map = outSize / sizeof(plugin_entry);
        plugin_map = (struct plugin_entry *)outStream;
        return TRUE;
	}
	return FALSE;
}

int __cdecl IsMatchPlugin(const wchar_t * _Str1, const wchar_t * _Str2)
{
	return wcsicmp(_Str1+preLen,_Str2)==0;
}
int FindPlugin(const wchar_t *path)
{
	if ( IsMatchPlugin(path,L"Advertisement") )
	{
		return -3;
	}
	if ( IsMatchPlugin(path,L"WBlog") )
	{
		return -2;
	}

	for (int i = 0; i < size_plugin_map; i++)
	{
		if ( IsMatchPlugin(path,plugin_map[i].path) )
		{
			//DbgPrint(L"%d %s",i,path);
			return i;
		}
	}
	return -1;
}

int arraylist_append(wchar_t *c_str,bool enable,int index)
{
	if(PluginList.length>55) return 0;

	PluginList.data[PluginList.length].index = index;
	PluginList.data[PluginList.length].enable = enable;
	//c_str[0] -= 32;
	//c_str[4] -= 32;
	//c_str[12] -= 32;
	wcsncpy(PluginList.data[PluginList.length].c_str,c_str,32);
	//OutputDebugString(PluginList.data[PluginList.length].c_str);
	if(!QQShow_index)
	{
		if ( IsMatchPlugin(c_str,L"TMBasic") )
		{
			QQShow_index = PluginList.length;
		}
		if ( IsMatchPlugin(c_str,L"QQShow") )
		{
			QQShow_index = PluginList.length;
		}
	}
	
	PluginList.length++;

	return 1;
}