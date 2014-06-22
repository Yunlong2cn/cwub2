

#pragma pack(1) //保证按照1字节对齐
#define IINFO_MAGIC2 0x189ED81C

struct DataHeader2
{
    DWORD magic;
    DWORD omask;//外网掩码
    DWORD smask;//子网掩码
    DWORD crc32;
    
    DWORD dic1_start; //字典1 
    DWORD dic2_start; //字典2 
};
struct IPTable2
{
    DWORD begin;//开始IP
    DWORD endip;//开始IP
    DWORD offset1;//区域偏移
    DWORD offset2;//地址偏移
};
#pragma pack(4)

BYTE* MyBufferResult2;
BOOL WryOK2;

DWORD CountZero( long v)
{
	static const DWORD MultiplyDeBruijnBitPosition[32] = 
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	return MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531U)) >> 27]; 
}

int BinarySearch2(IPTable2 * tb1,IPTable2 * tb2,DWORD ip)
{
    int low = 0;
    int high = (tb2 - tb1) ;
    while (low <= high)
    {
        int mid = (low + high) / 2;
        if ( ip >= tb1[mid].begin && ip <= tb1[mid].endip )
        {
            return mid;
        }
        else if ( ip < tb1[mid].begin ) high = mid - 1;
        else low = mid + 1;
    }
    //printf("%d %d\n",high,low);
    return -1;
}

BOOL InitCWUBLan()
{
    WryOK2 = FALSE;
    HANDLE hfile = CreateFile(tzQQWry2,
            GENERIC_READ,
            FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BY_HANDLE_FILE_INFORMATION file_info;
    GetFileInformationByHandle(hfile, &file_info);
    DWORD FileLength2 = file_info.nFileSizeLow;
    if(FileLength2 < sizeof(DataHeader2))
        return FALSE;
        
    HANDLE hfilemap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 0, NULL);
    CloseHandle(hfile);

    BYTE *FileBuf2 = (BYTE*) MapViewOfFile(hfilemap, FILE_MAP_READ, 0, 0, 0);
    CloseHandle(hfilemap);
    
    DataHeader2 *d_Header2 = (DataHeader2*)FileBuf2;
    int datalen = sizeof(DataHeader2) + 5 + 8;
    if( d_Header2->magic != IINFO_MAGIC2 || CRC32_MEM(FileBuf2 + datalen, FileLength2 - datalen) != d_Header2->crc32 )
    {
		UnmapViewOfFile(FileBuf2);
        return FALSE;
    }
    
    UInt32 outSize = 0; 
    BYTE *outStream = 0;
    
    if(LZMADec(FileBuf2+sizeof(DataHeader2),FileLength2 - sizeof(DataHeader2),&outStream,&outSize,datalen)==LZMA_RESULT_OK)
    {
		memcpy(outStream,(BYTE*)FileBuf2,datalen);
		MyBufferResult2 = outStream;
		WryOK2 = TRUE;
    }
    UnmapViewOfFile(FileBuf2);
    
    
    return TRUE;
}