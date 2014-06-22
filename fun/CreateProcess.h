#include "Skins.h"

typedef BOOL (WINAPI* GCREATEPROCESS)(
					LPCTSTR lpApplicationName,
					LPTSTR lpCommandLine,
					LPSECURITY_ATTRIBUTES lpProcessAttributes,
					LPSECURITY_ATTRIBUTES lpThreadAttributes,
					BOOL bInheritHandles,
					DWORD dwCreationFlags,
					LPVOID lpEnvironment,
					LPCTSTR lpCurrentDirectory,
					LPSTARTUPINFO lpStartupInfo,
					LPPROCESS_INFORMATION lpProcessInformation
					);
GCREATEPROCESS OldCreateProcess = NULL;

BOOL WINAPI MyCreateProcess(
					LPCTSTR lpApplicationName,
					LPTSTR lpCommandLine,
					LPSECURITY_ATTRIBUTES lpProcessAttributes,
					LPSECURITY_ATTRIBUTES lpThreadAttributes,
					BOOL bInheritHandles,
					DWORD dwCreationFlags,
					LPVOID lpEnvironment,
					LPCTSTR lpCurrentDirectory,
					LPSTARTUPINFO lpStartupInfo,
					LPPROCESS_INFORMATION lpProcessInformation
					)
{
	//OutputDebugStringW(lpCommandLine);
	if(wcsstr(lpCommandLine,L"QQExternal.exe")!=0)
	{
		if(wcsstr(lpCommandLine,L"FlashService")==0) return 0;
	}
	if(wcsstr(lpCommandLine,L"auclt.exe")!=0)
	{
		if(wcsstr(lpCommandLine,L"/MU")==0) return 0;
	}
	if(wcsstr(lpCommandLine,L"txupd.exe")!=0)
	{
		if(wcsstr(lpCommandLine,L"/manual")==0) return 0;
	}
	//OutputDebugStringW(lpCommandLine);
    if(OldCreateProcess) return OldCreateProcess(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
    return 0;
}
/*

typedef struct _IO_STATUS_BLOCK
{
	union
	{
		ULONG Status;
		PVOID Pointer;
	};

	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _FILE_NAME_INFORMATION { // Information Classes 9 and 21
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION,
FILE_ALTERNATE_NAME_INFORMATION, *PFILE_ALTERNATE_NAME_INFORMATION;

typedef NTSTATUS (WINAPI *pZwQueryInformationFile)( HANDLE, PVOID, PVOID, ULONG, ULONG );
pZwQueryInformationFile ZwQueryInformationFile = NULL;


*/
typedef LPVOID (WINAPI*  MYMapViewOfFile)(
	__in HANDLE hFileMappingObject, 
	__in DWORD dwDesiredAccess, 
	__in DWORD dwFileOffsetHigh, 
	__in DWORD dwFileOffsetLow, 
	__in SIZE_T dwNumberOfBytesToMap 
	);
MYMapViewOfFile OldMapViewOfFile = NULL;
LPVOID  WINAPI MyMapViewOfFile(
							   __in HANDLE hFileMappingObject, 
							   __in DWORD dwDesiredAccess, 
							   __in DWORD dwFileOffsetHigh, 
							   __in DWORD dwFileOffsetLow, 
							   __in SIZE_T dwNumberOfBytesToMap 
							   )
{
	if(Skin_hMapFile && Skin_hMapFile==hFileMappingObject)
	{
		//DbgPrint(L"%X",hFileMappingObject);
		Skin_hMapFile = NULL;
		LPVOID MapData;
		if(OldMapViewOfFile) MapData = OldMapViewOfFile(hFileMappingObject,FILE_MAP_COPY,dwFileOffsetHigh,dwFileOffsetLow,dwNumberOfBytesToMap);
		if(MapData)
		{
			SkinsPatch((BYTE*)MapData);
			//SkinsPatch((BYTE*)MapData,Skin_length);
			//CloseHandle(hMapFile);
			//UnmapViewOfFile(MapData);
		}
		return MapData;
	}
	if(OldMapViewOfFile) return OldMapViewOfFile(hFileMappingObject,dwDesiredAccess,dwFileOffsetHigh,dwFileOffsetLow,dwNumberOfBytesToMap);
	return 0;
}
typedef HANDLE (WINAPI*  MYCreateFileMapping)(
	HANDLE hFile, //物理文件句柄 
	LPSECURITY_ATTRIBUTES lpAttributes, //安全设置 
	DWORD flProtect, //保护设置 
	DWORD dwMaximumSizeHigh, //高位文件大小 
	DWORD dwMaximumSizeLow, //低位文件大小 
	LPCTSTR lpName //共享内存名称 
	);

MYCreateFileMapping OldCreateFileMapping = NULL;

HANDLE WINAPI MyCreateFileMapping(
								  HANDLE hFile, //物理文件句柄 
								  LPSECURITY_ATTRIBUTES lpAttributes, //安全设置 
								  DWORD flProtect, //保护设置 
								  DWORD dwMaximumSizeHigh, //高位文件大小 
								  DWORD dwMaximumSizeLow, //低位文件大小 
								  LPCTSTR lpName //共享内存名称 
								  )
{
	if(Skin_hFile && Skin_hFile==hFile)
	{
		//DbgPrint(L"%X",Skin_hFile);
		Skin_hFile = 0;
		if(OldCreateFileMapping) Skin_hMapFile = OldCreateFileMapping(hFile,lpAttributes,PAGE_WRITECOPY,dwMaximumSizeHigh,dwMaximumSizeLow,lpName);
		return Skin_hMapFile;
	}

	if(OldCreateFileMapping) return OldCreateFileMapping(hFile,lpAttributes,flProtect,dwMaximumSizeHigh,dwMaximumSizeLow,lpName);
	return 0;
}
typedef HANDLE (WINAPI*  MYCreateFile)(
									   LPCTSTR lpFileName, //指向文件名的指针 
									   DWORD dwDesiredAccess, //访问模式（写/读） 
									   DWORD dwShareMode, //共享模式 
									   LPSECURITY_ATTRIBUTES lpSecurityAttributes, //指向安全属性的指针 
									   DWORD dwCreationDisposition, //如何创建 
									   DWORD dwFlagsAndAttributes, //文件属性 
									   HANDLE hTemplateFile //用于复制文件句柄 
									   );

MYCreateFile OldCreateFile= NULL;

HANDLE WINAPI MyCreateFile(
						   LPCTSTR lpFileName, //指向文件名的指针 
						   DWORD dwDesiredAccess, //访问模式（写/读） 
						   DWORD dwShareMode, //共享模式 
						   LPSECURITY_ATTRIBUTES lpSecurityAttributes, //指向安全属性的指针 
						   DWORD dwCreationDisposition, //如何创建 
						   DWORD dwFlagsAndAttributes, //文件属性 
						   HANDLE hTemplateFile //用于复制文件句柄 
						   )
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if(OldCreateFile) hFile = OldCreateFile(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
	if(!isFindXtml && isEndWith(lpFileName,L".rdb") && wcsstr(lpFileName,L"Xtml") )
	{
		//OutputDebugString(lpFileName);
		//DbgPrint(L"%X",hFile);
		isFindXtml = true;
		Skin_hFile = hFile;
		BY_HANDLE_FILE_INFORMATION file_info;
		GetFileInformationByHandle(hFile, &file_info);
		Skin_length = file_info.nFileSizeLow;
		/*
		HANDLE hFile = NULL;
		if(OldCreateFile) hFile = OldCreateFile(lpFileName,dwDesiredAccess ,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			BY_HANDLE_FILE_INFORMATION file_info;
			GetFileInformationByHandle(hFile, &file_info);

			HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_WRITECOPY, 0, 0, NULL);
			if(hMapFile)
			{
				unsigned char* MapData = (BYTE*)MapViewOfFile(hMapFile, FILE_MAP_COPY, 0, 0, 0);
				
				SkinsPatch(MapData,file_info.nFileSizeLow);

				CloseHandle(hMapFile);
				//UnmapViewOfFile(MapData);
			}
		}

		return hFile;
		*/
	}
	
	return hFile;
}


typedef DWORD (WINAPI* MYFileAttributes)(LPCTSTR lpFileName);
MYFileAttributes OldGetFileAttributes = NULL;

DWORD WINAPI MyFileAttributes(LPCTSTR lpFileName)
{
	if(wcsicmp(lpFileName,tzPath)==0) return 0xFFFFFFFF;
	if(OldGetFileAttributes) return OldGetFileAttributes(lpFileName);
	return 0xFFFFFFFF;
}
void HookCreateProcess()
{
	isFindXtml = false;
	Skin_hMapFile = NULL;
	Skin_hFile = NULL;
	Skin_length = 0;
	OldCreateProcess = (GCREATEPROCESS)InstallHook((DWORD)CreateProcessW, (DWORD)MyCreateProcess);
	//ZwQueryInformationFile = (pZwQueryInformationFile)GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwQueryInformationFile");
	OldMapViewOfFile = (MYMapViewOfFile)InstallHook((DWORD)MapViewOfFile, (DWORD)MyMapViewOfFile);
	OldCreateFileMapping = (MYCreateFileMapping)InstallHook((DWORD)CreateFileMappingW, (DWORD)MyCreateFileMapping);
	OldCreateFile = (MYCreateFile)InstallHook((DWORD)CreateFileW, (DWORD)MyCreateFile);

	//
	OldGetFileAttributes = (MYFileAttributes)InstallHook((DWORD)GetFileAttributesW, (DWORD)MyFileAttributes);
}