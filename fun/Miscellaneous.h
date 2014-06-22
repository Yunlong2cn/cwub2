

typedef int (__cdecl * QQPLUGIN) (wchar_t *);
QQPLUGIN EnablePlugin = NULL;
QQPLUGIN DisablePlugin = NULL;
int __cdecl MyEnablePlugin(wchar_t *path)
{
	if(EnablePlugin) return EnablePlugin(path);
	return 0;
}
int __cdecl MyDisablePlugin(wchar_t *path)
{
	if(DisablePlugin) return DisablePlugin(path);
	return 0;
}


int EnableCAM;
int __cdecl MyCheckCameraStatus()
{
    return EnableCAM;
}

void __cdecl TXLog_DoTXLogVW(void)
{
    return;
}
/*
unsigned short Swap16(unsigned short n)
{
	return (n<<8) | (n>>8);//((n&0x00FF)<<8) | ((n&0xFF00)>>8);
}
void ChangeVersion(PVOID pvoid)
{
    int offset = (int)pvoid;
    unsigned short test = *((unsigned short*)offset);
    if(test==0x1589)
    {
		wchar_t buff[256];
		unsigned short myVersion = 0;
		GetPrivateProfileString(L"其它功能", _T("版本号伪装"), _T("0"), buff, 255, tzIniPath);
		_stscanf(buff,_T("%X"),&myVersion);
		if(myVersion==0) return;
		
        int version = *((int*)(offset + 2));
        while(*((unsigned short*)version)==0)
        {
            Sleep(100);
        }
        *((unsigned short*)version) = Swap16(myVersion);
    }
}
*/

typedef LONG NTSTATUS, *PNTSTATUS;
#ifndef NT_SUCCESS
#define NT_SUCCESS(x) ((x)>=0)
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif

typedef NTSTATUS (WINAPI* PFN_LdrLoadDll)(PWCHAR PathToFile OPTIONAL, ULONG Flags OPTIONAL, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle);
PFN_LdrLoadDll s_pfnLdrLoadDll = NULL;//跳转地址
DWORD LdrLoadDll;//原始地址
NTSTATUS WINAPI MyLdrLoadDll(PWCHAR PathToFile OPTIONAL, ULONG Flags OPTIONAL, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle)
{
    if(GetModuleHandle(ModuleFileName->Buffer) == 0)
    {
		//DbgPrint(L"MyLdrLoadDll:%s",ModuleFileName->Buffer);
        if(wcsstr(ModuleFileName->Buffer,L"SafeBase")!=0)
        {
            return -1;
        }
        if(wcsstr(ModuleFileName->Buffer,L"SSOPlatform.dll")!=0)
        {
            NTSTATUS res = 0;
            if(s_pfnLdrLoadDll) res = s_pfnLdrLoadDll(PathToFile, Flags, ModuleFileName, ModuleHandle);
            else return res;
            
            if(NT_SUCCESS(res))
            {
                InstallHook((DWORD)GetProcAddress(GetModuleHandle(L"SSOCommon.dll"), "?TXLog_DoTXLogVW@@YAXPAUtagLogObj@@PB_W1PAD@Z"), (DWORD)TXLog_DoTXLogVW);
                InstallHook((DWORD)GetProcAddress(GetModuleHandle(L"SSOCommon.dll"), "?GetLogByFilter@TXLog@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@ABVCTXStringW@@K0K@Z"), (DWORD)TXLog_DoTXLogVW);
            }
            return res;
        }
    }

    if(s_pfnLdrLoadDll) return s_pfnLdrLoadDll(PathToFile, Flags, ModuleFileName, ModuleHandle);
    return 0;
}

// 无限发图
int __cdecl CheckMsgImage(DWORD p1, DWORD p2)
{
    return 1;
}


typedef int (__cdecl* QQIsFileExist)(wchar_t const *);
QQIsFileExist MyIsFileExist = NULL;//跳转地址
int __cdecl IsFileExist(wchar_t const *path)
{
	//DbgPrint(path);
    if(isEndWith(path,L"skin.ini"))
	{
		//DbgPrint(L"%s",path);
		return 1;
	}
    if(MyIsFileExist) return MyIsFileExist(path);
    
    return 0;
}

int __cdecl IsTencentTrusted(void)
{
    return 1;
}


typedef long (__stdcall* MYCreateObjectFromDllFile)(wchar_t const *,struct _GUID const &,struct _GUID const &,void * *,struct IUnknown *);
MYCreateObjectFromDllFile cofdf = NULL;//跳转地址
long __stdcall CreateObjectFromDllFile(wchar_t const * path,struct _GUID const & p1,struct _GUID const & p2,void * * p3,struct IUnknown * p4)
{
	//DbgPrint(L"%s",path);
    static wchar_t sso[] = L"SSOPlatform.dll";
    if(wcsstr(path,sso)!=0 )
    {
        wchar_t Temp[256];
        wsprintf(Temp,L"%s\\%s",tzDicPath,sso);
        //OutputDebugString(Temp);
        DWORD dwAttributes = GetFileAttributes(Temp);
        if(dwAttributes!=0xFFFFFFFF)
        {
            if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
            {
                //OutputDebugString(L"OK");
                if(cofdf) return cofdf(sso,p1,p2,p3,p4);
            }
        }
    }
    if(cofdf) return cofdf(path,p1,p2,p3,p4);
    return 0;
}


/*
typedef int (__cdecl* MYGetTXDataStr) (struct ITXDataRead *,wchar_t const *,class CTXBuffer &);
MYGetTXDataStr QQGetTXDataStr = NULL;//跳转地址
int __cdecl GetTXDataStr(struct ITXDataRead *p1,wchar_t const *p2,class CTXBuffer &p3)
{
	DbgPrint(L"%s",p2);
	if(QQGetTXDataStr) return QQGetTXDataStr(p1,p2,p3);
	return 0;
}
*/

void Miscellaneous()
{
	HookLib Common;
	Common.Load(L"Common.dll");

    int CameraStatus = GetPrivateProfileInt(L"其它功能", L"虚拟摄像头", 0, tzIniPath);
    
    if(CameraStatus)
    {
        if(CameraStatus==1) EnableCAM = 1;
        else EnableCAM = 0;
        InstallHook((DWORD)Common.GetFunc("?CheckCameraStatus@Detect@Util@@YAHXZ"), (DWORD)MyCheckCameraStatus);
        InstallHook((DWORD)Common.GetFunc("?CheckCameraStatus@Detect@Util@@YAHAAW4enumCamera@12@@Z"), (DWORD)MyCheckCameraStatus);
    }

	EnablePlugin = (QQPLUGIN)InstallHook((DWORD)Util::CoreCenter::EnablePlugin,(DWORD)MyEnablePlugin);
	DisablePlugin = (QQPLUGIN)InstallHook((DWORD)Util::CoreCenter::DisablePlugin,(DWORD)MyDisablePlugin);
    
	InstallHook((DWORD)Util::Misc::IsTencentTrusted, (DWORD)IsTencentTrusted);
	MyIsFileExist = (QQIsFileExist)InstallHook((DWORD)FS::IsFileExist, (DWORD)IsFileExist);

    InstallHook((DWORD)Common.GetFunc("?TXLog_DoTXLogVW@@YAXPAUtagLogObj@@PB_W1PAD@Z"), (DWORD)TXLog_DoTXLogVW);
    InstallHook((DWORD)Common.GetFunc("?GetLogByFilter@TXLog@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@ABVCTXStringW@@K0K@Z"), (DWORD)TXLog_DoTXLogVW);

	//QQGetTXDataStr = (MYGetTXDataStr)InstallHook((DWORD)Common.GetFunc("?GetTXDataBuf@Data@Util@@YAHPAUITXDataRead@@PB_WAAVCTXBuffer@@@Z"), (DWORD)GetTXDataStr);
    
	cofdf = (MYCreateObjectFromDllFile)InstallHook((DWORD)Util::Com::CreateObjectFromDllFile, (DWORD)CreateObjectFromDllFile);
    
	InstallHook((DWORD)Util::Group::CheckMsgImage, (DWORD)CheckMsgImage);

	//_beginthread(ChangeVersion,0,(PVOID)((DWORD)Version::Init + 0x10F));
    /*
    HMODULE m_ntdll = GetModuleHandle(L"ntdll.dll");
    if(m_ntdll)
    {
        LdrLoadDll = (DWORD)GetProcAddress(m_ntdll, "LdrLoadDll");
        DWORD offset = VAtoFileOffset(m_ntdll,(void*)LdrLoadDll);
        
        TCHAR szDLL[MAX_PATH+1] = {0};
        GetSystemDirectory(szDLL, MAX_PATH);
        lstrcat(szDLL, TEXT("\\ntdll.dll"));
        
        FILE *fp;
        fp = _wfopen(szDLL,L"rb");
        fseek(fp,offset,0);
        BYTE bak[5];
        fread(bak,1,5,fp);
        fclose(fp);
        
        WriteProcessMemory(g_process,(void*)LdrLoadDll, &bak, sizeof(bak), NULL);
        s_pfnLdrLoadDll = (PFN_LdrLoadDll)InstallHook(LdrLoadDll, (DWORD)MyLdrLoadDll);
    }
	*/
	s_pfnLdrLoadDll = (PFN_LdrLoadDll)InstallHook((DWORD)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "LdrLoadDll"), (DWORD)MyLdrLoadDll);

	//TM隐身抖动
    HMODULE m_hModule = LoadLibrary(L"ChatFrame.dll");
    if (m_hModule)
    {
        DWORD dwBaseAddr = (DWORD)m_hModule + 0x52000;
        BYTE arBytes[8200];
        ::ReadProcessMemory(g_process, (LPVOID)dwBaseAddr, arBytes, 8192, NULL);

        BYTE key[] = {0x0F, 0x85, 0x85, 0x00, 0x00, 0x00};
        int offset = memstr(key, sizeof(key), arBytes, 8192);

        if(offset!=-1)
        {
            WriteJMP(dwBaseAddr + offset,dwBaseAddr + offset + 0x169);
            WriteJMP(dwBaseAddr + offset + 0x1AF,dwBaseAddr + offset + 0x30E);
        }
    }
}
