
TCHAR UrlFilterStr[10240];
int UrlFilterLen;

bool isInclude(const wchar_t *str, const wchar_t *cmp, int len)
{
	StringSplit split(str,0,len);
	for(int i=0;i<split.GetCount();i++)
	{
		if (wcsstr(cmp, split.GetIndex(i)) != 0)
		{
			return true;
		}
	}

	return false;
}

typedef int (__stdcall * SEND) (SOCKET s, char FAR *buf, int len, int flags);
SEND SendOldProc = NULL;
int WINAPI MySend(SOCKET s, char FAR *buf, int len, int flags)
{
    if(len>10 && ( (buf[0]=='G' && buf[1]=='E' && buf[2]=='T' && buf[3]==' ') || (buf[0]=='P' && buf[1]=='O' && buf[2]=='S' && buf[3]=='T' && buf[4]==' ') ) )
    {
		
        wchar_t* wszString = Ansi2Unicode(buf,len);
		wchar_t* end = wcsstr(wszString,L"\r\n\r\n");
		if(end)
		{
			end[0] = '\n';
			end[1] = 0;
		}
		//DbgPrint(L"%s",wszString);
        /*
        if(wcsstr(wszString,L"shuax.sinaapp.com")!=0)
        {
            wchar_t *str = wcsstr(wszString,L"&id=");
            if(str)
            {
                unsigned int qquin; 
                swscanf(str+4,L"%08X&",&qquin);
                if( GetIMVersion(qquin)==0 )
                {
                    free(wszString);
                    WSASetLastError(WSAENETRESET);
                    return SOCKET_ERROR;
                }
            }
        }
        */
        if(isInclude(UrlFilterStr,wszString, UrlFilterLen))
        {
            if(wcsstr(wszString,L"CWUB")==0)
            {
                free(wszString);
                //WSASetLastError(WSAENETRESET);
                return SOCKET_ERROR;
            }
        }
        
        free(wszString);
    }
    if(SendOldProc) return SendOldProc(s,buf,len,flags);
    
    return 0;
}

void UrlFilter()
{
	UrlFilterLen = GetPrivateProfileSection(L"Á¬½Ó¹ýÂË", UrlFilterStr, 10240, tzIniPath);
	SendOldProc = (SEND)InstallHook((DWORD)WS2_32.GetFunc("send"),(DWORD)MySend);
}
