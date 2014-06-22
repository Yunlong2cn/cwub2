
#include "cwublan.h"

#pragma pack(1) //保证按照1字节对齐
#define IINFO_MAGIC 0x1353C5B7
struct DataHeader
{
    DWORD magic;
    DWORD total;
    DWORD crc32;
    
    DWORD dic1_start; //字典1 
    DWORD dic2_start; //字典2 
};
struct IPTable
{
    DWORD begin;//开始IP
    DWORD offset1;//区域偏移
    DWORD offset2;//地址偏移
};
#pragma pack(4)

BOOL WryOK;
BYTE* MyBufferResult;

int BinarySearch(IPTable * tb1,IPTable * tb2,DWORD ip)
{
    //int ver = wVersion;//MAKEWORD(wVersion % 100, wVersion / 100);
    int low = 0;
    int high = (tb2 - tb1);
    while (low <= high)
    {
        int mid = (low + high) / 2;
        if ( ip == tb1[mid].begin )
        {
            return mid;
        }
        else if ( ip < tb1[mid].begin ) high = mid - 1;
        else low = mid + 1;
    }
    //printf("%d %d\n",high,low);
    return high;
}

BOOL CreateCacheIP(wchar_t *FileName)
{
    //
    HANDLE hfile = CreateFile(tzQQWry,
            GENERIC_READ,
            FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BY_HANDLE_FILE_INFORMATION file_info;
    GetFileInformationByHandle(hfile, &file_info);
    DWORD FileLength = file_info.nFileSizeLow;
    if(FileLength < sizeof(DataHeader))
        return FALSE;
        
    HANDLE hfilemap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 0, NULL);
    CloseHandle(hfile);

    BYTE *FileBuf = (BYTE*) MapViewOfFile(hfilemap, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(hfilemap);
    
    DataHeader *d_Header = (DataHeader*)FileBuf;
    int datalen = sizeof(DataHeader) + 5 + 8;
    
    DWORD dwAttributes = GetFileAttributes(FileName);
    bool NeedUpdate = true;//需要刷新
    if( dwAttributes!=0xFFFFFFFF )
    {
        if( d_Header->magic == IINFO_MAGIC )
        {
            UInt32 lzmasize =*(UInt32 *)(FileBuf+sizeof(DataHeader) + LZMA_PROPERTIES_SIZE) + datalen;
            
            HANDLE hTempIP = CreateFile(FileName,
                    GENERIC_READ,
                    FILE_SHARE_READ, NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, NULL);
			if (hTempIP != INVALID_HANDLE_VALUE)
			{
				BY_HANDLE_FILE_INFORMATION temp_info;
				GetFileInformationByHandle(hTempIP, &temp_info);
	            
				NeedUpdate = !(temp_info.nFileSizeLow==lzmasize);

				CloseHandle(hTempIP);
			}
        }
    }
    
    if(NeedUpdate)
    {
        if( d_Header->magic != IINFO_MAGIC || CRC32_MEM(FileBuf + datalen, FileLength - datalen) != d_Header->crc32 )
        {
            UnmapViewOfFile(FileBuf);
            return FALSE;
        }
        
        UInt32 outSize = 0; 
        BYTE *outStream = 0;

        if(LZMADec(FileBuf+sizeof(DataHeader),FileLength - sizeof(DataHeader),&outStream,&outSize,datalen)==LZMA_RESULT_OK)
        {
            memcpy(outStream,(BYTE*)FileBuf,datalen);

            FILE *fp = _wfopen(FileName, _T("wb"));
            if(fp)
            {
                fwrite(outStream,outSize,1,fp);
                fclose(fp);
            }

            free(outStream);
        }
    }
    
    UnmapViewOfFile(FileBuf);
    return TRUE;
}

BOOL ReadCacheIP(wchar_t *FileName)
{
    HANDLE hfile = CreateFile(FileName,
            GENERIC_READ,
            FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BY_HANDLE_FILE_INFORMATION file_info;
    GetFileInformationByHandle(hfile, &file_info);
    DWORD FileLength = file_info.nFileSizeLow;
    if(FileLength < sizeof(DataHeader))
        return FALSE;
        
    HANDLE hfilemap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 0, NULL);
    CloseHandle(hfile);

    MyBufferResult = (BYTE*) MapViewOfFile(hfilemap, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(hfilemap);
    
    DataHeader *d_Header = (DataHeader*)MyBufferResult;
    if( d_Header->magic != IINFO_MAGIC )
    {
		UnmapViewOfFile(MyBufferResult);
		DeleteFile(FileName);
        return FALSE;
    }
        
    return TRUE; 
}
BOOL InitCWUBIP()
{
    WryOK = FALSE;

    TCHAR TempIP[MAX_PATH] = {0};
    GetTempPath(MAX_PATH, TempIP);
    wcscat(TempIP,L"CWUB2.IP");
    
    if( CreateCacheIP(TempIP) )
    {
        WryOK = ReadCacheIP(TempIP);
    }
    
    return TRUE;
}

wchar_t * getipwhere (char *ip,char *lan)
{
    static wchar_t buff[1024];
    buff[0] = 0;
    if(!WryOK)
    {
        free(ip);
        free(lan);
        return buff;
    }
    
    DataHeader *d_Header = (DataHeader*)&MyBufferResult[0];

    IPTable * tb1 = (IPTable *)&MyBufferResult[sizeof(DataHeader) + 5 + 8];
    IPTable * tb2 = (IPTable *)&MyBufferResult[d_Header->dic1_start-sizeof(IPTable)] ;
    
    DWORD nowip = str2ip(ip);
    int res = BinarySearch(tb1, tb2, nowip);
    
    if(res>0&&res<0xFFFFFF)
    {
        tb1+=res;
        wchar_t *country = Ansi2Unicode((const char*)&MyBufferResult[tb1->offset1+d_Header->dic1_start]);
        wchar_t *area = Ansi2Unicode((const char*)&MyBufferResult[tb1->offset2+d_Header->dic2_start]);
        wsprintf(buff, L"%s%s", country, area);
        free(area);
        free(country);
        
        if(WryOK2)
        {
            DataHeader2 *d_Header2 = (DataHeader2*)&MyBufferResult2[0];
            DWORD begin = d_Header2->omask;
            DWORD endip = d_Header2->omask;
            endip += (2<<(CountZero(endip)-1)) - 1;
            if(nowip>=begin && nowip<=endip)
            {
                //
                IPTable2 * tbb1 = (IPTable2 *)&MyBufferResult2[sizeof(DataHeader2) + 5 + 8];
                IPTable2 * tbb2 = (IPTable2 *)&MyBufferResult2[d_Header2->dic1_start-sizeof(IPTable2)] ;//
                int lanres = BinarySearch2(tbb1, tbb2, str2ip(lan) & ~d_Header2->smask);
                if(lanres!=-1)
                {
                    tbb1+=lanres;
                    wchar_t *country2 = Ansi2Unicode((const char*)&MyBufferResult2[tbb1->offset1+d_Header2->dic1_start]);
                    wchar_t *area2 = Ansi2Unicode((const char*)&MyBufferResult2[tbb1->offset2+d_Header2->dic2_start]);
                    wcscat(buff,L"/");
                    wcscat(buff,country2);
                    wcscat(buff,area2);
                    free(area2);
                    free(country2);
                }
            }
            
        }
    }
    else
    {
        wcscpy(buff,L"保留地址");
    }
    free(ip);
    free(lan);

	buff[255] = 0;
	trim(buff);
    return buff;
}