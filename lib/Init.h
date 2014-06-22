
// 全局变量
wchar_t tzPath[MAX_PATH];//文件路径
wchar_t tzDicPath[MAX_PATH];//文件路径
wchar_t tzBakPath[MAX_PATH];//备份文件路径
wchar_t tzUACPath[MAX_PATH];//UAC文件路径
wchar_t tzIniPath[MAX_PATH];//配置文件路径
wchar_t tzCachePath[MAX_PATH];//缓存文件路径
wchar_t tzQQWryPath[MAX_PATH];//查询文件路径
wchar_t tzQQWry[MAX_PATH];//QQWry.dat文件路径
wchar_t tzQQWry2[MAX_PATH];//QQWry.dat文件路径
wchar_t g_szHotkey[256];

wchar_t * configPath;

//char tzUpdate1[MAX_PATH];//文件路径
//char tzUpdate2[MAX_PATH];//文件路径

HookLib WS2_32;
//HookLib Kernel32;
//HookLib Shell32;

//CRITICAL_SECTION g_yd;
//CRITICAL_SECTION g_ip;

//extern bool isEndWith(const wchar_t *path,const wchar_t* ext);


void WriteUnicode(wchar_t *lpFileName)
{
    FILE *fp;
    fp = _wfopen(lpFileName, _T("rb"));
    if (fp == NULL)
    {
        fp = _wfopen(lpFileName, _T("wb"));
		if (fp != NULL)
		{
			fputwc(0xFEFF,fp);
			fputws(L";本文件由幽香2自动生成，可安全删除",fp);
		}
    }
	fclose(fp);
}

BOOL IsUserAdmin()
{
	static int result = 2;
	if(result!=2) return result;

	result = 0;
	if(WritePrivateProfileString(L"其它功能", L"TEST",NULL, tzIniPath) && WritePrivateProfileString(L"其它功能", L"TEST", L"TEST", tzIniPath))
	{
		WritePrivateProfileString(L"其它功能", L"TEST",NULL, tzIniPath);
		DeleteFile(tzUACPath);
		WriteUnicode(tzUACPath);
		result = 1;
	}
	return result;
}

wchar_t * GetConfig()
{
	return IsUserAdmin()?tzIniPath:tzUACPath;
}

void mrdir()
{
    //创建缓存目录
    TCHAR Temp[MAX_PATH];
    SHGetSpecialFolderPath(NULL, Temp, CSIDL_APPDATA, FALSE);
    wcscat(Temp,L"\\CWUB2");

    DWORD dwAttributes = GetFileAttributes(Temp);
    if(dwAttributes==0xFFFFFFFF)
    {
        CreateDirectory(Temp,NULL);
    }
    else
    {
        //
        if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
        {
            DeleteFile(Temp);
            CreateDirectory(Temp,NULL);
        }
    }
    
    WriteUnicode(tzCachePath);
    WriteUnicode(tzQQWryPath);
    WriteUnicode(tzUACPath);
    
    //测试是否可写
    if(WritePrivateProfileString(L"WAN", L"TEST", L"TEST", tzCachePath) && WritePrivateProfileString(L"IP", L"TEST", L"TEST", tzQQWryPath))
    {
        WritePrivateProfileString(L"WAN", L"TEST",NULL, tzCachePath);
        WritePrivateProfileString(L"IP", L"TEST",NULL, tzQQWryPath);
    }
    else
    {
        MessageBox(0,L"警告，缓存目录 %AppData%\\CWUB2 不可写！",L"幽香2",MB_OK + MB_ICONERROR);
    }
}

long MemorySearch(const wchar_t *path,BYTE* key, int length);
bool InitCWUB2()
{
    //获得配置文件目录
    GetModuleFileName(g_hModule, tzDicPath, sizeof(tzDicPath) / sizeof(TCHAR));
    wcslwr(tzDicPath);
    wcscpy(tzPath,tzDicPath);

	/*
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	if(SystemTime.wYear!=2011 || SystemTime.wMonth!=11 )
	{
		//MessageBox(0,L"您现在正在使用幽香2测试版，感谢您的测试。但测试已经到期，为了避免您遭到损失，本软件已经停用。",L"幽香2",0);
		return false;
	}
	*/
	//保持单实例
	wsprintf(tzIniPath, (_T("CWUB2.MUTEX.%X")), GetCurrentProcessId());
	::CreateMutex(NULL, TRUE, tzIniPath);
	if(GetLastError()   ==   ERROR_ALREADY_EXISTS)
	{
		return false;
	}

    wcscpy(tzBakPath,tzPath);
    wcscat(tzBakPath,L".bak");
    DeleteFile(tzBakPath);
	
    
	/*
    strcpy(tzUpdate1,"http://shuax.googlecode.com/svn/trunk/cwub2/");
    strcpy(tzUpdate2,"http://shuax.googlecode.com/svn/trunk/cwub2/");
    
    strcat(tzUpdate1,"msimg32.dll");
    strcat(tzUpdate2,"update.txt");
    */
    *(wcsrchr(tzDicPath, '\\')) = 0;

    wcscpy(tzIniPath,tzDicPath);
    wcscat(tzIniPath, L"\\CWUB2.ini");

    wcscpy(tzQQWry,tzDicPath);
    wcscat(tzQQWry, L"\\CWUB2.ip");
    
	wcscpy(tzQQWry2,tzDicPath);
	wcscat(tzQQWry2, L"\\CWUB2.LAN");
    

    //获得缓存文件目录
    SHGetSpecialFolderPath(NULL, tzCachePath, CSIDL_APPDATA, FALSE);
    wcscat(tzCachePath,L"\\CWUB2");
    
    wcscpy(tzQQWryPath,tzCachePath);
    wcscat(tzQQWryPath,L"\\Online.ini");
    
    wcscpy(tzUACPath,tzCachePath);
    wcscat(tzUACPath,L"\\UAC.ini");
    
    wcscat(tzCachePath,L"\\Cache.ini");
    mrdir();
    
	configPath = GetConfig();

	preLen = wcslen(L"Com.Tencent.");
    PluginListNew();
    WS2_32.Load(L"WS2_32.dll");
	//Shell32.Load(L"Shell32.dll");
	//Kernel32.Load(L"Kernel32.dll");

	InitializeCriticalSection(&lock_conv);
	//InitializeCriticalSection(&g_yd);
	//InitializeCriticalSection(&g_ip);


	

	ShowHelpBtn = GetPrivateProfileInt(L"图标管理", L"帮助按钮", 1, tzIniPath);
	ShowFaceBtn = GetPrivateProfileInt(L"图标管理", L"魔法表情", 1, tzIniPath);
	ShowSafeEntry = GetPrivateProfileInt(L"图标管理", L"安全入口", 1, tzIniPath);
	ShowLevelBtn = GetPrivateProfileInt(L"图标管理", L"等级按钮", 1, tzIniPath);
	ShowUpdateEntry = GetPrivateProfileInt(L"图标管理", L"升级入口", 1, tzIniPath);
	ShowAddBtn = GetPrivateProfileInt(L"图标管理", L"精简图标", 0, tzIniPath);

	IsForceImageView = GetPrivateProfileInt(L"其它功能", L"内置看图", 1, tzIniPath);


    
    return true;
}