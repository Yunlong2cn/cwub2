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
        SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONERROR, (WPARAM)L"����Ȩ�޲��㣬�޷���ɸ��²�����", (LPARAM)L"����2");
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
    
    if(!flag || myself!=crcheck) //���ʧ��
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
            if(SendMessage(hwnd, WM_MSGBOX + MB_YESNO + MB_ICONINFORMATION, (WPARAM)L"��ϲ���Զ�������ɣ���������QQ�󼴿��л����°汾��\n\n�Ƿ��Զ�������ǰQQ��", (LPARAM)L"����2")==IDYES)
            {
                SendMessage(hwnd,WM_RESET,0,0);
            }
            return;
        }
    }
    
    if(update_model!=2) SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONERROR, (WPARAM)L"�ǳ���Ǹ�������ļ������Զ�����ʧ�ܣ�", (LPARAM)L"����2");
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
    
    if(!newversion || !crcheck) //���ʧ��
    {
        if(update_model==0) SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONERROR, (WPARAM)L"�ǳ���Ǹ��������������ԭ�򣬼�����ʧ�ܣ�",(LPARAM) L"����2");
    }
    else
    {
        unsigned long myself = CRC32_FLE(tzPath);
        if(myself!=crcheck)
        {
            //Update(crcheck);
            
            //��⵽CRCʧ��
            if(newversion>CWUBVERSION)
            {
                //�汾��ͬ
                wchar_t tips[1024];
                wchar_t *wlogs = Ansi2Unicode(logs);
                wsprintf(tips,_T("��⵽�°汾%d.%03d���Ƿ������Զ����£�\n\n�����ԣ�%s"), newversion / 1000 , newversion % 1000, wlogs);
                free(wlogs);
                if(update_model==2 || SendMessage(hwnd, WM_MSGBOX + MB_YESNO + MB_ICONINFORMATION, (WPARAM)tips, (LPARAM)L"����2")==IDYES)
                {
                    Update(hwnd,crcheck);
                }
                
            }
            else
            {
                //�汾��ͬ
                if(update_model==2 || SendMessage(hwnd, WM_MSGBOX + MB_YESNO + MB_ICONERROR, (WPARAM)L"���棬��ʹ�õİ汾��Ҫ�������£��Ƿ������Զ����£�", (LPARAM)L"����2")==IDYES)
                {
                    Update(hwnd,crcheck);
                }
            }
        }
        else
        {
            //CRC��ͬ
            if(update_model==0) SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONINFORMATION, (WPARAM)L"��ϲ����ʹ�õ��Ѿ������°�����2��",(LPARAM) L"����2");
        }
    }
}

void TCheckUpdate(PVOID pvoid)
{
	HWND hwnd = (HWND)pvoid;
	static bool isrun = false;
	if (isrun)
	{
        SendMessage(hwnd, WM_MSGBOX + MB_OK + MB_ICONINFORMATION,(WPARAM)L"��Ҫ�ţ��������ں�̨������ :-)", (LPARAM)L"����2");
        return;
    }
	isrun = true;
	
	CheckUpdate(hwnd);
	
	isrun = false;
}