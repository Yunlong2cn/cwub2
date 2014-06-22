#include "crack.h"
void MemoryClear()
{
    if (memc)
    {
        SetProcessWorkingSetSize(GetCurrentProcess(), (DWORD) - 1, (DWORD) - 1);
    }

	oneminits = 1;
	if(WryOK==FALSE) InitCWUBIP();
	if(WryOK2==FALSE) InitCWUBLan();
}
void FixHook()
{
	/*
	if(oneminits==0)
	{
		CallWindowProc((WNDPROC)0x6601B2D0,(HWND)0x0026059A,WM_HOTKEY,0x0000C055,0x00410003);
	}
	*/
	//DbgPrint(L"成功注入Timer");
    for(int i=0;i<HookCount;i++)
    {
        if(*(BYTE*)TagHook[i].orig!=0x68)
        {
            //InstallHook
			WritePUSH_RET(TagHook[i].orig,TagHook[i].det);
        }
    }
}
void MyThread(PVOID pvoid)
{
    SetTimer(NULL, 0, 1000*60, (TIMERPROC)MemoryClear);//60秒一次
    SetTimer(NULL, 0, 1000*30, (TIMERPROC)FixHook);//30秒一次
    
    CoInitializeEx(0,COINIT_MULTITHREADED);
    CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);

	AppBox =  GetPrivateProfileInt(L"其它功能", L"应用盒子", 1, tzIniPath);
	Banber =  GetPrivateProfileInt(L"其它功能", L"节日LOGO", 1, tzIniPath);
	Skins =   GetPrivateProfileInt(L"其它功能", L"皮肤提醒", 1, tzIniPath);
	Savelog = GetPrivateProfileInt(L"显示位置", L"写入记录", 0, tzIniPath);


	ShowHideicon =  GetPrivateProfileInt(L"图标管理", L"隐藏图标", 0, tzIniPath);
	//ShowSelficon =  GetPrivateProfileInt(L"图标管理", L"图标全亮", 0, tzIniPath);

    oneminits = 0;

    voice_contact = GetPrivateProfileInt(L"语音朗读",L"朗读好友消息", 0, configPath);
    voice_self =    GetPrivateProfileInt(L"语音朗读",L"朗读自己消息", 0, configPath);
    voice_group =   GetPrivateProfileInt(L"语音朗读",L"朗读群消息", 0, configPath);
    voice_wnd =     GetPrivateProfileInt(L"语音朗读",L"窗口确认", 1, configPath);
    
    wndf = GetPrivateProfileInt(L"其它功能",L"屏蔽弹窗", 1, configPath);
    lvip = GetPrivateProfileInt(L"其它功能",L"本地会员", 1, configPath);
    memc = GetPrivateProfileInt(L"其它功能",L"内存整理", 0, configPath);
    
    
    g_hwnd_inner = GetPrivateProfileInt(L"显示位置",L"框内显示", 1, configPath);
    g_hwnd_left = GetPrivateProfileInt(L"显示位置",L"左下角显示", 1, configPath);
    g_hwnd_top = GetPrivateProfileInt(L"显示位置",L"右上角显示", 0, configPath);

	//OutputDebugStringA("线程1开始");
	g_hFont_xp = MyCreateFont(L"SimSun");
	g_hFont_w7 = MyCreateFont(L"Microsoft Yahei");
	g_hFont = IsVistaTheme()?g_hFont_w7:g_hFont_xp;
		
	if(GetPrivateProfileString(L"其它功能", _T("老板键"), L"", g_szHotkey, 255, tzIniPath))
	{
		CreateDialog(g_hModule,MAKEINTRESOURCE(101),NULL,BossKey);
	}

	InitCWUBIP();
	InitCWUBLan();



    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
    {
		if (msg.message == WM_THEMECHANGED)
		{
			hButtonTheme = (HANDLE)-1;
			g_hFont = IsVistaTheme()?g_hFont_w7:g_hFont_xp;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}