#pragma once

#include <windows.h>
#include <tchar.h>
#include <process.h>
#include <WININET.h>
#include <shlwapi.h>
#include <stdio.h>
#include <shlobj.h>
#include <sapi.h>
#include <time.h>
#include <uxtheme.h>
#include <vssym32.h>

//char USER_AGENT[]= "CWUB/1.299";
//char CWUB_SERVE[]= "shuax.sinaapp.com";
WCHAR WUSER_AGENT[]= L"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)";


#define WM_MYHOTKEY (WM_APP+32)
//#define WM_RESET (WM_USER+1024)
#define WM_SHOWTIPS (WM_USER+1024)

#define FADE_IN 1
#define FADE_OUT 2
#define SWITCH 3
#define TQUERY 4

HINSTANCE g_hModule;
HANDLE g_process = (HANDLE)-1;
ISpVoice * pVoice = NULL;

bool voice_contact;
bool voice_self;
bool voice_group;
bool voice_wnd;
bool oneminits;

//bool CheckSigned;

bool AppBox;
bool Banber;
bool Skins;
bool Savelog;
int version;

bool wndf;
bool memc;
bool lvip;

//int update_model;
//bool firstrun;
int preLen;

bool g_hwnd_inner;
bool g_hwnd_left;
bool g_hwnd_top;

bool isOpenQQShow;
int QQShow_index;
int isBossKey = 0;

bool ShowHideicon;
//bool ShowSelficon;
bool IsForceImageView;

bool ShowHelpBtn;
bool ShowFaceBtn;

bool ShowSafeEntry;
bool ShowUpdateEntry;
bool ShowLevelBtn;
bool ShowAddBtn;

SYSTEMTIME EnterTime;
HFONT g_hFont;
HFONT g_hFont_xp;
HFONT g_hFont_w7;
COLORREF showclr;
bool isGlow;
HTHEME hButtonTheme = (HANDLE)-1;

#pragma warning(disable:4996)
#pragma warning(disable:4800)
#pragma warning(disable:4804)
#pragma warning(disable:4244)

/*
void DbgPrint(const wchar_t *format, ...)
{
	va_list argptr;
	TCHAR buffer[1024];
	va_start(argptr, format);
	_vsnwprintf(buffer, 1024, format, argptr);
	va_end(argptr);

	OutputDebugString(buffer);
}
*/
/*
int free_count;
int mall_count;
void Myfree(void* mem)
{
	free_count++;
	DbgPrint(L"%d,%d,%d",mall_count,free_count,mall_count-free_count);
	free(mem);
}

void * Mymalloc(int x)
{
	mall_count++;
	DbgPrint(L"%d,%d,%d",mall_count,free_count,mall_count-free_count);
	return malloc(x);
}
#define free Myfree
#define malloc Mymalloc

*/
CRITICAL_SECTION lock_conv;
char *ip2str(struct in_addr in)
{
	EnterCriticalSection(&lock_conv);
	static char buf[16];
	sprintf(buf,"%u.%u.%u.%u", in.S_un.S_un_b.s_b1, in.S_un.S_un_b.s_b2, in.S_un.S_un_b.s_b3, in.S_un.S_un_b.s_b4);
	LeaveCriticalSection(&lock_conv);
	return buf;
}
/*

unsigned long mystr2ip(const char *cp)
{
	//OutputDebugString(L"mystr2ip");
	
	int ret;
	static in_addr ipaddr;

	ret = sscanf(cp,"%u.%u.%u.%u", &ipaddr.S_un.S_un_b.s_b4, &ipaddr.S_un.S_un_b.s_b3, &ipaddr.S_un.S_un_b.s_b2, &ipaddr.S_un.S_un_b.s_b1);
	if (ret != 4) ipaddr.s_addr =  -1;

	
	return ipaddr.s_addr;
}
*/

#define inet_ntoa ip2str
//#define inet_addr mystr2ip

//#define SWAP16(x) ((((x) & 0xff) << 8) | (((x) >> 8) & 0xff))
//#define SWAP32(x) ((SWAP16((x) & 0xffff) << 16) | (SWAP16((x) >> 16)))
//typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
//typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

//LPALPHABLEND lpAlphaBlend = NULL;
//PGradientFill lpGradientFill  = NULL;

//#include "alg\sha1.h"
typedef unsigned   uint32_t;
#include "alg\CRC.h"
#include "alg\LZMA.h"
#include "alg\FHD.h"

#include "lib\arraylist.h"
#include "lib\hook.h"
#include "lib\Init.h"
#include "lib\misc.h"
#include "lib\qqdef.h"
#include "lib\qqwrap.h"
#include "lib\split.h"


#include "fun\UrlFilter.h"
#include "fun\BossKey.h"
#include "fun\localvip.h"
#include "fun\ipprobe.h"
#include "fun\Miscellaneous.h"
#include "fun\CreateProcess.h"
#include "fun\Thread.h"

#include "view\showtips.h"
#include "view\sapi.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "wininet.lib")
//#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.lib")


// 声明导出函数
#pragma comment(linker, "/EXPORT:vSetDdrawflag=_My_Fun1,@1")
#pragma comment(linker, "/EXPORT:AlphaBlend=_My_Fun2,@2")
#pragma comment(linker, "/EXPORT:DllInitialize=_My_Fun3,PRIVATE")
#pragma comment(linker, "/EXPORT:GradientFill=_My_Fun4,@4")
#pragma comment(linker, "/EXPORT:TransparentBlt=_My_Fun5,@5")

// 函数实现,写成宏,简化代码量
#define IMPL_STUB_FUNC(n) \
	DWORD g_dwFunPtr##n=0; \
	extern "C" void _declspec(naked) My_Fun##n() \
{ \
	__asm jmp DWORD PTR[g_dwFunPtr##n] \
}

// 实现5个跳板函数
IMPL_STUB_FUNC(1);
IMPL_STUB_FUNC(2);
IMPL_STUB_FUNC(3);
IMPL_STUB_FUNC(4);
IMPL_STUB_FUNC(5);

#define INIT_STUB_FUNC(n,name) \
	g_dwFunPtr##n = (DWORD)GetProcAddress(hDll,name);


// 加载系统dll,初始化函数指针
void LoadSysDll()
{
	TCHAR szDLL[MAX_PATH+1];
	GetSystemDirectory(szDLL,MAX_PATH);
	lstrcat(szDLL,TEXT("\\msimg32.dll"));

	HINSTANCE hDll=LoadLibrary(szDLL);
	if (hDll!=NULL)
	{
		INIT_STUB_FUNC(1,"vSetDdrawflag");
		INIT_STUB_FUNC(2,"AlphaBlend");
		INIT_STUB_FUNC(3,"DllInitialize");
		INIT_STUB_FUNC(4,"GradientFill");
		INIT_STUB_FUNC(5,"TransparentBlt");

		//lpAlphaBlend = (LPALPHABLEND)g_dwFunPtr2;
	}
}

/*
#pragma comment(linker,"/EXPORT:IPCServiceMain=FlashService2.IPCServiceMain,PRIVATE")
#pragma comment(linker,"/EXPORT:StartIPCService=FlashService2.StartIPCService,PRIVATE")
#pragma comment(linker,"/EXPORT:StopIPCService=FlashService2.StopIPCService,PRIVATE")
*/
