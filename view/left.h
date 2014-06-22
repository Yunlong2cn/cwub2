
bool isicon = false;
void DrawBar(HWND hwnd)
{
	if( !IsWindow(hwnd) || !IsWindowVisible(GetParent(hwnd)))
	{
		return;
	}
	if( IsIconic(GetParent(hwnd)) || !g_hwnd_left)
	{
		ShowWindow(hwnd,SW_HIDE);
		SetProp(hwnd,_T("ISHIDE"),(LPVOID)hwnd);
		return;
	}
	
	if(!IsWindowVisible(hwnd))
	{
		if(GetProp(hwnd,_T("ISHIDE")))
		{
			ShowWindow(hwnd,SW_SHOW);
			RemoveProp(hwnd,_T("ISHIDE"));
		}
	}

    static long isrun = 0;
    if(isrun) return;
    InterlockedIncrement(&isrun);

    HDC hdc = GetDC(hwnd);
	HDC mdc = (HDC)GetProp(hwnd,_T("MDC"));

	int fade = (int)GetProp(GetParent(hwnd),_T("ShowFade"));
	
    //窗口大小设置
    SIZE sizeWindow = {220,16};

    BLENDFUNCTION m_Blend = { AC_SRC_OVER, 0,fade,AC_SRC_ALPHA};
    RECT rct;
    GetWindowRect(GetParent(hwnd), &rct);
    POINT ptSrc = {0, 0};

    //自定义显示位置
    POINT ptWinPos;
    ptWinPos.x = rct.left + 12;
    ptWinPos.y = rct.bottom - 30;



    //绘制最终图
    UpdateLayeredWindow(hwnd, hdc, &ptWinPos, &sizeWindow, mdc, &ptSrc, 0, &m_Blend, ULW_ALPHA);
    ReleaseDC(hwnd,hdc);
    
    InterlockedDecrement(&isrun);
    return;
}

void Fadein(PVOID pvoid)
{
    HWND hWnd = (HWND)pvoid;
    if(IsWindow(hWnd) && IsWindowVisible(GetParent(hWnd)))
    {
		for(int i= 45;i<255;i+=20)
		{
			SendMessage(hWnd,WM_USER+1,(WPARAM)i,0);
			Sleep(30);
			if(!IsWindow(hWnd) || !IsWindowVisible(GetParent(hWnd)) ) break;
		}
		SendMessage(hWnd,WM_USER+1,255,0);
    }
}
/*
*/

void Fadeout(PVOID pvoid)
{
    HWND hWnd = (HWND)pvoid;
    if( IsWindow(hWnd) )
    {
        if(GetProp(hWnd,_T("Fadeout"))!=0) return;//避免重复显示
        else SetProp(hWnd,_T("Fadeout"),(LPVOID)hWnd);
    
        int fade = (int)GetProp(GetParent(hWnd),_T("ShowFade"));
        if(fade>30) fade = fade- 30;
        for(int i = fade;i>20;i-=30)
        {
            SendMessage(hWnd,WM_USER+1,(WPARAM)i,0);
            Sleep(30);
            if(!IsWindow(hWnd) || !IsWindowVisible(GetParent(hWnd)) ) break;
        }
        
        int IsSwitch = (int)GetProp(GetParent(hWnd),_T("SWITCH"));
        IsSwitch = !IsSwitch;
        SetProp(GetParent(hWnd),_T("SWITCH"),(void*)IsSwitch);
        

		SendMessage(hWnd,WM_TIMER,TQUERY,0);
		Fadein(hWnd);
		
        if(IsWindow(hWnd)) RemoveProp(hWnd,_T("Fadeout"));
    }
}

BOOL CALLBACK left(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SETFOCUS:
        SetFocus(GetParent(hWnd));
        break;
    case WM_INITDIALOG:
		if(GetProp(GetParent(hWnd),_T("GROUP"))==0) EndDialog(hWnd, 0);
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);

		ShowWindow(hWnd,SW_SHOW);

		SetProp(GetParent(hWnd),_T("SWITCH"),(void*)(Util::Contact::GetIMVersion((unsigned long)GetProp(GetParent(hWnd),_T("QQUIN")))==0));
		{
			HDC hdc = GetDC(hWnd);
			HDC mdc = CreateCompatibleDC(hdc);
			SetProp(hWnd,_T("MDC"),mdc);
			ReleaseDC(hWnd,hdc);
		}
		SendMessage(hWnd,WM_TIMER,TQUERY,0);
		SetTimer(hWnd,TQUERY,1000,NULL);
		SetTimer(hWnd,SWITCH,30 * 1000,NULL);
		//SendMessage(hWnd,WM_USER+1,255,0);
        _beginthread(Fadein,0,(PVOID)hWnd);
        break;
    case WM_USER:
        DrawBar(hWnd);
        break;
	case WM_USER+1:
        SetProp(GetParent(hWnd),_T("ShowFade"),(void*)wParam);
        DrawBar(hWnd);
        break;
    case WM_TIMER:
        switch(wParam)
        {
            case SWITCH:
				KillTimer(hWnd,SWITCH);
				_beginthread(Fadeout,0,(PVOID)hWnd);
				SetTimer(hWnd,SWITCH,30 * 1000,NULL);
				break;
			case TQUERY:
				{
					if(!g_hwnd_left) break;
					unsigned long QQUIN = (unsigned long)GetProp(GetParent(hWnd),_T("QQUIN"));
					wchar_t tzAddress1[256];
					wchar_t tzAddress2[256];
					GetBarAddress(QQUIN, tzAddress1,1);
					GetBarAddress(QQUIN, tzAddress2,2);

					HDC mdc = (HDC)GetProp(hWnd,_T("MDC"));
					unsigned int *data;
					HBITMAP hBitmap = SCreateCompatibleBitmap(220,16,(void**)&data);
					SelectObject(mdc, hBitmap);

					RECT rcText = {0, 0, 220, 16};
					int IsSwitch = (int)GetProp(GetParent(hWnd),_T("SWITCH"));
					if(!IsSwitch)
					{
						DrawCWUBText(mdc,tzAddress1,showclr,rcText);
					}
					else
					{
						DrawCWUBText(mdc,tzAddress2,showclr,rcText);
					}
					DoTrans(&data,220,16);
					DeleteObject(hBitmap);
				}
				break;
        }
        break;
	case WM_NCDESTROY:
		HDC mdc = (HDC)GetProp(hWnd,_T("MDC"));
		DeleteDC(mdc);

		break;
    }
    return 0;
}