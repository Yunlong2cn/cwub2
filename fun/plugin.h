#include "PluginMap.h"


// �������
typedef int (__cdecl * MYINITPLUGIN) (wchar_t *);
MYINITPLUGIN PluginOldProc = NULL;
int __cdecl MyInitPlugin(wchar_t * path)
{
    int index = FindPlugin(path);
	int state = GetPrivateProfileInt(L"�������",path, -1, tzIniPath);
    if(state==-1)
    {
        state = GetPrivateProfileInt(L"�������",path, -1, tzUACPath);
    }
    if(state==-1)
    {
        //δ��ȡ������
		if(index>=0)
		{
			state = plugin_map[index].suggestion!=1;
		}
		else
		{
			if(index>=-2) state = 1;//δ֪���+΢�����
			else state = 0;//�����
		}

        //��������
        SetPrivateProfileInt(L"�������",path, state, configPath);
    }
    if(index>=-1) arraylist_append(path, state ,index);

    if (state==0)  return 1;

    // ����ԭ���ĺ���
    if(PluginOldProc) return PluginOldProc(path);
    return 1;
}

