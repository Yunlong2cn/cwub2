void AutoRestart()
{
	void* ptr = (LPVOID)((DWORD)Util::Misc::SwitchUser + 180);

    int length = 12;
    if(*(BYTE*)ptr==0xE8)
    {
        length+=5;
    }
    else
    {
        if(*(WORD*)ptr==0x15FF)
        {
            length+=6;
        }
        else
        {
            return ;
        }
    }
    
    BYTE dwFill[18];
    memset(dwFill, 0x90, 18);
    WriteProcessMemory( g_process, ptr, &dwFill, length, NULL);
    
    Util::Misc::SwitchUser();
}
void Update(HWND hwnd,unsigned long crcheck)
{
    if(!IsUserAdmin())
    {
        SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONERROR, (WPARAM)L"由于权限不足，无法完成更新操作。", (LPARAM)L"幽香2");
        return;
    }
    bool flag = false;
    
    HANDLE hfile = CreateFile(tzBakPath,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);  
    
    if (hfile!=INVALID_HANDLE_VALUE)
    {
        HINTERNET hSession = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if(hSession!=NULL)
        {
            HINTERNET hConnect = InternetOpenUrlA(hSession,tzUpdate1,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,0);
            if(hConnect!=NULL)
            {
                BYTE buffer[1024];
                DWORD DataReadLength = 0;
                DWORD DataWriteLength = 0;
                while(1)
                {
                    SetWininetTimeout(hConnect);
                    InternetReadFile(hConnect,buffer,sizeof(buffer),&DataReadLength);
                    if(DataReadLength==0) break;
                    if(flag==false)
                    {
                        if(buffer[0]=='M' && buffer[1]=='Z') flag = true;
                        else break;
                    }
                    WriteFile(hfile,buffer,DataReadLength,&DataWriteLength,NULL);  
                }
                CloseHandle(hfile);
            }
            InternetCloseHandle(hSession);
        }
    }
    
    unsigned long myself = CRC32_FLE(tzBakPath);
    //DbgPrint(L"CRC32_FLE %X",myself);
    
    if(!flag || myself!=crcheck) //检查失败
    {
        DeleteFile(tzBakPath);
    }
    else
    {
        
        wchar_t tzBakBakPath[MAX_PATH];
        wcscpy(tzBakBakPath,tzBakPath);
        wcscat(tzBakBakPath,L".bak");
        
        bool res = true;
        res *= MoveFile(tzPath,tzBakBakPath);
        res *= MoveFile(tzBakPath,tzPath);
        res *= MoveFile(tzBakBakPath,tzBakPath);
        
        if(res && update_model!=2)
        {
            if(SendMessage(hwnd, WM_MSGBOX + MB_YESNO + MB_ICONINFORMATION, (WPARAM)L"恭喜，自动更新完成，重新启动QQ后即可切换至新版本！\n\n是否自动重启当前QQ？", (LPARAM)L"幽香2")==IDYES)
            {
                SendMessage(hwnd,WM_RESET,0,0);
            }
            return;
        }
    }
    
    if(update_model!=2) SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONERROR, (WPARAM)L"非常抱歉，下载文件错误，自动更新失败！", (LPARAM)L"幽香2");
}
void CheckUpdate(HWND hwnd)
{
    int newversion = 0;
    unsigned long crcheck = 0;
    char logs[1024] = "";
    HINTERNET hSession = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(hSession!=NULL)
    {
        HINTERNET hConnect = InternetOpenUrlA(hSession,tzUpdate2,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,0);
        if(hConnect!=NULL)
        {
            BYTE buffer[1024];
            DWORD DataLength = 0;
            SetWininetTimeout(hConnect);
            InternetReadFile(hConnect,buffer,sizeof(buffer),&DataLength);
            buffer[DataLength]=0;
            sscanf((char*)buffer,"%d:%lx:%[^:]",&newversion,&crcheck,logs);
            
            char *ln = replace(logs, "\\n", "\n");
            strncpy(logs, ln, 1024);
            free(ln);
        }
    }
    
    if(!newversion || !crcheck) //检查失败
    {
        if(update_model==0) SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONERROR, (WPARAM)L"非常抱歉，可能由于网络原因，检查更新失败！",(LPARAM) L"幽香2");
    }
    else
    {
        unsigned long myself = CRC32_FLE(tzPath);
        if(myself!=crcheck)
        {
            //Update(crcheck);
            
            //检测到CRC失败
            if(newversion>CWUBVERSION)
            {
                //版本不同
                wchar_t tips[1024];
                wchar_t *wlogs = Ansi2Unicode(logs);
                wsprintf(tips,_T("检测到新版本%d.%03d，是否立即自动更新？\n\n新特性：%s"), newversion / 1000 , newversion % 1000, wlogs);
                free(wlogs);
                if(update_model==2 || SendMessage(hwnd, WM_MSGBOX + MB_YESNO + MB_ICONINFORMATION, (WPARAM)tips, (LPARAM)L"幽香2")==IDYES)
                {
                    Update(hwnd,crcheck);
                }
                
            }
            else
            {
                //版本相同
                if(update_model==2 || SendMessage(hwnd, WM_MSGBOX + MB_YESNO + MB_ICONERROR, (WPARAM)L"警告，您使用的版本需要紧急更新，是否立即自动更新？", (LPARAM)L"幽香2")==IDYES)
                {
                    Update(hwnd,crcheck);
                }
            }
        }
        else
        {
            //CRC相同
            if(update_model==0) SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONINFORMATION, (WPARAM)L"恭喜，您使用的已经是最新版幽香2！",(LPARAM) L"幽香2");
        }
    }
}

void TCheckUpdate(PVOID pvoid)
{
	HWND hwnd = (HWND)pvoid;
	static bool isrun = false;
	if (isrun)
	{
        SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONINFORMATION,(WPARAM)L"不要慌，程序正在后台检查更新 :-)", (LPARAM)L"幽香2");
        return;
    }
	isrun = true;
	
	CheckUpdate(hwnd);
	
	isrun = false;
}