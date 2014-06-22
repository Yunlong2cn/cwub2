/*
  HOOKLIB by shuax
  2011.05.30
*/
#pragma once
#ifndef __HOOKLIB_H__
#define __HOOKLIB_H__

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <malloc.h>
#include "HookTable.h"

#pragma pack(1) //保证按照1字节对齐
struct FILL_CODE
{
    BYTE push_code;
    DWORD address;
    BYTE retn_code;
}PUSH_CODE;
#pragma pack(4)

struct Tag_Hook
{
	DWORD orig;
	DWORD det;
};
Tag_Hook TagHook[100];
int HookCount = 0;

void* SaveHook;
int nowLength;

void WritePUSH_RET(DWORD TargetProc, DWORD NewProc)
{
    //写入一个 68 XXXXXXXX C3
    PUSH_CODE.address = NewProc;
    WriteProcessMemory((void*)-1,(LPVOID)TargetProc, &PUSH_CODE, sizeof(PUSH_CODE), NULL);
}
/*
void WriteNOP(DWORD TargetProc,int len)
{
    //填充NOP，方便反汇编观察
    BYTE NOP[] = {0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90};
    WriteProcessMemory((void*)-1,(LPVOID)TargetProc, &NOP, len, NULL);
}

*/

BOOL InitHook()
{
	PUSH_CODE.push_code = 0x68;
	PUSH_CODE.retn_code = 0xC3;
	UInt32 outSize = 0; 
	BYTE *outStream = 0;
	if(LZMADec(HookTable,sizeof(HookTable),&outStream,&outSize,0)==LZMA_RESULT_OK)
	{
		tables = (DWORD*)outStream;
		return TRUE;
	}
	return FALSE;
}

int VAtoFileOffset(void *pModuleBase, void *pVA);
bool AntiHook(DWORD orig,int length,void* bak)
{
	wchar_t stack[MAX_PATH + 1];
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((LPVOID)orig, &mbi, sizeof(mbi));
	GetModuleFileName((HMODULE)mbi.AllocationBase, stack, MAX_PATH);
	

	HMODULE h_module = GetModuleHandle(stack);
	DWORD offset = VAtoFileOffset(h_module,(void*)orig);

	HANDLE hfile = CreateFile(stack,
		GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile != INVALID_HANDLE_VALUE)
	{
		HANDLE hfilemap = CreateFileMapping(hfile, NULL, PAGE_READONLY|SEC_IMAGE, 0, 0, NULL);
		CloseHandle(hfile);

		unsigned char *buf = (unsigned char*) MapViewOfFile(hfilemap, FILE_MAP_READ, 0, 0, 0);
		CloseHandle(hfilemap);
	
		memcpy(bak,buf+offset,length);
		UnmapViewOfFile(buf);

		return true;
	}
	return false;
}

int GetPatchLength(DWORD func_start, void* thunk, int max_need_len=6)
{
	BYTE temp[100];
	if(!AntiHook(func_start,100,temp)) return 0;

	int one_oplen = 0;
	int actual_oplen = 0;
	PBYTE func_temp = (PBYTE)temp;
	while(actual_oplen < max_need_len)
	{
		one_oplen = oplen(func_temp);
		
		if(one_oplen==-1) return 0;
		func_temp += one_oplen;
		actual_oplen += one_oplen;
	}
    
	thunk = (BYTE*)thunk + 1;
    memcpy(thunk,temp,actual_oplen);
    
    actual_oplen = actual_oplen + 1 + max_need_len;
     
	return actual_oplen;
}
void* InstallHook(DWORD orig, DWORD det)
{
    //检查传入参数
	if(orig==0 || det==0) return 0;

	if(HookCount==0)
	{
		SaveHook = VirtualAlloc(NULL,1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);//备份长度、备份指令、JMP指令
	}

    //保存指令
    void* ptr;
    ptr = ((BYTE*)SaveHook) + nowLength;;
    //获得需要处理的长度
    int ThunkLen = GetPatchLength(orig,ptr,sizeof(PUSH_CODE));

	if(ThunkLen == 0) return 0;

	if(HookCount<100)
	{
		TagHook[HookCount].orig = orig;
		TagHook[HookCount].det  = det;
		HookCount++;
	}
	
	nowLength += ThunkLen;
	if( nowLength > 1024 ) return 0;
	
    
    //调到原始地址接着运行
    WritePUSH_RET((DWORD)ptr + ThunkLen - sizeof(PUSH_CODE),orig + ThunkLen - sizeof(PUSH_CODE) - 1);
    
    //写入跳转到新程序
    WritePUSH_RET(orig,det);

    //对多余字节填充NOP
    //if(ThunkLen>13) WriteNOP(orig+6,ThunkLen-13);

    //DbgPrint(L"分配地址：%08X",ptr);
    //::VirtualProtect((LPVOID)oldAddr, sizeof(DWORD), dwOldProtect, 0);
	*(BYTE*)ptr = ThunkLen;
    ptr = (BYTE*)ptr + 1;
    return ptr;
}

/*
void* SafeCall(DWORD orig)
{
    //检查传入参数
	if(orig==0) return 0;

    //获得需要处理的长度
    int MinLen = GetPatchLength(orig,6);

	if(MinLen == 0) return 0;

    //保存指令
    void* ptr;
    ptr = ((BYTE*)SaveHook) + nowLength;
	nowLength += 1+MinLen+5;
	if( nowLength > 4096 ) return 0;
	*(BYTE*)ptr = MinLen;
	ptr = (BYTE*)ptr + 1;
	if(!AntiHook(orig,MinLen,ptr)) return 0;

	//调到原始地址接着运行
	BYTE JMP = 0xE9;
	WriteProcessMemory((void*)-1,(LPVOID)((DWORD)ptr+MinLen), &JMP, sizeof(JMP), NULL);
	DWORD offset = orig - (DWORD)ptr - 5;
	WriteProcessMemory((void*)-1,(LPVOID)((DWORD)ptr+MinLen+1), &offset, sizeof(offset), NULL);


	return ptr;
}
*/
/*
void Uninstallhook(void *ptr)
{
    int MinLen = *((BYTE*)ptr-1);
    DWORD base = * (DWORD*)((BYTE*)ptr+MinLen+1) + 5 + (DWORD)ptr;
    //DbgPrint(L"%X %X",base,(DWORD)ptr);//[776] 7785C43A 6726E1
    WriteProcessMemory((void*)-1,(LPVOID)base, ptr, MinLen, NULL);
    //WriteProcessMemory((void*)-1,(LPVOID)base, ptr, MinLen, NULL);
    //memset(ptr,0xC3,MinLen+5);
    //free();
}
*/
class HookLib
{
private:
    HMODULE m_hModule;  // 原始模块句柄
public:
    void Load(const TCHAR *tzDllPath);
    FARPROC GetFunc(PCSTR pszProcName);
};
void HookLib::Load(const TCHAR *tzDllPath)
{
    m_hModule = NULL;
    m_hModule = GetModuleHandle(tzDllPath);
    if(!m_hModule) m_hModule = LoadLibrary(tzDllPath);
}
FARPROC HookLib::GetFunc(PCSTR pszProcName)
{
    FARPROC fpAddress = GetProcAddress(m_hModule, pszProcName);
    //DbgPrint(L"获取地址：%08X",fpAddress);
    return fpAddress;
}
/*
template <typename Type>
void GetFuncToProc(Type t,const TCHAR *tzPath,PCSTR pszProcName)
{
    union
    {
        DWORD  _addr;
        Type  _t;
    } tmp;
    tmp._t = t;
    
    HookLib temp;
    temp.Load(tzPath);
    tmp._addr = (DWORD)temp.GetFunc(pszProcName);
}

//安装虚函数hook
template <typename Type, typename Class>
DWORD VirtualHook(Type t, Class c,DWORD newAddr)
{
    union
    {
        DWORD   _addr;
        Type  _t;
    } tmp;
    tmp._t = t;
    DWORD* oldAddr = (DWORD*)*(DWORD*)c + (tmp._addr-1)/4;
    DWORD oldFunc = *oldAddr;
    
    DWORD dwOldProtect;
    if(::VirtualProtect((LPVOID)oldAddr, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        DbgPrint(L"%X\n",oldFunc);
        *oldAddr = newAddr;
        ::VirtualProtect((LPVOID)oldAddr, sizeof(DWORD), dwOldProtect, 0);
        return oldFunc;
    }

    
    return (DWORD)InstallHook(oldFunc,newAddr);
    
}

//安装普通成员函数hook
template <typename Type, typename Class>
void* ClassHook(Type t, Class c,DWORD newAddr)
{
    union
    {
        DWORD   _addr;
        Type  _t;
    } tmp;
    tmp._t = t;
    return InstallHook(tmp._addr,newAddr);
}
*/
#endif // __HOOKLIB_H__
