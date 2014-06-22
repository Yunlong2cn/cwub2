void QueryIPAddress(unsigned long QQUIN);

char isNetWork = 0;
bool GetKey(char *key)
{
    if(isNetWork>3) return false;
    
    bool res = false;
    key[0] = 0;
    HINTERNET hSession = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(hSession!=NULL)
    {
        //OutputDebugStringA("InternetOpenA");
		char frmdata[1024];
		wsprintfA(frmdata, "http://%s/?m=0", CWUB_SERVE);

        HINTERNET hConnect = InternetOpenUrlA(hSession,frmdata,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,0);
        //OutputDebugStringA("InternetOpenUrlA1");
        if(hConnect!=NULL)
        {
            //OutputDebugStringA("InternetOpenUrlA2");
            BYTE buffer[1024];
            DWORD DataReadLength = 0;
            while(1)
            {
                SetWininetTimeout(hConnect);
                InternetReadFile(hConnect,buffer,1023,&DataReadLength);
                //OutputDebugStringA("InternetReadFile");
                res = true;
                if(DataReadLength==0 || DataReadLength>127) break;

                char *org = (char *)buffer;
                org[DataReadLength] = 0;
                strcpy(key,org);
                //OutputDebugStringA(org);
                isNetWork = 0;
                break;
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    return res;
}
bool SafeGetKey(char *key)
{
	if(GetPrivateProfileInt(L"其它功能", _T("网络缓存"), 1, tzIniPath)==0) return false;
    bool flag = false;
    for(int i=0;i<3;i++)
    {
        if(GetKey(key))
        {
            flag = true;
            break;
        }
    }
    if(!flag || strlen(key)!=14)
    {
        flag = false;
        isNetWork++;
    }
    
    return flag;
}
void QueryCacheIPFromNet(PVOID pvoid)
{
    char szKey[128];
	//OutputDebugStringA("QueryCacheIPFromNet1");
    if(!SafeGetKey(szKey) ) return;//Util::Contact::IsStranger((DWORD)pvoid) || 
    
    //OutputDebugStringA("QueryCacheIPFromNet2");
    char *key = Calculate(szKey);

    char frmdata[1024];
    wsprintfA(frmdata, "http://%s/?m=%d&k=%s%s&id=%08X&qq=%08X", CWUB_SERVE, 2, szKey, key, pvoid, Util::Contact::GetSelfUin());
    //OutputDebugStringA(frmdata);
    free(key);
    
    HINTERNET hSession = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(hSession!=NULL)
    {
        HINTERNET hConnect = InternetOpenUrlA(hSession,frmdata,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,0);
        if(hConnect!=NULL)
        {
            BYTE buffer[1024];
            DWORD DataReadLength = 0;
            while(1)
            {
                SetWininetTimeout(hConnect);
                InternetReadFile(hConnect,buffer,1023,&DataReadLength);
                if(DataReadLength==0) break;

                char *org = (char *)buffer;
                org[DataReadLength] = 0;
                //OutputDebugStringA(org);
                unsigned int qquin; 
                unsigned int wanip; 
                unsigned int lanip; 
                unsigned int time; 
                sscanf(org,"%08X:%08X:%08X:%08X:",&qquin,&wanip,&lanip,&time);
                if(qquin==(unsigned long)pvoid)
                {
                    struct in_addr TempIP;
                    
                    TCHAR MYQQ[64];
                    wsprintf(MYQQ, _T("%X"), qquin);
                    
                    TCHAR Temp[64];
                    
                    TempIP.s_addr = wanip;
                    wsprintf(Temp, _T("%S"), inet_ntoa(TempIP));
                    WritePrivateProfileString(_T("WAN"), MYQQ, Temp, tzCachePath);
                    
                    TempIP.s_addr = lanip;
                    wsprintf(Temp, _T("%S"), inet_ntoa(TempIP));
                    WritePrivateProfileString(_T("LAN"), MYQQ, Temp, tzCachePath);
                    
                    wsprintf(Temp, _T("%u"), time);
                    WritePrivateProfileString(_T("TIME"), MYQQ, Temp, tzCachePath);
                    
                    QueryIPAddress(qquin);
                }
                break;
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    
    //isrun = false;
}
/*
void PostSelfIPToNet(PVOID pvoid)
{
    char szKey[128];

    bool flag = true;
    for(int i=0;i<3;i++)
    {
        if(GetKey(szKey))
        {
            flag = false;
            break;
        }
    }
    if(flag) return;
    
    char *key = Calculate(szKey);

    char frmdata[1024];
    wsprintfA(frmdata, "http://shuax.sinaapp.com/test.php?m=%d&k=%s:%s&id=%08X", 3, szKey, key, pvoid);
    OutputDebugStringA(frmdata);
    free(key);
    
    HINTERNET hSession = InternetOpenA("CWUB/2.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(hSession!=NULL)
    {
        HINTERNET hConnect = InternetOpenUrlA(hSession,frmdata,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,0);
        if(hConnect!=NULL)
        {
            BYTE buffer[1024];
            DWORD DataReadLength = 0;
            
            SetWininetTimeout(hConnect);
            InternetReadFile(hConnect,buffer,1023,&DataReadLength);
            
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    
}
*/