#pragma once
#ifndef __MISC_H__
#define __MISC_H__

#include <tchar.h>

DWORD GetTime(SYSTEMTIME time)
{
	return time.wMilliseconds + time.wSecond * 1000 + time.wMinute * 60 * 1000 + time.wHour * 60 * 60 * 1000;
}
DWORD GetTimeDiff()
{
	SYSTEMTIME  now;
	GetLocalTime(&now);

	return GetTime(now) - GetTime(EnterTime);
}

//#include "WinTrust.h"
/*
void KillConnect(PVOID pvoid)
{
    Sleep(1000*5);
    InternetCloseHandle((HINTERNET)pvoid);
}
*/
void SetWininetTimeout(HINTERNET hConnect)
{
    //_beginthread(KillConnect, 0, (LPVOID)hConnect);
}
bool IsVistaSystem(void) 
{ 
    static int result = -1;
    
    if(result==-1)
    {
        OSVERSIONINFO osvi; 
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        ::GetVersionEx(&osvi); 
        if ( osvi.dwMajorVersion >= 6) result = 1; 
        else result = 0;
    }
    
    return result!=0; 
}

bool IsVistaTheme()
{
	if(!IsVistaSystem()) return false;
	if(hButtonTheme!=(HANDLE)-1) return hButtonTheme!=NULL;
	
	hButtonTheme = OpenThemeData(GetDesktopWindow(), L"BUTTON");
	if(hButtonTheme) CloseThemeData(hButtonTheme);
	return hButtonTheme!=NULL;
}

bool IsSystemWin7()
{
	static int result = -1;

	if(result==-1)
	{
		OSVERSIONINFO osvi; 
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&osvi); 
		if (osvi.dwMajorVersion == 6 &&   
			osvi.dwMinorVersion == 1 ) 
		{
			result = true; 
		}
		else result = false;

	}

	return result!=0; 
}

void WriteJMP(DWORD TargetProc, DWORD NewProc)
{
    BYTE JMP = 0xE9;
    WriteProcessMemory(g_process, (LPVOID)TargetProc, &JMP, sizeof(JMP), NULL);
    DWORD offset = NewProc - TargetProc - 5;
    WriteProcessMemory(g_process, (LPVOID)(TargetProc + 1), &offset, sizeof(offset), NULL);
}


void SetPrivateProfileInt(LPCTSTR lpAppName,LPCTSTR lpKeyName,INT nDefault,LPCTSTR lpFileName)
{
    TCHAR buffer[256];
    wsprintf(buffer,_T("%d"),nDefault);
    WritePrivateProfileString(lpAppName,lpKeyName, buffer, lpFileName);
}

bool isEndWith(const wchar_t *path,const wchar_t* ext)
{
	int len1 = wcslen(path);
	int len2 = wcslen(ext);
	if(len2>len1) return false;
	return !memicmp(path + len1 - len2,ext,len2*sizeof(wchar_t));
}

bool isStartWith(const wchar_t *path,const wchar_t* ext)
{
	int len1 = wcslen(path);
	int len2 = wcslen(ext);
	if(len2>=len1) return false;
	return !memcmp(path,ext,len2*sizeof(wchar_t));
}
/*
inline void GET_DWORD(LPBYTE buf, DWORD * b)
{
    *b  = *buf & 0xff;
    *b <<= 8;
    *b |= *(buf+1)&0xff;
    *b <<=8;
    *b |= *(buf+2)&0xff;
    *b <<= 8;
    *b |= *(buf+3)&0xff;
}
*/
WORD WORD2BB(DWORD wVersion)
{
    return MAKEWORD(wVersion % 100, wVersion / 100);
}

wchar_t * GetCallStack(void *param)
{
    static wchar_t stack[MAX_PATH+1];
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery((LPVOID)*((DWORD*)param-1), &mbi, sizeof(mbi));
    GetModuleFileName((HMODULE)mbi.AllocationBase, stack, MAX_PATH);
    
    //DbgPrint(L"%X",((DWORD*)param-1));
    return stack;
}


/*
bool wcsistr(const wchar_t *str1,const wchar_t *str2)
{
    wchar_t buffer1[1024];
    wchar_t buffer2[1024];
    wcscpy(buffer1,str1);
    wcscpy(buffer2,str2);
    wcslwr(buffer1);
    wcslwr(buffer2);

    return wcsstr(buffer1,buffer2);
}
bool isEndWith(const char *path,const char* ext)
{
    return !stricmp(path+strlen(path)-strlen(ext),ext);
}*/

wchar_t* Ansi2Unicode(const char* szAnsi,int length = -1)
{
    if(length=-1) length = strlen(szAnsi);
    int wcsLen = ::MultiByteToWideChar(936, NULL, szAnsi, length, NULL, 0);
    wchar_t* wszString = (wchar_t*)malloc(sizeof(wchar_t) * (wcsLen + 1));
    ::MultiByteToWideChar(936, NULL, szAnsi, length, wszString, wcsLen);
    wszString[wcsLen] = 0;
    return wszString;
}

char* Unicode2Ansi(const wchar_t* wszString)
{
    int ansiLen = ::WideCharToMultiByte(936, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
    char* szAnsi = (char*)malloc(sizeof(char) * (ansiLen + 1));
    ::WideCharToMultiByte(936, NULL, wszString, wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
    szAnsi[ansiLen] = 0;
    return szAnsi;
}

wchar_t* UTF82Unicode(const char* szUTF8,int length)
{
    int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szUTF8, length, NULL, 0);
    wchar_t* wszString = (wchar_t*)malloc(sizeof(wchar_t) * (wcsLen + 1));
    ::MultiByteToWideChar(CP_UTF8, NULL, szUTF8, length, wszString, wcsLen);
    wszString[wcsLen] = 0;
    return wszString;
}
wchar_t *Jiami(const wchar_t *string)
{
	wchar_t *temp = wcsdup(string);
	long length = wcslen(temp) + 1;

	long len = length/sizeof(DWORD);
	if(len>1)
	{
		btea((DWORD*)temp, len);
	}
	else 
	{
		for(int i=0;i<length;i++)
		{
			temp[i] ^= btea_k[3];
		}
	}
	
	length = length * sizeof(wchar_t) + sizeof(DWORD)*2;
	
	BYTE* buffer = (BYTE*)malloc(length);

	long randkey = rand32() + length;

	memcpy(buffer,&randkey,sizeof(DWORD));

	randkey = (randkey ^ btea_k[0])  + length;
	memcpy(buffer+sizeof(DWORD),&randkey,sizeof(DWORD));

	//DbgPrint(L"len %d %X",length,randkey);

	memcpy(buffer+sizeof(DWORD)*2,temp, length - sizeof(DWORD)*2);
	
	free(temp);


	char* res = (char *)malloc(((length + 2) / 3) * 4 + 1);

	base64_encode(res, (const char*)buffer, length);

	wchar_t* result = Ansi2Unicode(res);

	free(res);
	
	return result;
}

wchar_t *Jiemi(const wchar_t *string)
{
	char* temp = Unicode2Ansi(string);
	long length = strlen(temp);

	BYTE* res = (BYTE *)malloc(length + 1);
	length = base64_decode((char*)res, temp, length);
	//DbgPrint(L"len %d",length);
	DWORD *d_res = (DWORD*)res;
	if(length<10 || (d_res[0] ^ (d_res[1] - length) ) != btea_k[0])
	{
		//DbgPrint(L"decode base64 failed %X",d_res[1]-length);
		//密钥不正确
		free(temp);
		free(res);
		return wcsdup(string);
	}

	length -= sizeof(DWORD)*2;
	wchar_t* res2 = (wchar_t *)malloc(length);
	memcpy(res2,res+sizeof(DWORD)*2,length);

	length = length/sizeof(wchar_t);

	long len = length/sizeof(DWORD);
	if(len>1)
	{
		btea((DWORD*)res2, -len);
	}
	else 
	{
		for(int i=0;i<length;i++)
		{
			res2[i] ^= btea_k[3];
		}
	}


	free(temp);
	free(res);

	wchar_t* result = (wchar_t *)malloc(length*sizeof(wchar_t) + 10);
	wcscpy(result,L"[密]");
	wcscat(result,res2);
	free(res2);

	return result;
}
/*
char* Unicode2UTF8(const wchar_t* wszString)
{
    int utf8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
    char* szUTF8 = (char*)malloc(sizeof(char) * (utf8Len + 1));
    ::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szUTF8, utf8Len, NULL, NULL);
    szUTF8[utf8Len] =  0;
    return szUTF8;
}



bool isInclude(const wchar_t *str, const wchar_t *cmp, int len)
{
    TCHAR buff[256];
    int i, j;
    for (i = 0, j = 0; i < len; i++)
    {
        if (str[i] != 0) buff[j++] = str[i];
        else
        {
            buff[j++] = 0;
            j = 0;
            //
            if (wcsicmp(buff, cmp) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

void trim( char *str )
{
    char *copied;
    char *tail;
    char c;

    if ( str == NULL )
        return;

    for ( copied = str, tail = NULL; ( c = *str ) != 0; str ++ )
    {
        if ( c != ' ' && c != '\t' )
        {
            *copied ++ = c;
            tail = copied;
        }
        else
        {
            if ( tail != NULL )
                *copied ++ = c;
        }
    }

    if ( tail != NULL )
        *tail = 0;
    else
        *str = 0;
}
*/

void trim(wchar_t* str)
{
    if (str == 0)
    {
        wcscpy(str, L"");
        return;
    }
    int len = wcslen(str);

    wchar_t *buf = (wchar_t *)malloc(sizeof(wchar_t) * (len + 10));

    //左端空格过滤
    wchar_t* l = str;
    while (iswspace(*l)) l++;

    //右端空格过滤
    wchar_t* r = str + len - 1;
    while (iswspace(*r)) r--;

    if (r - l + 1 <= 0)
    {
        //全部是空格
        str[0] = 0;
    }
    else
    {
        wcsncpy(buf, l, r - l + 1);
        buf[r-l+1] = 0;//补充0
        wcscpy(str, buf);
    }

    //释放创建的缓存
    free(buf);
}


wchar_t *replace(wchar_t *source,const wchar_t *sub,const wchar_t *rep)
{
    if (0 == source) return wcsdup(L"");
    if (0 == *sub) return wcsdup(source);


    wchar_t *pc1, *pc2;
    const wchar_t *pc3;

    int isub = wcslen(sub);
    int irep = wcslen(rep);
    int isource = wcslen(source);

    wchar_t* result = (wchar_t *)malloc(sizeof(wchar_t) * (( irep > isub ? isource / isub * irep : isource ) + 10) );
    pc1 = result;
    while (*source != 0)
    {
        pc2 = source;
        pc3 = sub;

        while (*pc2 == *pc3 && *pc3 != 0 && *pc2 != 0)
            pc2++, pc3++;
        // 寻找子串
        if (0 == *pc3)
        {
            pc3 = rep;
            //追加替代串到结果串
            while (*pc3 != 0)
                *pc1++ = *pc3++;
            pc2--;
            source = pc2;
        }
        else
            *pc1++ = *source;
        source++;
    }
    *pc1 = 0;
    return result;
}

wchar_t *replace2(wchar_t *source,const wchar_t *sub,const wchar_t *rep)
{
	wchar_t *result = replace(source,sub,rep);
	free(source);
	return result;
}
char *replace(char *source, const char *sub, const char *rep)
{
    if (0 == source) return strdup("");
    if (0 == *sub) return strdup(source);


    char *pc1, *pc2;
    const char *pc3;

    int isub = strlen(sub);
    int irep = strlen(rep);
    int isource = strlen(source);

    char* result = (char *)malloc(sizeof(char) * (( irep > isub ? isource / isub * irep : isource ) + 10) );
    pc1 = result;
    while (*source != 0)
    {
        pc2 = source;
        pc3 = sub;

        while (*pc2 == *pc3 && *pc3 != 0 && *pc2 != 0)
            pc2++, pc3++;
        // 寻找子串
        if (0 == *pc3)
        {
            pc3 = rep;
            //追加替代串到结果串
            while (*pc3 != 0)
                *pc1++ = *pc3++;
            pc2--;
            source = pc2;
        }
        else
            *pc1++ = *source;
        source++;
    }
    *pc1 = 0;
    return result;
}

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

int VAtoFileOffset(void *pModuleBase, void *pVA)
{
    return (DWORD)pVA - (DWORD)pModuleBase;
}
int memstr(BYTE* p, int m,BYTE* s, int n)
{
    long mask;
    int skip;
    int i, j, mlast, w;

    w = n - m;

    if (w < 0)
        return -1;

    /* look for special cases */
    if (m <= 1)
    {
        if (m <= 0)//如果模式串为空
            return -1;
        /* use special case for 1-character strings */
        for (i = 0; i < n; i++)
            if (s[i] == p[0])
                return i;

        return -1;
    }

    mlast = m - 1;

    /* create compressed boyer-moore delta 1 table */
    skip = mlast - 1;
    /* process pattern[:-1] */
    for (mask = i = 0; i < mlast; i++)
    {
        mask |= (1 << (p[i] & 0x1F));
        if (p[i] == p[mlast])
            skip = mlast - i - 1;
    }

    /* process pattern[-1] outside the loop */
    mask |= (1 << (p[mlast] & 0x1F));

    for (i = 0; i <= w; i++)   // w == n - m;
    {
        /* note: using mlast in the skip path slows things down on x86 */
        if (s[i+m-1] == p[m-1])    //(Boyer-Moore式的后缀搜索)
        {
            /* candidate match */
            for (j = 0; j < mlast; j++)
                if (s[i+j] != p[j])
                    break;
            if (j == mlast) /* got a match! */
                return i;
            /* miss: check if next character is part of pattern */
            if (!(mask & (1 << (s[i+m] & 0x1F))))  //(Sunday式的基于末字符的下一字符)
                i = i + m;
            else
                i = i + skip; //(Horspool式的基于末字符)
        }
        else
        {
            /* skip: check if next character is part of pattern */
            if (!(mask & (1 << (s[i+m] & 0x1F))))
                i = i + m;
        }
    }
    return -1;
}


bool CheckPE(void *image)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)image;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
    {
        //不是有效的DOS文件
        return false;
    }
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader+pDosHeader->e_lfanew);//指向pNtHeader的指针
    if ( pNtHeader->Signature != IMAGE_NT_SIGNATURE )
    {
        //不是有效的NT文件
        return false;
    }
    //检查完成
    //OutputDebugStringA("CheckPE");
    return true;
}

long MemorySearch(const wchar_t *path,BYTE* key, int length)
{
    //OutputDebugStringA("MemorySearch");
	/*
    if(CheckSigned && VerifySignature(path)==0)
    {
        //OutputDebugString(path);
        return -1;
    }
	*/
    //OutputDebugStringA("VerifySignature");
    HMODULE m_hModule = GetModuleHandle(path);
    if(!m_hModule) m_hModule = LoadLibrary(path);
    if(m_hModule)
    {
        if(CheckPE(m_hModule))
        {
            PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)m_hModule;
            PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader+pDosHeader->e_lfanew);
            PIMAGE_FILE_HEADER pPEHead=(PIMAGE_FILE_HEADER)&pNtHeader->FileHeader;
            PIMAGE_SECTION_HEADER section = (PIMAGE_SECTION_HEADER)((DWORD)pPEHead + sizeof(IMAGE_FILE_HEADER) + pPEHead->SizeOfOptionalHeader);
            for(int i=0; i<pPEHead->NumberOfSections; i++)
            {
                if(strcmp((const char*)section[i].Name,".text")==0)
                {
                    long res = memstr(key,length,(BYTE*)((DWORD)m_hModule + section[i].PointerToRawData),section[i].SizeOfRawData);
                    if(res!=-1)
                    {
                        //OutputDebugStringA("memstr");
                        //DbgPrint(L"%s %X",path,res+(DWORD)m_hModule + section[i].PointerToRawData);
                        return res+(DWORD)m_hModule + section[i].PointerToRawData;
                    }
                    break;
                }
            }
        }
    }
    return -1;
}
DWORD str2ip(const char *lp)
{
	/*
	OutputDebugString(L"str2ip");
	
	DWORD ipaddr = mystr2ip(lp);
	_asm
	{
		mov     eax,ipaddr
		bswap   eax
		mov     ipaddr,eax
	}
    */
	DWORD ret = 0;
    BYTE now = 0;

    while(*lp)
    {
        if('.' == *lp)
        {
            ret = 256 * ret + now;
            now = 0;
        }
        else
            now = 10 * now + *lp - '0';
        ++lp;
    }
    ret = 256 * ret + now;

    return ret;
}


HBITMAP SCreateCompatibleBitmap(int Width,int Height,void **data)  
{
	void					  *pBits;
	BITMAPINFOHEADER          bmih;   
	ZeroMemory( &bmih, sizeof(BITMAPINFOHEADER));   

	bmih.biSize                 = sizeof (BITMAPINFOHEADER) ;   
	bmih.biWidth                = Width;   
	bmih.biHeight               = IsVistaTheme()?-Height:Height;   
	bmih.biPlanes               = 1 ;   
	bmih.biBitCount             = 32;        //这里一定要是32   
	bmih.biCompression          = BI_RGB ;   
	bmih.biSizeImage            = 0 ;   
	bmih.biXPelsPerMeter        = 0 ;   
	bmih.biYPelsPerMeter        = 0 ;   
	bmih.biClrUsed              = 0 ;   
	bmih.biClrImportant         = 0 ;   

	HBITMAP hBitMap = CreateDIBSection (NULL, (BITMAPINFO *)  &bmih, DIB_RGB_COLORS, &pBits, NULL, 0) ;  
	//memset(pBits,0xFFE6F0FA, * 4);
	*data = pBits;
	DWORD *fill = (DWORD*)pBits;

	if(!IsVistaTheme())
	{
		for(int i=0;i<Width*Height;i++)
		{
			fill[i] = 0xFFFFFFFF;
		}
	}

	return hBitMap;  
}
/*
inline float SquareRootFloat(float number)
{
	long i;
	float x, y;
	const float f = 1.5F;
	x = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;
	i  = 0x5f375a86 - ( i >> 1 );	//注意这一行
	y  = * ( float * ) &i;
	y  = y * ( f - ( x * y * y ) );
	return number * y;
}
inline int _floor(float f)
{
	int a         = *(int*)(&f);
	int sign      = (a>>31);
	int mantissa  = (a&((1<<23)-1))|(1<<23);
	int exponent  = ((a&0x7fffffff)>>23)-127;
	int r         = ((unsigned int)(mantissa)<<8)>>(31-exponent);
	return ((r ^ (sign)) - sign ) &~ (exponent>>31);
}

int CmpColor(DWORD argb,COLORREF colleft)
{
	BYTE *bit = (BYTE *)&argb;

	int absR = GetRValue(colleft) - bit[0];
	int absG = GetGValue(colleft) - bit[1];
	int absB = GetBValue(colleft) - bit[2];

	int flag = abs(absR) + abs(absG) + abs(absB);//_floor(SquareRootFloat( absR*absR + absG*absG + absB*absB));

	
	return (768-flag)/3;
}
*/
HFONT MyCreateFont(wchar_t *font)
{
	//HDC hDC = GetDC(0);
	//int lfHeight = -12;//::MulDiv(nPointSize, ::GetDeviceCaps(hDC, LOGPIXELSY), 72);
	//ReleaseDC(NULL, hDC);
	//DbgPrint(L"%d",lfHeight);
	return CreateFont(-12, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, font);
}

void DrawCWUBText(HDC hDC, LPWSTR szText, COLORREF color,RECT rcArea, DWORD dwTextFlags = DT_LEFT | DT_VCENTER | DT_SINGLELINE)  
{
	if(IsVistaTheme())
	{
		//获取主题句柄  
		HTHEME hThm = OpenThemeData(GetDesktopWindow(), L"TextStyle");
		if(hThm)
		{
			//绘制选项  
			DTTOPTS dttopts = {0};  
			dttopts.dwSize = sizeof(DTTOPTS);  
			dttopts.dwFlags = DTT_TEXTCOLOR | DTT_COMPOSITED | ( isGlow?DTT_GLOWSIZE:0 ) ;  
			dttopts.crText = color;
			if(isGlow)
			{
				dttopts.iGlowSize = 12;
			}

			//绘制文本  
			//RECT rc = {0, 0, rcArea.right - rcArea.left, rcArea.bottom - rcArea.top};  
			HRESULT hr = DrawThemeTextEx(hThm, hDC, TEXT_LABEL, 0, szText, -1, dwTextFlags | DT_END_ELLIPSIS | DT_NOPREFIX, &rcArea, &dttopts);

			CloseThemeData(hThm);
		}
	}
	else
	{
		::SetBkMode(hDC, TRANSPARENT);
		::SelectObject(hDC, g_hFont);
		::SetTextColor(hDC, color);
		::DrawText(hDC,szText,-1,&rcArea, dwTextFlags | DT_END_ELLIPSIS | DT_NOPREFIX);
	}
}  

void DoTrans(unsigned int **pixs,int Width,int Height)
{
	unsigned int *data = *pixs;
	if(IsVistaTheme())
	{
		/*
		int dwsize = sizeof(int)*Width;
		BYTE *buff = (BYTE*)malloc(dwsize);
		for(int i=0;i<Height/2;i++)
		{
			int pt1 = Width*i;
			int pt2 = Width*(Height-i);
			memcpy(buff,&data[pt1],dwsize);
			memcpy(&data[pt1],&data[pt2],dwsize);
			memcpy(&data[pt2],buff,dwsize);
		}
		free(buff);
		*/
	}
	else
	{
		for(int i=0;i<Width*Height;i++)
		{
			if(data[i]==0xFFFFFFFF) data[i] = 0;
			else
			{
				BYTE *bits = (BYTE*)&data[i];
				bits[3] = 0xFF;
			}
		}
	}
}

int GetTransX(unsigned int **pixs,int Width,int Height)
{
	unsigned int *data = *pixs;
	for(int i = Width;i>Width/2;i--)
	{
		for(int j=0;j<Height;j++)
		{			
			if(data[ i + j*Width ] != 0)
			{
				return Width - i;
			}
		}
	}

	return 0;
}
#endif // __MISC_H__
