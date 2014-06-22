
// ȫ�ֱ���
wchar_t tzPath[MAX_PATH];//�ļ�·��
wchar_t tzDicPath[MAX_PATH];//�ļ�·��
wchar_t tzBakPath[MAX_PATH];//�����ļ�·��
wchar_t tzUACPath[MAX_PATH];//UAC�ļ�·��
wchar_t tzIniPath[MAX_PATH];//�����ļ�·��
wchar_t tzCachePath[MAX_PATH];//�����ļ�·��
wchar_t tzQQWryPath[MAX_PATH];//��ѯ�ļ�·��
wchar_t tzQQWry[MAX_PATH];//QQWry.dat�ļ�·��
wchar_t tzQQWry2[MAX_PATH];//QQWry.dat�ļ�·��
wchar_t g_szHotkey[256];

wchar_t * configPath;

//char tzUpdate1[MAX_PATH];//�ļ�·��
//char tzUpdate2[MAX_PATH];//�ļ�·��

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
			fputws(L";���ļ�������2�Զ����ɣ��ɰ�ȫɾ��",fp);
		}
    }
	fclose(fp);
}

BOOL IsUserAdmin()
{
	static int result = 2;
	if(result!=2) return result;

	result = 0;
	if(WritePrivateProfileString(L"��������", L"TEST",NULL, tzIniPath) && WritePrivateProfileString(L"��������", L"TEST", L"TEST", tzIniPath))
	{
		WritePrivateProfileString(L"��������", L"TEST",NULL, tzIniPath);
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
    //��������Ŀ¼
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
    
    //�����Ƿ��д
    if(WritePrivateProfileString(L"WAN", L"TEST", L"TEST", tzCachePath) && WritePrivateProfileString(L"IP", L"TEST", L"TEST", tzQQWryPath))
    {
        WritePrivateProfileString(L"WAN", L"TEST",NULL, tzCachePath);
        WritePrivateProfileString(L"IP", L"TEST",NULL, tzQQWryPath);
    }
    else
    {
        MessageBox(0,L"���棬����Ŀ¼ %AppData%\\CWUB2 ����д��",L"����2",MB_OK + MB_ICONERROR);
    }
}

long MemorySearch(const wchar_t *path,BYTE* key, int length);
bool InitCWUB2()
{
    //��������ļ�Ŀ¼
    GetModuleFileName(g_hModule, tzDicPath, sizeof(tzDicPath) / sizeof(TCHAR));
    wcslwr(tzDicPath);
    wcscpy(tzPath,tzDicPath);

	/*
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	if(SystemTime.wYear!=2011 || SystemTime.wMonth!=11 )
	{
		//MessageBox(0,L"����������ʹ������2���԰棬��л���Ĳ��ԡ��������Ѿ����ڣ�Ϊ�˱������⵽��ʧ��������Ѿ�ͣ�á�",L"����2",0);
		return false;
	}
	*/
	//���ֵ�ʵ��
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
    

    //��û����ļ�Ŀ¼
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


	

	ShowHelpBtn = GetPrivateProfileInt(L"ͼ�����", L"������ť", 1, tzIniPath);
	ShowFaceBtn = GetPrivateProfileInt(L"ͼ�����", L"ħ������", 1, tzIniPath);
	ShowSafeEntry = GetPrivateProfileInt(L"ͼ�����", L"��ȫ���", 1, tzIniPath);
	ShowLevelBtn = GetPrivateProfileInt(L"ͼ�����", L"�ȼ���ť", 1, tzIniPath);
	ShowUpdateEntry = GetPrivateProfileInt(L"ͼ�����", L"�������", 1, tzIniPath);
	ShowAddBtn = GetPrivateProfileInt(L"ͼ�����", L"����ͼ��", 0, tzIniPath);

	IsForceImageView = GetPrivateProfileInt(L"��������", L"���ÿ�ͼ", 1, tzIniPath);


    
    return true;
}