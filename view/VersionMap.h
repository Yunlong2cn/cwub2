#include "VersionTable.h"

struct version_entry
{
	unsigned short ver;
	wchar_t name[24];
};
struct version_entry *version_map;
int size_version_map = 0;

WCHAR *BinarySearch(unsigned short wVersion)
{
	int ver = wVersion;//MAKEWORD(wVersion % 100, wVersion / 100);
	int low = 0;
	int high = size_version_map;
	while (low <= high)
	{
		int mid = (low + high) / 2;
		if ( ver == version_map[mid].ver )
		{
			return version_map[mid].name;
		}
		else if ( ver < version_map[mid].ver ) high = mid - 1;
		else low = mid + 1;
	}
	return NULL;
}
/*
void QueryVersionFromNet(PVOID pvoid)
{
	static long isrun = 0;
	if(isrun) return;
	InterlockedIncrement(&isrun);
    
    char szKey[128];
    if(!SafeGetKey(szKey) ) return;
    char *key = Calculate(szKey);
    
    char frmdata[256];
    //wsprintfA(frmdata, "http://www.shuax.com/ver/?ver=%04X", pvoid);
    wsprintfA(frmdata, "http://%s/?m=%d&k=%s%s&ver=%04X", CWUB_SERVE, 3, szKey, key, pvoid);
	//OutputDebugStringA(frmdata);
    free(key);
    
    HINTERNET hSession = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(hSession!=NULL)
    {
        HINTERNET hConnect = InternetOpenUrlA(hSession,frmdata,NULL,0,INTERNET_FLAG_RELOAD,0);
        if(hConnect!=NULL)
        {
            BYTE buffer[1024];
            DWORD DataReadLength = 0;
            while(1)
            {
                InternetReadFile(hConnect,buffer,1023,&DataReadLength);
                if(DataReadLength==0 || DataReadLength>40) break;

                char *org = (char *)buffer;
                org[DataReadLength] = 0;
				//OutputDebugStringA(org);
                
                wchar_t *text = Ansi2Unicode(org,DataReadLength);
                
                WORD version;
                wchar_t ver[256] = {0};
                swscanf(text,L"%04X:%[^:]",&version,ver);
                if(version==(unsigned short)pvoid)
                {
                    wchar_t name[5];
                    wsprintf(name, L"%04X", pvoid);
                    WritePrivateProfileString(L"�汾��ȫ", name, ver, configPath);
                }
                free(text);
                break;
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hSession);
    }
    
    InterlockedDecrement(&isrun);
}
*/
BOOL InitVersion()
{
	version = GetPrivateProfileInt(L"��������", L"�汾αװ", 0, tzIniPath);
	//DbgPrint(L"%d",version);

    UInt32 outSize = 0; 
	BYTE *outStream = 0;
	if(LZMADec(VersionTable,sizeof(VersionTable),&outStream,&outSize,0)==LZMA_RESULT_OK)
	{
        size_version_map = outSize / sizeof(version_entry) - 1;
        version_map = (struct version_entry *)outStream;
        return TRUE;
	}
	return FALSE;
}

void GetVersion(unsigned long QQUIN, wchar_t *version)
{
    unsigned short IMVersion = WORD2BB(Util::Contact::GetIMVersion(QQUIN));
	wchar_t name[64];
	wsprintf(name, L"%04X", IMVersion);
	if(!GetPrivateProfileString(L"�汾��ȫ",name, L"", version, 255, tzIniPath))
	{
		//��ѯʧ��
		WCHAR *result = BinarySearch(IMVersion);
		if(result==NULL)
		{
			wsprintf(version, L"δ֪�汾[%04X]", IMVersion);
		}
		else wcscpy(version,result);
	}
	//FindVersion(WORD2BB(Util::Contact::GetIMVersion(QQUIN)), version);
}
