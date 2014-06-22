//#pragma comment (linker, "/ALIGN:0x200")
//#pragma comment (linker, "/OPT:NOWIN98")

#include "CWUB2.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_hModule = hModule;
        g_process = GetCurrentProcess();

        LoadSysDll();

        wchar_t MainModel[MAX_PATH];
        GetModuleFileName(0, MainModel, sizeof(MainModel) / sizeof(TCHAR));
        //OutputDebugString(MainModel);
        if( isEndWith(MainModel,L"QQ.exe") || isEndWith(MainModel,L"TM.exe") )
        {
			//DbgPrint(L"成功注入");
			//double begin = clock();

            //协助加载
            LoadLibrary(L"Fineip.dll");
            LoadLibrary(L"QQFun.dll");
            LoadLibrary(L"HookQQ.dll");
            HINSTANCE m_hModule = LoadLibrary(L"qqext.dll");
            if (!m_hModule) m_hModule = LoadLibrary(L"qqext\\qqext.dll");
            if (m_hModule)
            {
                FARPROC fpAddress = GetProcAddress(m_hModule, "Onload");
                if (fpAddress != NULL)
                {
                    typedef DWORD (WINAPI * Onload) (LPVOID);
                    ((Onload)fpAddress)(NULL);
                }
            }

            if(InitCWUB2() && InitVersion() && InitPlugin() && InitHook())
            {                
				_beginthread(MyThread,0,NULL);// 读取配置、消息循环
                ToCrack();//内存补丁
                LocalVIP();
                QQWrap();
                UrlFilter();
                ipprobe();
                Miscellaneous();
                HookCreateProcess();
				showtips();
				//LoadLibrary(L"AppFramework.dll");
            }
			//DbgPrint(L"%f",clock()-begin);
        }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}