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
	//DbgPrint(L"�ɹ�ע��Timer");
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
    SetTimer(NULL, 0, 1000*60, (TIMERPROC)MemoryClear);//60��һ��
    SetTimer(NULL, 0, 1000*30, (TIMERPROC)FixHook);//30��һ��
    
    CoInitializeEx(0,COINIT_MULTITHREADED);
    CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);

	AppBox =  GetPrivateProfileInt(L"��������", L"Ӧ�ú���", 1, tzIniPath);
	Banber =  GetPrivateProfileInt(L"��������", L"����LOGO", 1, tzIniPath);
	Skins =   GetPrivateProfileInt(L"��������", L"Ƥ������", 1, tzIniPath);
	Savelog = GetPrivateProfileInt(L"��ʾλ��", L"д���¼", 0, tzIniPath);


	ShowHideicon =  GetPrivateProfileInt(L"ͼ�����", L"����ͼ��", 0, tzIniPath);
	//ShowSelficon =  GetPrivateProfileInt(L"ͼ�����", L"ͼ��ȫ��", 0, tzIniPath);

    oneminits = 0;

    voice_contact = GetPrivateProfileInt(L"�����ʶ�",L"�ʶ�������Ϣ", 0, configPath);
    voice_self =    GetPrivateProfileInt(L"�����ʶ�",L"�ʶ��Լ���Ϣ", 0, configPath);
    voice_group =   GetPrivateProfileInt(L"�����ʶ�",L"�ʶ�Ⱥ��Ϣ", 0, configPath);
    voice_wnd =     GetPrivateProfileInt(L"�����ʶ�",L"����ȷ��", 1, configPath);
    
    wndf = GetPrivateProfileInt(L"��������",L"���ε���", 1, configPath);
    lvip = GetPrivateProfileInt(L"��������",L"���ػ�Ա", 1, configPath);
    memc = GetPrivateProfileInt(L"��������",L"�ڴ�����", 0, configPath);
    
    
    g_hwnd_inner = GetPrivateProfileInt(L"��ʾλ��",L"������ʾ", 1, configPath);
    g_hwnd_left = GetPrivateProfileInt(L"��ʾλ��",L"���½���ʾ", 1, configPath);
    g_hwnd_top = GetPrivateProfileInt(L"��ʾλ��",L"���Ͻ���ʾ", 0, configPath);

	//OutputDebugStringA("�߳�1��ʼ");
	g_hFont_xp = MyCreateFont(L"SimSun");
	g_hFont_w7 = MyCreateFont(L"Microsoft Yahei");
	g_hFont = IsVistaTheme()?g_hFont_w7:g_hFont_xp;
		
	if(GetPrivateProfileString(L"��������", _T("�ϰ��"), L"", g_szHotkey, 255, tzIniPath))
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