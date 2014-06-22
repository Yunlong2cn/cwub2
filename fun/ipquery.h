//#include "netquery.h"
//#include "NetCache.h"
#include "cwubip.h"

void GetTime(unsigned long QQUIN, wchar_t *time);

struct PARAM
{
    in_addr ip;
    unsigned long qq;
    bool isTips;
};

bool QueryAddressFromNet(in_addr ip,wchar_t *address)
{
    bool result = false;

    TCHAR server[256];
    TCHAR  begin[256];
    TCHAR endstr[256];
    TCHAR filstr[1024];
    GetPrivateProfileString(L"在线查询", _T("服务商"), _T("http://www.ip138.com/ips8.asp?ip=%S&action=2"), server, 255, tzIniPath);
    GetPrivateProfileString(L"在线查询", _T("标记开始"), _T("<li>本站主数据："),   begin, 255, tzIniPath);
    GetPrivateProfileString(L"在线查询", _T("标记结束"), _T("</li><li>"),  endstr, 255, tzIniPath);
    GetPrivateProfileString(L"在线查询", _T("过滤字符串"), _T(" "),  filstr, 1023, tzIniPath);
	StringSplit split(filstr,L'|');

    int isUTF8 = GetPrivateProfileInt(L"在线查询", L"UTF8", 0, tzIniPath);

    TCHAR frmdata[256];
    wsprintf(frmdata, server, inet_ntoa(ip));

    HINTERNET hSession = InternetOpen(WUSER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(hSession!=NULL)
    {
        HINTERNET hConnect = InternetOpenUrl(hSession,frmdata,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_NO_COOKIES,0);
        if(hConnect!=NULL)
        {
            DWORD dwLength = 0;
            DWORD dwLengthLen = sizeof(dwLength);
            
            if(HttpQueryInfo(hConnect, HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_CONTENT_LENGTH, &dwLength, &dwLengthLen, NULL))
            {
                BYTE *buffer = (BYTE*)malloc(dwLength+1);
                dwLengthLen = 0;
                if(InternetReadFile(hConnect, buffer, dwLength,&dwLengthLen))
                {
                    if(dwLength==dwLengthLen)
                    {
                        buffer[dwLength] = 0;
                        result = true;
                        
                        wchar_t *org = NULL;
                        if(isUTF8) org = UTF82Unicode((const char *)buffer,dwLength);
                        else org = Ansi2Unicode((const char *)buffer,dwLength);

                        wchar_t *str = wcsstr(org, begin);
                        if(str!=0)
                        {
                            wchar_t *buf = str + wcslen(begin);
                            *str = 0;
                            str = wcsstr(buf, endstr);
                            if(str!=0)
                            {
                                *str = 0;

                                for(int i=0;i<split.GetCount();i++)
                                {
                                    wchar_t *p = replace(buf, split.GetIndex(i), L"");
                                    wcscpy(buf,p);
                                    free(p);
                                }

                                wsprintf(address,_T("%s"),buf);
                                trim(address);
                            }
                        }
                        free(org);
                    }
                }
                free(buffer);
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    return result;
}

void ShowIP(unsigned long QQUIN,wchar_t *ip,wchar_t *address)
{
    HWND hWnd = Util::ChatSession::GetContactChatSessionMainHWnd(QQUIN);
    if(IsWindow(hWnd) && IsWindowVisible(hWnd))
    {
        SendMessage(hWnd, WM_SHOWTIPS, (WPARAM)ip,(LPARAM)address);
    }
}
void QueryFromNet(PARAM* tmp)
{
    wchar_t Temp[64];
    wsprintf(Temp, _T("%S"), inet_ntoa(tmp->ip));

    wchar_t address[256] = L"";
    
    for(int i=0;i<3;i++)
    {
        QueryAddressFromNet(tmp->ip, address);
       
        if (address[0] != 0) break;
    }

    if (address[0] == 0)
    {
        wcscpy(address, L"查询失败");
    }
    else
    {
        if(tmp->isTips) ShowIP(tmp->qq, Temp, address);
    }
    if(wcslen(address)>34) address[34]=0;
    WritePrivateProfileString(L"IP", Temp, address, tzQQWryPath);
    
    free(tmp);
}


void QueryOnline(PVOID pvoid)
{
	static long isrun = 0;
	if(isrun) return;
	InterlockedIncrement(&isrun);

    QueryFromNet((PARAM*)pvoid);

    InterlockedDecrement(&isrun);
}

void QueryIPAddress(unsigned long QQUIN)
{
    wchar_t MYQQ[64];
    wsprintf(MYQQ, _T("%X"), QQUIN);

    wchar_t tzWanIP[256];

    if (GetPrivateProfileString(_T("WAN"), MYQQ, _T(""), tzWanIP, 255, tzCachePath))
    {
        wchar_t tzLanIP[256];
        GetPrivateProfileString(_T("LAN"), MYQQ, _T("0.0.0.0"), tzLanIP, 255, tzCachePath);
        wchar_t *address = getipwhere(Unicode2Ansi(tzWanIP),Unicode2Ansi(tzLanIP));
        if (address[0] != 0)
        {
            ShowIP(QQUIN, tzWanIP, address);
        }
        
        // 已经获得IP
        wchar_t buff[256];
        GetPrivateProfileString(L"IP", tzWanIP, L"", buff, 255, tzQQWryPath);
        if ( buff[0]==0 || wcscmp(buff, L"查询失败") == 0 )
        {
            PARAM *tmp = (PARAM *)malloc(sizeof(PARAM));

            char *ip = Unicode2Ansi(tzWanIP);
            tmp->ip.s_addr = inet_addr(ip);
            tmp->qq = QQUIN;
            tmp->isTips = true;
            free(ip);

            _beginthread(QueryOnline, 0, (LPVOID)tmp);
        }
        else
        {
            ShowIP(QQUIN, tzWanIP, buff);
        }
    }
}