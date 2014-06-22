//#include "Update.h"
#include "pmgr.h"

//#include "hookqq.h"

#define CWUB_ABOUT (WM_APP+1)
#define CWUB_UPDATE (WM_APP+2)
#define CWUB_COPY (WM_APP+3)
#define CWUB_EDIT (WM_APP+4)
#define CWUB_CLER (WM_APP+9)

#define CWUB_CONT (WM_APP+5)
#define CWUB_SELF (WM_APP+6)
#define CWUB_GRUP (WM_APP+7)
#define CWUB_SETT (WM_APP+8)
#define CWUB_PMGR (WM_APP+10)
#define CWUB_MEMC (WM_APP+11)
#define CWUB_WNDF (WM_APP+12)

#define CWUB_WNDQ (WM_APP+13)

#define CWUB_WND1 (WM_APP+14)
#define CWUB_WND2 (WM_APP+15)
#define CWUB_WND3 (WM_APP+16)

//#define CWUB_HKQ1 (WM_APP+17)
//#define CWUB_HKQ2 (WM_APP+18)

#define CWUB_FONT (WM_APP+19)
#define CWUB_LVIP (WM_APP+20)

void ReadWndCfg();
void GetMyFont(PVOID pvoid)
{
    static bool isrun = 0;
    if(isrun) return;
    isrun = true;
	/*
	
	COLORREF color;
	COLORREF tempclr;
	GetPrivateProfileString(L"显示效果", _T("颜色"), _T("000000"), buff, 255, tzIniPath);
	_stscanf(buff,_T("%x"),&tempclr);
	color = RGB(GetBValue(tempclr),GetGValue(tempclr),GetRValue(tempclr));
	*/

	CHOOSECOLOR cc = {0};
	COLORREF acrCustClr[16] = {0};
	cc.lStructSize = sizeof(cc);
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;
	cc.hwndOwner = (HWND)pvoid;

	cc.rgbResult = showclr;
	if(ChooseColor(&cc))
	{
		showclr = cc.rgbResult;
		TCHAR buff[256];
		wsprintf(buff,L"%02X%02X%02X",GetRValue(showclr),GetGValue(showclr),GetBValue(showclr));
		WritePrivateProfileString(L"显示位置", _T("文字颜色"), buff, configPath);
		isGlow = GetPrivateProfileInt(L"显示位置", L"文字发光", 0, tzIniPath);

		HWND c_hwnd3 = (HWND)GetProp((HWND)pvoid,(_T("LeftHwnd")));
		HWND c_hwnd4 = (HWND)GetProp((HWND)pvoid,(_T("TopHwnd")));
		if(IsWindow(c_hwnd3)) SendMessage(c_hwnd3,WM_USER,0,0);
		if(IsWindow(c_hwnd4)) SendMessage(c_hwnd4,WM_USER,0,0);
	}
	/*
    CHOOSEFONT cf;
    LOGFONT lf;
	memset(&cf,0,sizeof(CHOOSEFONT));
	memset(&lf,0,sizeof(LOGFONT));


	
    GetPrivateProfileString(L"显示效果", _T("字体"), _T("Microsoft Yahei"), buff, 255, tzIniPath);
    wcscpy(lf.lfFaceName,buff);
	
	
    cf.lStructSize = sizeof(CHOOSEFONT);
    cf.hwndOwner = (HWND)pvoid;
	cf.hDC = (HDC)NULL;
	cf.iPointSize = 0;
    cf.lpLogFont = &lf;
    
	
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfHeight = -12;

    cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_LIMITSIZE | CF_INITTOLOGFONTSTRUCT | CF_NOVERTFONTS | CF_FORCEFONTEXIST | CF_NOSCRIPTSEL | CF_SELECTSCRIPT;
	cf.Flags &= ~0x02000000;
    cf.rgbColors = color;
    cf.lpfnHook = (LPCFHOOKPROC)NULL;
    cf.lpTemplateName = 0;
    cf.hInstance = (HINSTANCE) NULL;
    cf.lpszStyle = 0;
    cf.nFontType = REGULAR_FONTTYPE;
    cf.nSizeMin = 9;
    cf.nSizeMax = 9;

    if(ChooseFont(&cf))
    {
        color = cf.rgbColors;
        wsprintf(buff,L"%02X%02X%02X",GetRValue(color),GetGValue(color),GetBValue(color));
        WritePrivateProfileString(L"显示效果", _T("字体"), lf.lfFaceName, configPath);
        WritePrivateProfileString(L"显示效果", _T("颜色"), buff, configPath);
        
        ReadWndCfg();
        HWND c_hwnd3 = (HWND)GetProp((HWND)pvoid,(_T("LeftHwnd")));
        HWND c_hwnd4 = (HWND)GetProp((HWND)pvoid,(_T("TopHwnd")));
        if(IsWindow(c_hwnd3)) SendMessage(c_hwnd3,WM_USER,0,0);
        if(IsWindow(c_hwnd4)) SendMessage(c_hwnd4,WM_USER,0,0);
    }
	*/
    isrun = false;
    return;
}
void CopyBoard(TCHAR *text)
{
    HGLOBAL hSetData = NULL;
    LPBYTE lpSetData = NULL;
    INT nTextBytes = (wcslen(text) + 1) * sizeof(WCHAR);
    hSetData = GlobalAlloc(GPTR, nTextBytes);
    lpSetData = (LPBYTE)GlobalLock(hSetData);
    memcpy(lpSetData, text, nTextBytes);

    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, lpSetData);
    CloseClipboard();
    //SetProcessWorkingSetSize(GetCurrentProcess(),(size_t)-1,(size_t)-1);
}

BOOL CALLBACK left(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK top(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void DealWithMenu(HWND hWnd,int event)
{
    switch(event)
    {
        case 0:
            break;
        case CWUB_FONT:
            _beginthread(GetMyFont,0,(void*)hWnd);
            break;
        //case CWUB_HKQ1:
		//	_beginthread(UDPRecv, 0, (LPVOID)hWnd);
		//	break;
		//case CWUB_HKQ2:
		//	_beginthread(UDPSend, 0, (LPVOID)hWnd);
		//	break;
        case CWUB_PMGR:
            {
                HANDLE hMutex;
                hMutex = CreateMutex(NULL , TRUE, L"CWUB_PMGR");
                if (GetLastError() == ERROR_ALREADY_EXISTS)
                {
                    CloseHandle(hMutex);//即使已经存在仍需释放，否则计数会一直累加
                    SetForegroundWindow(FindWindow(NULL,L"幽香2 插件管理器"));
                    return;
                }
				
				DialogBox(g_hModule, MAKEINTRESOURCE(102), NULL, (DLGPROC)DialogProc);
                CloseHandle(hMutex);
            }
            break;
        case CWUB_WND1:
            g_hwnd_inner = !g_hwnd_inner;
            SetPrivateProfileInt(L"显示位置",L"框内显示", g_hwnd_inner, configPath);
            break;
        case CWUB_WND2:
            g_hwnd_left = !g_hwnd_left;
            SetPrivateProfileInt(L"显示位置",L"左下角显示", g_hwnd_left, configPath);
			if(GetProp(hWnd,_T("GROUP"))==0)
			{
				HWND c_hwnd3 = (HWND)GetProp(hWnd,(_T("LeftHwnd")));
				if(!c_hwnd3)
				{
					if(g_hwnd_left)
					{
						c_hwnd3 = CreateDialog(g_hModule,MAKEINTRESOURCE(101),hWnd,left);
						SetProp(hWnd,_T("LeftHwnd"),(void*)c_hwnd3);
					}
				}
				else SendMessage(c_hwnd3,WM_USER,0,0);
			}
            break;
        case CWUB_WND3:
            g_hwnd_top = !g_hwnd_top;
            SetPrivateProfileInt(L"显示位置",L"右上角显示", g_hwnd_top, configPath);
			if(GetProp(hWnd,_T("GROUP"))==0)
			{
				HWND c_hwnd4 = (HWND)GetProp(hWnd,(_T("TopHwnd")));
				if(!c_hwnd4)
				{
					if(g_hwnd_top)
					{
						c_hwnd4 = CreateDialog(g_hModule,MAKEINTRESOURCE(101),hWnd,top);
						SetProp(hWnd,_T("TopHwnd"), (void*)c_hwnd4);
					}
				}
				else SendMessage(c_hwnd4,WM_USER,0,0);
			}
			
			if(g_hwnd_top && !isOpenQQShow &&!GetPrivateProfileInt(L"显示位置", _T("窗口宽度"), 0, configPath))
			{
				int result = QQMsgBox(hWnd, L"您已经禁止了侧边栏的显示，是否需要扩大默认聊天窗口宽度？\r\n\r\n您也可以禁用部分插件或者打开精简图标。", L"幽香2", MB_YESNO + MB_ICONASTERISK);
				if(result==IDYES)
				{
					SetPrivateProfileInt(L"显示位置",L"窗口宽度", 510, configPath);
					if(GetProp(hWnd,_T("GROUP"))==0)
					{
						RECT rct;
						GetClientRect(hWnd, &rct);
						SetWindowPos(hWnd, 0, 0, 0, max(rct.right,510), rct.bottom, SWP_FRAMECHANGED | SWP_NOMOVE);// | SWP_NOACTIVATE |SWP_NOOWNERZORDER
					}
				}
			}
            break;
        case CWUB_CONT:
            voice_contact = !voice_contact;
            SetPrivateProfileInt(L"语音朗读",L"朗读好友消息", voice_contact, configPath);
            break;
        case CWUB_SELF:
            voice_self = !voice_self;
            SetPrivateProfileInt(L"语音朗读",L"朗读自己消息", voice_self, configPath);
            break;
        case CWUB_GRUP:
            voice_group = !voice_group;
            SetPrivateProfileInt(L"语音朗读",L"朗读群消息", voice_group, configPath);
            break;
        case CWUB_WNDQ:
            voice_wnd = !voice_wnd;
            SetPrivateProfileInt(L"语音朗读",L"窗口确认", voice_wnd, configPath);
            break;
            
        case CWUB_CLER:
            {
                int result = QQMsgBox(hWnd, L"清理缓存后，将会丢失之前所有的探测信息，确定继续吗？", L"幽香2", MB_OKCANCEL + MB_ICONASTERISK + MB_DEFBUTTON2);
                if(result==IDOK)
				{
                    DeleteFile(tzCachePath);
                    DeleteFile(tzQQWryPath);
                    WriteUnicode(tzCachePath);
                    WriteUnicode(tzQQWryPath);

                    //QQMsgBox(hWnd, L"恭喜你，缓存文件已经全部删除！", L"幽香2", MB_OK + MB_ICONINFORMATION);
                }
            }
            break;
        case CWUB_SETT:
            {
                wchar_t cpl[1024];
                if(IsVistaSystem())
                {
                    SHGetSpecialFolderPath(NULL, cpl, CSIDL_SYSTEM, FALSE);
                    wcscat(cpl,L"\\Speech\\SpeechUX\\sapi.cpl");
                }
                else
                {
                    SHGetSpecialFolderPath(NULL, cpl, CSIDL_PROGRAM_FILES_COMMON, FALSE);
                    wcscat(cpl,L"\\Microsoft Shared\\Speech\\sapi.cpl");
                }
                DWORD ret = (DWORD)ShellExecute(NULL, L"", cpl, NULL, NULL, SW_SHOWNORMAL);
				if(ret<32)
				{
					QQMsgBox(hWnd, L"语音选项打开失败。可能是因为您的操作系统是不支持TTS功能的精简版！", L"幽香2", MB_OK + MB_ICONWARNING);
				}
            }
            break;
        case CWUB_ABOUT:
            {
                HANDLE hMutex;
                hMutex = CreateMutex(NULL , TRUE, L"CWUB_About");
                if (GetLastError() == ERROR_ALREADY_EXISTS)
                {
                    CloseHandle(hMutex);//即使已经存在仍需释放，否则计数会一直累加
                    SetForegroundWindow(FindWindow(NULL,L"关于幽香2"));
                    return;
                }
                
                //wchar_t tips[1024];
                // __TIMESTAMP__ __DATE__
				QQMsgBox(0, L"幽香2  正式版  2012.02.05\r\n\r\n给你一个干净清爽的QQ！", L"关于幽香2", MB_OK + MB_ICONINFORMATION);//博客：http://www.shuax.com\n微博：http://t.qq.com/irisit
                
                CloseHandle(hMutex);
            }
            break;
        case CWUB_EDIT:
			if(!IsUserAdmin())
			{
				QQMsgBox(hWnd, L"由于权限不足，幽香2工作于兼容模式。\r\n\r\n这会导致手动编辑配置文件不一定能生效，此时应该在菜单直接进行设置。", L"幽香2", MB_OK + MB_ICONWARNING);
			}
            ShellExecute(NULL, L"", tzIniPath, NULL, NULL, SW_SHOWNORMAL);
            break;
        case CWUB_LVIP:
            lvip = !lvip;
            SetPrivateProfileInt(L"其它功能",L"本地会员", lvip, configPath);
            break;
        case CWUB_WNDF:
            wndf = !wndf;
            SetPrivateProfileInt(L"其它功能",L"屏蔽弹窗", wndf, configPath);
            break;
        case CWUB_MEMC:
            memc = !memc;
            SetPrivateProfileInt(L"其它功能",L"内存整理", memc, configPath);
            break;
        case CWUB_COPY:
            {
                unsigned long QQUIN = (unsigned long)GetProp(hWnd,(_T("QQUIN")));
                TCHAR tzTips[4100] = L"";
                QueryInfo(QQUIN,tzTips,3);
                CopyBoard(tzTips);
                
                bool paste = GetPrivateProfileInt(L"其它功能", _T("自动粘贴"), 0, tzIniPath);
                
                if(!paste) QQMsgBox(hWnd, tzTips, L"以下信息已经复制到粘贴板", MB_OK + MB_ICONINFORMATION);
                else SendMessage(hWnd,WM_PASTE,0,0);
            }
            break;
        //case CWUB_UPDATE:
        //    _beginthread(TCheckUpdate, 0, (LPVOID)hWnd);
        //    break;
    }
}

void ShowContextMenu(HWND hwnd)
{
    //鼠标坐标
    POINT pt;
    GetCursorPos(&pt);

    //
    HMENU hMenu = CreatePopupMenu();
    HMENU hPopMenu;
    
    hPopMenu = CreatePopupMenu();
    AppendMenu(hPopMenu, MF_BYPOSITION | (voice_contact?MF_CHECKED:0), CWUB_CONT, _T("朗读好友消息"));
    AppendMenu(hPopMenu, MF_BYPOSITION | (voice_self?MF_CHECKED:0), CWUB_SELF, _T("朗读自己消息"));
    AppendMenu(hPopMenu, MF_BYPOSITION | (voice_group?MF_CHECKED:0), CWUB_GRUP, _T("朗读群消息"));
    AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hPopMenu, MF_BYPOSITION | (voice_wnd?MF_CHECKED:0), CWUB_WNDQ, _T("窗口确认"));
    AppendMenu(hPopMenu, MF_BYPOSITION , CWUB_SETT, _T("语音选项..."));
    AppendMenu(hMenu, MF_POPUP | MF_BYPOSITION, (UINT)hPopMenu, _T("语音朗读"));
    
    AppendMenu(hMenu, MF_BYPOSITION, CWUB_PMGR, _T("插件管理..."));
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    
    /*
    if(GetProp(hwnd,_T("GROUP"))==0)
    {
        bool hexie = GetPrivateProfileInt(L"其它功能",L"和谐", 0, configPath);
		if((GetAsyncKeyState(VK_SHIFT)& 0x8000)!=0 || hexie)
		{
			hPopMenu = CreatePopupMenu();
			AppendMenu(hPopMenu, MF_BYPOSITION, CWUB_HKQ1, _T("查询好友IP"));
			AppendMenu(hPopMenu, MF_BYPOSITION, CWUB_HKQ2, _T("清除自己IP"));
			AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(hPopMenu, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED , 0, L"测试功能，慎用");
			AppendMenu(hMenu, MF_POPUP | MF_BYPOSITION, (UINT)hPopMenu, _T("HookQQ缓存"));
			SetPrivateProfileInt(L"其它功能", _T("和谐"), 1, configPath);
		}
	}
	*/
    hPopMenu = CreatePopupMenu();
    AppendMenu(hPopMenu, MF_BYPOSITION | (wndf?MF_CHECKED:0), CWUB_WNDF, _T("屏蔽弹窗"));
    AppendMenu(hPopMenu, MF_BYPOSITION | (lvip?MF_CHECKED:0), CWUB_LVIP, _T("本地会员"));
    AppendMenu(hPopMenu, MF_BYPOSITION | (memc?MF_CHECKED:0), CWUB_MEMC, _T("内存整理"));
    AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hPopMenu, MF_BYPOSITION, CWUB_CLER, _T("清理缓存"));
    AppendMenu(hPopMenu, MF_BYPOSITION, CWUB_EDIT, _T("手动编辑"));
    AppendMenu(hMenu, MF_POPUP | MF_BYPOSITION, (UINT)hPopMenu, _T("其它设置"));
    
    hPopMenu = CreatePopupMenu();
	AppendMenu(hPopMenu, MF_BYPOSITION | (g_hwnd_inner?MF_CHECKED:0), CWUB_WND1, _T("框内显示"));
	AppendMenu(hPopMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hPopMenu, MF_BYPOSITION | (g_hwnd_left?MF_CHECKED:0), CWUB_WND2, _T("左下角显示"));
    AppendMenu(hPopMenu, MF_BYPOSITION | (g_hwnd_top?MF_CHECKED:0), CWUB_WND3, _T("右上角显示"));
    AppendMenu(hPopMenu, MF_BYPOSITION, CWUB_FONT, _T("文字颜色..."));
    AppendMenu(hMenu, MF_POPUP | MF_BYPOSITION, (UINT)hPopMenu, _T("显示位置"));
    
    //AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    if(GetProp(hwnd,_T("GROUP"))==0) AppendMenu(hMenu, MF_BYPOSITION, CWUB_COPY, _T("复制信息"));


    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    //#ifndef DEBUG
    //if(update_model==0) AppendMenu(hMenu, MF_BYPOSITION, CWUB_UPDATE, _T("检查更新"));
    //#endif
    AppendMenu(hMenu, MF_BYPOSITION, CWUB_ABOUT, _T("关于我们"));

    //显示菜单
    //SetForegroundWindow(hwnd);
    int id = TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
	DestroyMenu(hMenu);

    DealWithMenu(hwnd, id);   
}


HWND ToolTipClassic(HWND hwnd,RECT rct, TCHAR *szTexto)
{
	TOOLINFO ti;
	//InitCommonControls();
	HWND hTool = CreateWindowEx(0,TOOLTIPS_CLASS,NULL,WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,hwnd,NULL,g_hModule,NULL);
	//GetClientRect(GetParent(hwnd), &ti.rect);
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwnd;
	ti.lpszText = szTexto;
	ti.rect = rct;

	SendMessage(hTool, WM_SETFONT, (WPARAM)g_hFont, 0);
	SendMessage(hTool, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hTool, TTM_SETMAXTIPWIDTH, 0, 200);
	return hTool;
}
void AddToolTips(HWND hWnd)
{
	RECT rcText;
	GetClientRect(hWnd, &rcText);
	rcText.left += 10;
	rcText.top = rcText.bottom - 30;
	rcText.bottom = rcText.top + 20;
	rcText.right = rcText.left + 230;
	HWND hTool = ToolTipClassic(hWnd,rcText,L"此处点击右键可弹出幽香2设置菜单");
	SetProp(hWnd,_T("HTOOL"),(void*)hTool);
}
inline void SetTopMost(HWND hWnd,bool enable)
{
	if(enable) ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else ::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
BOOL CALLBACK CloseSimilarWindows(HWND hwnd, LPARAM lParam)
{
	if (GetParent(hwnd)==NULL  &&  IsWindowVisible(hwnd)) //可见顶层窗口
	{
		TCHAR buff[256];
		GetClassName(hwnd, buff, 255);
		if (lstrcmp(buff, (TCHAR *)lParam) == 0) PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
	return 1;
}
LRESULT CALLBACK MyWndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
{
    HWND c_hwnd3 = (HWND)GetProp(hWnd,(_T("LeftHwnd")));
    HWND c_hwnd4 = (HWND)GetProp(hWnd,(_T("TopHwnd")));

	if(message==WM_THEMECHANGED)
	{
		hButtonTheme = (HANDLE)-1;
		g_hFont = IsVistaTheme()?g_hFont_w7:g_hFont_xp;
	}
    if(message==WM_SHOWWINDOW)
    {
        if(wParam==false)
        {
            if(IsWindow(c_hwnd3)) ShowWindow(c_hwnd3,SW_HIDE);
            if(IsWindow(c_hwnd4)) ShowWindow(c_hwnd4,SW_HIDE);
        }
        else
        {
            if(!IsIconic(hWnd))
			{
				if(IsWindow(c_hwnd3)) ShowWindow(c_hwnd3,SW_SHOW);
				if(IsWindow(c_hwnd4)) ShowWindow(c_hwnd4,SW_SHOW);
			}
        }
    }
	if(message==WM_MOVE||message==WM_PAINT)
	{
		/*
		RECT rcClient = { 0 };
		::GetClientRect(hWnd, &rcClient);
		BLENDFUNCTION blend = {AC_SRC_OVER,0,255,AC_SRC_ALPHA};

		HDC m_hDcPaint = ::GetDC(hWnd);

		//HDC m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
		//HBITMAP m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
		//HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
		//int iSaveDC = ::SaveDC(m_hDcOffscreen);

		HDC m_hDcLft = ::CreateCompatibleDC(m_hDcPaint);
		HDC m_hDcTop = ::CreateCompatibleDC(m_hDcPaint);
		HBITMAP m_hbmpLft = ::CreateCompatibleBitmap(m_hDcPaint, 230, 16);
		HBITMAP m_hbmpTop = ::CreateCompatibleBitmap(m_hDcPaint, 230, 30);

		::SelectObject(m_hDcLft, m_hbmpLft);
		//::lpAlphaBlend(m_hDcLft, 0, 0, 230, 16, m_hDcPaint, rcClient.left + 10, rcClient.bottom - 30, 230, 16, blend);

		::SelectObject(m_hDcTop, m_hbmpTop);
		//::lpAlphaBlend(m_hDcTop, 0, 0, 230, 30, m_hDcPaint, rcClient.right - 234, rcClient.top + 56, 230, 30,blend);



		HFONT hFont = MyCreateFont(L"Microsoft Yahei",9);
		::SetBkMode(m_hDcTop, TRANSPARENT);
		::SelectObject(m_hDcTop, hFont);
		::SetTextColor(m_hDcTop, clr);

		RECT rcText = {0, 0, 230, 15};
		::DrawText(m_hDcTop,L"222.18.127.50",-1,&rcText, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
		rcText.top = 15;
		rcText.bottom = 30;
		::DrawText(m_hDcTop,L"成都信息工程学院",-1,&rcText, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
		DeleteObject(hFont);

		DWORD dwSize = GetBitmapBits(m_hbmpTop, 0, NULL);
		unsigned char* pMem = (unsigned char*)malloc(dwSize);

		GetBitmapBits(m_hbmpTop, dwSize, pMem);
		for(DWORD i = 0; (i+3) < dwSize; i += 4)
		{
			if((pMem[i] == 0x0)
			&& (pMem[i+1] == 0x0)
			&& (pMem[i+2]) == 0x0)
			continue;
			pMem[i+3] = 0xFF;
			if(!akjsgfkjashgfkgsdjgdsagbjhkj) DbgPrint(L"%X",pMem[i+3]);
		}
		akjsgfkjashgfkgsdjgdsagbjhkj = true;

		SetBitmapBits(m_hbmpTop, dwSize, pMem);
		free(pMem);
		
		

		::lpAlphaBlend(m_hDcPaint, rcClient.left + 10, rcClient.bottom - 30, 230, 16, m_hDcLft, 0, 0, 230, 16, blend);
		::lpAlphaBlend(m_hDcPaint, rcClient.right - 234, rcClient.top + 56, 230, 30, m_hDcTop, 0, 0, 230, 30, blend);
		
		//::RestoreDC(m_hDcOffscreen, iSaveDC);

		::DeleteObject(m_hbmpLft);
		::DeleteObject(m_hbmpTop);
		::DeleteDC(m_hDcLft);
		::DeleteDC(m_hDcTop);
		::DeleteDC(m_hDcPaint);
		*/
		if(IsWindow(c_hwnd3)) SendMessage(c_hwnd3,WM_USER,wParam,lParam);
		if(IsWindow(c_hwnd4)) SendMessage(c_hwnd4,WM_USER,wParam,lParam);
	}
    if(message==WM_NCLBUTTONDOWN||message==WM_LBUTTONDOWN)
    {
        if(IsWindow(c_hwnd3))
        {
            //窗口位置
            RECT rct;
            GetWindowRect(c_hwnd3, &rct);

            //鼠标坐标
            POINT press;
            GetCursorPos(&press);

            if(PtInRect(&rct,press))
            {
                SendMessage(c_hwnd3,WM_TIMER,SWITCH,lParam);
            }
        }
    }
    if(message==WM_MBUTTONUP || message==WM_NCMBUTTONUP)
    {
		bool IsTopMost = GetProp(hWnd,L"ISTOPMOST");
		if(IsTopMost==0)
		{
			SetProp(hWnd,L"ISTOPMOST",(LPVOID)1);
		}
		else
		{
			SetProp(hWnd,L"ISTOPMOST",(LPVOID)0);
		}
		SetTopMost(hWnd,!IsTopMost);
        
        return TRUE;
    }
	if(message==WM_NCRBUTTONUP)
	{
		ShowContextMenu(hWnd);
		return TRUE;
	}
	
    /*
    
    if(message==WM_MSGBOX + MB_OK + MB_ICONINFORMATION)
    {
        return QQMsgBox(hWnd, (const wchar_t*)wParam, (const wchar_t*)lParam, MB_OK + MB_ICONINFORMATION);
    }
    if(message==WM_MSGBOX + MB_OK + MB_ICONERROR)
    {
        return QQMsgBox(hWnd, (const wchar_t*)wParam, (const wchar_t*)lParam, MB_OK + MB_ICONERROR);
    }
    if(message==WM_MSGBOX + MB_YESNO + MB_ICONINFORMATION)
    {
        return QQMsgBox(hWnd, (const wchar_t*)wParam, (const wchar_t*)lParam, MB_YESNO + MB_ICONINFORMATION);
    }
    if(message==WM_MSGBOX + MB_YESNO + MB_ICONERROR)
    {
        return QQMsgBox(hWnd, (const wchar_t*)wParam, (const wchar_t*)lParam, MB_YESNO + MB_ICONERROR);
    }
	*/
	/*
    if(message==WM_RESET)
    {
        AutoRestart();
        return TRUE;
    }
	
	if(message==WM_SIZE)
	{
		if(wParam==SIZE_MAXIMIZED) 
		{
			DbgPrint(L"最大化");
			SetProp(hWnd,L"ISMAX",(LPVOID)hWnd);
		}
		else
		{
			DbgPrint(L"还原");
			SetProp(hWnd,L"ISMAX",(LPVOID)0);
		}		
	}
	*/
    if(message==WM_SHOWTIPS)
    {
        if(GetProp(hWnd,L"IPTIPS")==0)
        {
            SetProp(hWnd,L"IPTIPS",(LPVOID)hWnd);
            wchar_t Temp[256];
            wsprintf(Temp, _T("%s - %s"), (const wchar_t*)wParam,(const wchar_t*)lParam);
            unsigned long QQUIN = (unsigned long)GetProp(hWnd,(_T("QQUIN")));
            QQMsgInFrame(QQUIN, Temp, false);
        }
        return TRUE;
    }
    if(message==WM_SHOWTIPS+1)
    {
        if(GetProp(hWnd,L"HITIPS")==0)
        {
            SetProp(hWnd,L"HITIPS",(LPVOID)hWnd);
            unsigned long QQUIN = (unsigned long)GetProp(hWnd,(_T("QQUIN")));
            QQMsgInFrame(QQUIN, (const wchar_t*)wParam, false);
        }
        return TRUE;
    }
    
    if(message==WM_WINDOWPOSCHANGING)
    {
		snapHandler.OnWindowPosChanging(lParam);//,GetProp(hWnd,L"ISMAX")!=0
    }
	
    if(message==WM_NCDESTROY)
    {
		HWND h_Tools = (HWND)GetProp(hWnd,(_T("HTOOL")));
		if(IsWindow(h_Tools)) DestroyWindow(h_Tools);

        if( KEYDOWN(VK_SHIFT) )
	    {
			static bool isrun = false;
			if (!isrun)
			{
				isrun = true;
				TCHAR buff[256];
				GetClassName(hWnd, buff, 255);
				EnumWindows(CloseSimilarWindows, LPARAM(buff));
				isrun = false;
			}
	    }
		RemoveWindowSubclass(hWnd,MyWndProc,0);
        
        //RemoveProp(hWnd,L"QQUIN");
        //RemoveProp(hWnd,L"GROUP");
        //RemoveProp(hWnd,L"IMME");
        
        //SendMessage(hWnd,WM_CLOSE,wParam,lParam);
        //return TRUE;
    }
    
    return DefSubclassProc(hWnd, message, wParam, lParam);
}
void Group(unsigned long QQUIN)
{
    //此处QQUIN代表ID
	HWND hWnd = Util::ChatSession::GetContactChatSessionMainHWnd(QQUIN);

	if(IsWindow(hWnd) && GetProp(hWnd,_T("QQUIN"))==0)
	{
		AddToolTips(hWnd);
        SetProp(hWnd,_T("QQUIN"),(LPVOID)QQUIN);
        SetProp(hWnd,_T("GROUP"),(LPVOID)QQUIN);//代表这是群窗口
        ::SetWindowSubclass(hWnd, MyWndProc, 0, 0);
	}
	
}