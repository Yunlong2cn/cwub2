
wchar_t szClassName[] = TEXT("CWUB_BossKey_Wnd");


void HotKeyRegister(LPVOID pvoid)
{
	//DbgPrint(L"�����");

	HANDLE hMutex = CreateMutex(NULL , TRUE, szClassName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//DbgPrint(L"�ȴ���");
		WaitForSingleObject(hMutex,INFINITE);
	}

	//DbgPrint(L"��ʼע���ȼ�");
	//OutputDebugStringA("��ʼע���ȼ�");
	UINT mo = 0;
	UINT vk = 0;
	
	StringSplit split(g_szHotkey,L'+');
	for(int i=0;i<split.GetCount();i++)
	{
		wchar_t* str2 = split.GetIndex(i);

		if(wcsicmp(str2,L"Shift")==0) mo |= MOD_SHIFT;
		else if(wcsicmp(str2,L"Ctrl")==0) mo |= MOD_CONTROL;
		else if(wcsicmp(str2,L"Alt")==0) mo |= MOD_ALT;
		else if(wcsicmp(str2,L"Win")==0) mo |= MOD_WIN;
		
		wchar_t wch = str2[0];
		if (wcslen(str2)==1)
		{
			if(iswalnum(wch)) vk = towupper(wch);
			else vk = LOWORD(VkKeyScan(wch));
		}
		else if (wch=='F'||wch=='f')
		{
			if(iswdigit(str2[1]))  vk = VK_F1 + _ttoi(&str2[1]) - 1;
		}
		else
		{
			if(wcsicmp(str2,L"Left")==0) vk = VK_LEFT;
			else if(wcsicmp(str2,L"Right")==0) vk = VK_RIGHT;
			else if(wcsicmp(str2,L"Up")==0) vk = VK_UP;
			else if(wcsicmp(str2,L"Down")==0) vk = VK_DOWN;

			else if(wcsicmp(str2,L"End")==0) vk = VK_END;
			else if(wcsicmp(str2,L"Home")==0) vk = VK_HOME;

			else if(wcsicmp(str2,L"Tab")==0) vk = VK_TAB;
			else if(wcsicmp(str2,L"Space")==0) vk = VK_SPACE;

			else if(wcsicmp(str2,L"Esc")==0) vk = VK_ESCAPE;
			else if(wcsicmp(str2,L"Delete")==0) vk = VK_DELETE;

			else if(wcsicmp(str2,L"PageUp")==0) vk = VK_PRIOR;
			else if(wcsicmp(str2,L"PageDown")==0) vk = VK_NEXT;
		}

	}

	if( IsSystemWin7() ) mo |= 0x4000;
	
	//DbgPrint(L"��Ϣѭ��");
	SendMessage((HWND)pvoid,WM_MYHOTKEY,1000,MAKELPARAM(mo,vk));

	
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//OutputDebugStringA("�˳��ȼ��߳�");
}


BOOL CALLBACK broadcast(HWND hWnd, LPARAM lParam)
{
	TCHAR buff[256];
	DWORD dwData =GetWindowLong(hWnd,GWL_USERDATA);
	if(dwData==0x1353C5B7)
	{
		GetWindowText(hWnd, buff, 255);
		if (wcscmp(buff, szClassName) == 0) //�Ƚϴ�����
		{
			SendMessage(hWnd,WM_MYHOTKEY+1,0,lParam);
		}
	}
	return true;
}


NOTIFYICONDATAW nid;

typedef BOOL (WINAPI* TShell_NotifyIconW)(
									  DWORD dwMessage,
									  PNOTIFYICONDATAW lpData
									  );
TShell_NotifyIconW OldShell_NotifyIconW = NULL;
void HideTray(bool show)
{
	nid.uFlags |= NIF_STATE;
	nid.dwState = show;
	nid.dwStateMask = NIS_HIDDEN;
	if(OldShell_NotifyIconW)  OldShell_NotifyIconW(NIM_MODIFY,&nid);
}
BOOL WINAPI MyShell_NotifyIconW(DWORD dwMessage, __in PNOTIFYICONDATAW lpData)
{
	//DbgPrint(L"dwMessage:%d",dwMessage);
	
	if(dwMessage==NIM_ADD||dwMessage==NIM_MODIFY)
	{
		memcpy(&nid,lpData,lpData->cbSize);
	}
	
	if(isBossKey)
	{
		HideTray(NIS_HIDDEN);
		return 0;
	}
	

	if(OldShell_NotifyIconW)  return OldShell_NotifyIconW(dwMessage,lpData);

	return 0;
}

HWND WndList[100];
int now = 0;
BOOL CALLBACK SearchQQ(HWND hWnd, LPARAM lParam)
{
	if(IsWindowVisible(hWnd))
	{
		TCHAR buff[256];
		GetClassName(hWnd, buff, 255);
		if ( wcscmp(buff, L"TXGuiFoundation") == 0) //�Ƚ�����
		{
			DWORD pid;
			GetWindowThreadProcessId(hWnd,&pid);
			if(GetCurrentProcessId()==pid)
			{
				if(now<100)
				{
					ShowWindow(hWnd,SW_HIDE);
					WndList[now] = hWnd;
					now++;
				}
			}
		}
	}
	return true;
}

void RestoreQQ()
{
	for(int i = now - 1;i>=0;i--)
	{
		HWND hWnd = WndList[i];
		ShowWindow(hWnd,SW_SHOW);
		//SetForegroundWindow(hWnd);
		//if(IsIconic(hWnd)) ShowWindow(hWnd,SW_RESTORE);
	}
	now = 0;
}

void HideAllWindow()
{
	if(isBossKey)
	{
		//����
		HideTray(NIS_HIDDEN);
		EnumWindows(SearchQQ,0);
	}
	else
	{
		//��ԭ
		HideTray(0);
		RestoreQQ();
	}
}
BOOL CALLBACK BossKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
		//OutputDebugStringA("�����ϰ���������");
        ShowWindow(hWnd,SW_HIDE);
		SetWindowText(hWnd,szClassName);
		SetWindowLong(hWnd,GWL_USERDATA,0x1353C5B7);
		 _beginthread(HotKeyRegister,0,(LPVOID)hWnd);
		OldShell_NotifyIconW = (TShell_NotifyIconW)InstallHook((DWORD)Shell_NotifyIconW, (DWORD)MyShell_NotifyIconW);
        break;
	case WM_MYHOTKEY:
		if(!RegisterHotKey(hWnd,wParam,LOWORD(lParam), HIWORD(lParam)))
		{
			Sleep(300);
			wchar_t Tips[512];
			wsprintf(Tips,L"��ݼ� \"%s\" ע��ʧ�ܣ����������ϰ�����á�",g_szHotkey);
			MessageBox(hWnd,Tips,L"����2",MB_OK | MB_ICONWARNING | MB_TOPMOST);
		}
		break;
	case WM_MYHOTKEY + 1:
		isBossKey = lParam;
		HideAllWindow();
		break;
	case WM_HOTKEY:
		isBossKey = !isBossKey;
		EnumWindows(broadcast,isBossKey);
		break;
    }
    return 0;
}