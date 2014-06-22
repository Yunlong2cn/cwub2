#include "VersionMap.h"
#include "queryinfo.h"
#include "menu.h"

void ReadWndCfg()
{
    TCHAR buff[256];
	COLORREF tempclr;
    GetPrivateProfileString(L"显示位置", _T("文字颜色"), _T("101010"), buff, 255, configPath);
    _stscanf(buff,_T("%X"),&tempclr);
	showclr = RGB(GetBValue(tempclr),GetGValue(tempclr),GetRValue(tempclr));

	isGlow = GetPrivateProfileInt(L"显示位置", L"文字发光", 0, tzIniPath);
}

#include "left.h"
#include "top.h"
//#include <dwmapi.h>
//#pragma comment(lib, "dwmapi.lib")

void showtips(unsigned long QQUIN)
{
	HWND hWnd = Util::ChatSession::GetContactChatSessionMainHWnd(QQUIN);

    if(!IsWindow(hWnd) || GetProp(hWnd,_T("QQUIN"))!=0) return;//避免重复显示
    
    int kuandu = GetPrivateProfileInt(L"显示位置", _T("窗口宽度"), 0, configPath);
    if(kuandu)
    {
        RECT rct;
        GetClientRect(hWnd, &rct);
        SetWindowPos(hWnd, 0, 0, 0, kuandu, rct.bottom, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOACTIVATE |SWP_NOOWNERZORDER);
    }
    
	//BOOL enabled = false;
	//HRESULT hr = DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &enabled, sizeof(enabled));
	//if (SUCCEEDED(hr))
	//{
	//	MessageBox(0,0,0,1);
	//}
	//else
	//{
	//	MessageBox(0,0,0,0);
	//}

	AddToolTips(hWnd);

    SetProp(hWnd,_T("QQUIN"),(LPVOID)QQUIN);
    ::SetWindowSubclass(hWnd, MyWndProc, 0, 0);
    
	/*
    TCHAR MYQQ[64];
    TCHAR tzWanIP[256];
    wsprintf(MYQQ, _T("%X"), QQUIN);
    if(GetPrivateProfileString(_T("WAN"), MYQQ, L"", tzWanIP, 255, tzCachePath)==0)
    {
        _beginthread(QueryCacheIPFromNet, 0, (LPVOID)QQUIN);
    }
	*/



	
    ReadWndCfg();
    
    if(g_hwnd_left)
    {
        HWND c_hwnd3 = CreateDialog(g_hModule,MAKEINTRESOURCE(101),hWnd,left);
        SetProp(hWnd,_T("LeftHwnd"),(void*)c_hwnd3);
    }
    
    if(g_hwnd_top)
	{
        HWND c_hwnd4 = CreateDialog(g_hModule,MAKEINTRESOURCE(101),hWnd,top);
        SetProp(hWnd,_T("TopHwnd"), (void*)c_hwnd4);
    }
	/*
    if(firstrun)
    {
        firstrun = 0;
        if(update_model!=0) _beginthread(TCheckUpdate, 0, (LPVOID)hWnd);
    }
	*/

	TCHAR tzUpTips[4100] = L"";
	QueryInfo(QQUIN,tzUpTips,1);
	ShowUpBar(QQUIN,tzUpTips);

	TCHAR tzTips[4100] = L"";
	QueryInfo(QQUIN,tzTips,2);

    QQMsgInFrame(QQUIN,tzTips);
    MyGetStatus(QQUIN);
    /*
    if(firstrun)
    {
        int result = QQMsgBox(hWnd, L"您是首次使用本软件，是否需要阅读说明？", L"CWUB2", MB_YESNO + MB_ICONINFORMATION + MB_DEFBUTTON2);
        if(result==IDYES)
        {
            ShellExecute(NULL, L"open", L"http://www.shuax.com", NULL, NULL, SW_SHOWNORMAL);
        }
        firstrun = false;
        SetPrivateProfileInt(L"版权说明", L"首次运行", 0, configPath);
    }
    */
}

typedef void (__cdecl * MYOPENCONTACTSESSION) (unsigned long, struct ITXData *);
MYOPENCONTACTSESSION ChrOldProc = NULL;
void __cdecl MyOpenContactChatSession(unsigned long QQUIN, struct ITXData *ID)
{
    HWND hwnd = Util::ChatSession::GetContactChatSessionMainHWnd(QQUIN);
    if(ChrOldProc) ChrOldProc(QQUIN, ID);

    if (hwnd == 0 && QQUIN > 10000)
    {
        showtips(QQUIN);
    }
    return;
}

void QQMsgToText(struct ITXMsgPack *imp,unsigned long QQUIN);

typedef int (__cdecl * MYSAVEMSH) (wchar_t const *,unsigned long,unsigned long,unsigned long,struct ITXMsgPack *,struct ITXData *);
MYSAVEMSH  MsgOldProc = NULL;
unsigned long g_qquin;
ITXMsgPack * g_imp;
int __cdecl MySaveMsg(wchar_t const * str,unsigned long src,unsigned long des,unsigned long QQUIN,struct ITXMsgPack * imp,struct ITXData * ixd)
{
	g_qquin = QQUIN;
	g_imp = imp;
	//DbgPrint(L"%08X:%X %d %d %d %X %X ",MsgOldProc,str,src,des,QQUIN,imp,ixd);
	HWND hWnd = Util::ChatSession::GetContactChatSessionMainHWnd(QQUIN);
    if(wcsicmp(str,L"group")!=0) //避免在群窗口显示
    {
        showtips(QQUIN);        
        if(des==QQUIN)
        {
            if(des!=src)
            {
                if(voice_contact)
                {
                    if(!voice_wnd || (IsWindow(hWnd) && IsWindowVisible(hWnd)) ) QQMsgToText(imp,QQUIN);
                }
            }
            else
            {
                if(voice_self)
                {
                    if(!voice_wnd || (IsWindow(hWnd) && IsWindowVisible(hWnd)) ) QQMsgToText(imp,src);
                }
            }
        }
        else
        {
            if(voice_self)
            {
                if(!voice_wnd || (IsWindow(hWnd) && IsWindowVisible(hWnd)) )  QQMsgToText(imp,des);
            }
        }
    }
	QQUIN = g_qquin;
	imp = g_imp;
	//DbgPrint(L"%d",QQUIN);
	//DbgPrint(L"%X:%X %d %d %d %X %X ",MsgOldProc,str,src,des,QQUIN,imp,ixd);
    if(MsgOldProc) return MsgOldProc(str,src,des,QQUIN,imp,ixd);
    return 0;
}


typedef int (__cdecl * MYSAVEMSH2) (wchar_t const *,wchar_t const *,wchar_t const *,wchar_t const *,unsigned long,unsigned long,struct ITXMsgPack *,struct ITXData *);
MYSAVEMSH2 MsgOldProc2 = NULL;
int __cdecl MySaveMsg2(wchar_t const *str1,wchar_t const *str2,wchar_t const *str3,wchar_t const *str4,unsigned long uin1,unsigned long QQUIN,struct ITXMsgPack *imp,struct ITXData *ixd)
{
    if(QQUIN>10000 && (wcscmp(str1,L"group")==0||wcscmp(str1,L"discuss")==0) )
    {
		if(!voice_wnd || Util::ChatSession::IsChatSessionOpen(uin1,1) || Util::ChatSession::IsChatSessionOpen(uin1,3))
        {
            if(QQUIN != Util::Contact::GetSelfUin())
            {
                if(voice_group) QQMsgToText(imp,QQUIN);
            }
            else
            {
                if(voice_self) QQMsgToText(imp,QQUIN);
            }
        }
    }

    //调用原来的函数
    if(MsgOldProc2) return MsgOldProc2(str1,str2,str3,str4,uin1,QQUIN,imp,ixd);
    return 0;
}

typedef int (__cdecl * MYGetTXChatSession) (unsigned long,int,struct ITXData *,struct IAFChatSession * *);
MYGetTXChatSession TXCOldProc = NULL;
int __cdecl MyGetTXChatSession(unsigned long QQUIN,int CFChatType,struct ITXData * itd,struct IAFChatSession * *iac)
{
    int res = 0;
    if(TXCOldProc) res = TXCOldProc(QQUIN,CFChatType,itd,iac);
    
    if((CFChatType==2 || CFChatType==0) && QQUIN > 10000)
    {
        showtips(QQUIN);
    }
    else
    {
        Group(QQUIN);
    }
    return res;
}

void showtips()
{
	/*
	GdiplusStartupInput gdiplusStartupInput = NULL;
	ULONG_PTR gdiplusToken = NULL;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	*/
    HookLib KernelUtil;
	HookLib AppUtil;
	KernelUtil.Load(L"KernelUtil.dll");
	AppUtil.Load(L"AppUtil.dll");

	
	OldIsMaskFlagValid = (QQIsMaskFlagValid)InstallHook((DWORD)KernelUtil.GetFunc("?IsMaskFlagValid@Contact@Util@@YAJKEKAAH@Z"), (DWORD)IsMaskFlagValid);

    //firstrun = 1;
	g_qquin = 0;
    //update_model = GetPrivateProfileInt(L"其它功能", _T("升级模式"), 1, tzIniPath);
  
	//GetNickname = (GetAccountName)KernelUtil.GetFunc("?GetNickname@Contact@Util@@YA?AVCTXStringW@@K@Z");
	//GetPublicName = (GetAccountName)KernelUtil.GetFunc("?GetPublicName@Contact@Util@@YA?AVCTXStringW@@K@Z");
	//GetMsgAbstract  = (MsgAbstract)KernelUtil.GetFunc("?GetMsgAbstract@Msg@Util@@YA?AVCTXStringW@@PAUITXMsgPack@@@Z");

    ChrOldProc = (MYOPENCONTACTSESSION)InstallHook((DWORD)Util::ChatSession::OpenContactChatSession, (DWORD)MyOpenContactChatSession);
    MsgOldProc = (MYSAVEMSH)InstallHook((DWORD)KernelUtil.GetFunc("?SaveMsg@Msg@Util@@YAHPB_WKKKPAUITXMsgPack@@PAUITXData@@@Z"),(DWORD)MySaveMsg);
    MsgOldProc2 = (MYSAVEMSH2)InstallHook((DWORD)KernelUtil.GetFunc("?SaveMsg@Msg@Util@@YAHPB_W000KKPAUITXMsgPack@@PAUITXData@@@Z"),(DWORD)MySaveMsg2);
    
    TXCOldProc = (MYGetTXChatSession)InstallHook((DWORD)AppUtil.GetFunc("?GetTXChatSession@ChatSession@Util@@YAHKHPAUITXData@@PAPAUIAFChatSession@@@Z"),(DWORD)MyGetTXChatSession);
    if(TXCOldProc==0)
    {
        TXCOldProc = (MYGetTXChatSession)InstallHook((DWORD)AppUtil.GetFunc("?GetTXChatSession@ChatSession@Util@@YAHKHPAUITXData@@PAPAUITXChatSession@@@Z"),(DWORD)MyGetTXChatSession);
    }
    
}

