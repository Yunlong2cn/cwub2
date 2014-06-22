void DrawTips(HWND hwnd)
{
	if( !IsWindow(hwnd) || !IsWindowVisible(GetParent(hwnd)))
	{
		return;
	}
	if( IsIconic(GetParent(hwnd)) || !g_hwnd_top)
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

    //OutputDebugString(L"DrawTips");
    HDC hdc = GetDC(hwnd);

	//DbgPrint(L"绘图");
    HDC mdc = (HDC)GetProp(hwnd,_T("MDC"));

	int trans = (int)GetProp(hwnd,_T("PAN"));

    //窗口大小设置
    SIZE sizeWindow = {220-trans,30};
    BLENDFUNCTION m_Blend = { AC_SRC_OVER, 0,255,AC_SRC_ALPHA};
    RECT rct;
    GetWindowRect(GetParent(hwnd), &rct);
    POINT ptSrc = {0, 0};

    //自定义显示位置
    POINT ptWinPos;
    ptWinPos.x = rct.right - 220 - 10 + trans;
    ptWinPos.y = rct.top + 55;

    //绘制最终图
    UpdateLayeredWindow(hwnd, hdc, &ptWinPos, &sizeWindow, mdc, &ptSrc, 0, &m_Blend, ULW_ALPHA);


	ReleaseDC(hwnd,hdc);

    InterlockedDecrement(&isrun);
    return;
}
BOOL CALLBACK top(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		{
			HDC hdc = GetDC(hWnd);
			HDC mdc = CreateCompatibleDC(hdc);
			SetProp(hWnd,_T("MDC"),mdc);
			ReleaseDC(hWnd,hdc);
		}
		SendMessage(hWnd,WM_TIMER,TQUERY,0);
		SetTimer(hWnd,TQUERY,1000,NULL);
        break;
    case WM_USER:
        DrawTips(hWnd);
        break;
	case WM_TIMER:
		switch(wParam)
		{
		case TQUERY:
			{
				if(!g_hwnd_top) break;
				unsigned long QQUIN = (unsigned long)GetProp(GetParent(hWnd),_T("QQUIN"));
				wchar_t tzAddress1[256];
				wchar_t tzAddress2[256];
				GetBarAddress(QQUIN, tzAddress1,3);
				GetBarAddress(QQUIN, tzAddress2,4);
				
				//
				HDC mdc = (HDC)GetProp(hWnd,_T("MDC"));
				unsigned int *data;
				HBITMAP hBitMap = SCreateCompatibleBitmap(220,30,(void**)&data);
				SelectObject(mdc, hBitMap);

				RECT rcText = {0, 2, 220, 15};
				DrawCWUBText(mdc,tzAddress1,showclr,rcText, DT_CENTER|DT_SINGLELINE|DT_VCENTER);

				rcText.top = 16;
				rcText.bottom = 30;
				DrawCWUBText(mdc,tzAddress2,showclr,rcText, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				
				DoTrans(&data,220,30);

				int trans = GetTransX(&data,220,30);
				if(trans>5)
				{
					trans -= 5;
					SetProp(hWnd,_T("PAN"),(HANDLE)trans);
				}

				DeleteObject(hBitMap);
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