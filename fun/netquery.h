/*
static char header[] = "Content-Type: application/x-www-form-urlencoded";

HINTERNET MySafeInternetConnectA( HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
	if(UrlOldProcA) return UrlOldProcA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
	return NULL;
}
*/
bool QueryAddressFromNet1(in_addr ip,wchar_t *address)
{
	bool result = false;
    char frmdata[256];
    wsprintfA(frmdata, "http://ip.qq.com/cgi-bin/searchip?searchip1=%s", inet_ntoa(ip));

    HINTERNET hSession = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(hSession!=NULL)
    {
        HINTERNET hConnect = InternetOpenUrlA(hSession,frmdata,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE,0);
        if(hConnect!=NULL)
        {
            BYTE buffer[4100];
            DWORD DataReadLength = 0;
            while(1)
            {
                SetWininetTimeout(hConnect);
                InternetReadFile(hConnect,buffer,4096,&DataReadLength);
                if(DataReadLength!=0) result = true;
                if(DataReadLength==0) break;
                
                char *org = (char *)buffer;
                org[DataReadLength] = 0;
                
                static char lf[]="<p>该IP所在地为：<span>";
                static char rg[]="</span></p>";
                char *str = strstr(org, lf);
                if(str!=0)
                {
                    char *buf = str + strlen(lf);
                    *str = 0;
                    str = strstr(buf, rg);
                    if(str!=0)
                    {
                        *str = 0;
                        char *p1 = replace(buf, "中国", "");
                        char *p2 = replace(p1, "&nbsp;", "");
                        char *p3 = replace(p2, "您查询的IP信息没有记录， 期待着您的分享：", "");
                        wchar_t *p4 = Ansi2Unicode(p3);
                        wnsprintf(address,255,_T("%s"),p4);
                        free(p4);
                        free(p3);
                        free(p1);
                        free(p2);
                        break;
                    }
                }
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    return result;
}
bool QueryAddressFromNet2(in_addr ip,wchar_t *address)
{
    bool result = false;

    char frmdata[256];
    wsprintfA(frmdata, "http://www.myip.cn/%s", inet_ntoa(ip));

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
                if(DataReadLength!=0) result = true;
                if(DataReadLength==0) break;
                
                char *org = (char *)buffer;
                org[DataReadLength] = 0;
                
                static char lf[]="location=";
                static char rg[]="\";";
                char *str = strstr(org, lf);
                if(str!=0)
                {
                    char *buf = str + strlen(lf);
                    *str = 0;
                    str = strstr(buf, rg);
                    if(str!=0)
                    {
                        *str = 0;
                        
                        wchar_t *ustr = UTF82Unicode(buf);
                        wchar_t *p1 = replace(ustr, L" ", L"");
                        wchar_t *p2 = replace(p1, L".", L"");
                        wnsprintf(address,255,_T("%s"),p2);
                        free(p1);
                        free(p2);
                        free(ustr);
                        break;
                    }
                }
                
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    return result;
}

bool QueryAddressFromNet3(in_addr ip,wchar_t *address)
{
    bool result = false;

    char frmdata[256];
    wsprintfA(frmdata, "http://ip.cn/getip2.php?action=queryip&ip_url=%s", inet_ntoa(ip));
    
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
                if(DataReadLength!=0) result = true;
                if(DataReadLength==0) break;

                char *org = (char *)buffer;
                org[DataReadLength] = 0;
                
                static char lf[]="来自：";
                
                char *str = strstr(org, lf);
                if(str!=0)
                {
                    char *p1 = replace(str+strlen(lf), " ", "");
                    char *p2 = replace(p1, "-ip.cn", "");
                    wchar_t *p3 = Ansi2Unicode(p2);
                    wnsprintf(address,255,_T("%s"),p3);
                    free(p3);
                    free(p2);
                    free(p1);
                    break;
                }
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    return result;
}
