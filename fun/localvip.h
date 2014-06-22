/*
bool isCtlIcon(int icon)
{
	BYTE key[] = {0,2,3,4,5,7,14,16,19,20,21,23,24,26,29,30,32,33,34,39,40,41,43,46,47,48,49,50,53,55,56,59,65,67,78,81,103,104,106,108,109,110,111,115,125,130,135,136,140,142,145,148,149,150,154,158};
	for(int i=0;i<sizeof(key);i++)
	{
		if(icon==key[i]) return true;
	}
	return false;
}
*/
// 本地会员
typedef int (__cdecl * MYISFLAGVALID) (unsigned long, unsigned long);
MYISFLAGVALID VipOldProc = NULL;
int __cdecl MyIsFlagValid(unsigned long QQUIN, unsigned long Flag)
{
	if ( Flag == 4 && lvip && QQUIN == Util::Contact::GetSelfUin() && !isEndWith(GetCallStack(&QQUIN),L"MsgMgr.dll"))
	{
		//DbgPrint(L"LocalVIP");
		return 1;
	}
	//if(ShowSelficon && QQUIN == Util::Contact::GetSelfUin()) return 1;

	// 调用原来的函数 
	if(VipOldProc) return VipOldProc(QQUIN, Flag);
	
	return 0;
}

// 获取状态
typedef unsigned short (__cdecl * MYGETSTATUS) (unsigned long);
MYGETSTATUS GetStatus = NULL;

unsigned long spec = 0;
unsigned short __cdecl MyGetStatus(unsigned long QQUIN)
{
	unsigned short res = 0;
	if(GetStatus) res = GetStatus(QQUIN);

	if(QQUIN == Util::Contact::GetSelfUin() || QQUIN == spec)
	{
        return res;
	}
	
	if(res==20) //离线
	{
        spec = QQUIN;
        if(Util::Contact::GetIMVersion(QQUIN)!=0 && MyGetStatus(Util::Contact::GetSelfUin())!=20)
        {
            //DbgPrint(L"GetSelfUin %d",MyGetStatus(QQUIN));
			HWND hWnd = Util::ChatSession::GetContactChatSessionMainHWnd(QQUIN);
            if(IsWindow(hWnd) && IsWindowVisible(hWnd))
            {
                SendMessage(hWnd, WM_SHOWTIPS+1, (WPARAM)L"对方似乎是隐身状态。",0);
            }
        }
        spec = 0;
        
        if(isEndWith(GetCallStack(&QQUIN),L"SNSApp.dll"))
        {
            res = 10;
        }
        

	}
	
	return res;
}

// 获取服务器状态
typedef int (__cdecl * QQIsServerControlBitOn) (unsigned long);
QQIsServerControlBitOn OldIsServerControlBitOn = NULL;

int __cdecl IsServerControlBitOn(unsigned long FLAG)
{
	switch(FLAG)
	{
		//case 120:	return 0;
		case 131:	return 0;//QQ秀
		case 163:	return 1;//升级
		//case 166:	return 1;
	}
	
	int ret = 0;
	//if(isEndWith(GetCallStack(&QQUIN),L"QQShow.dll")) return 0;
	if(OldIsServerControlBitOn) ret = OldIsServerControlBitOn(FLAG);
	

	//DbgPrint(L"%d:%d %s",FLAG,ret,GetCallStack(&FLAG));
	return ret;
}

//伪造版本号
void ChangeVersion(void)
{
	BYTE code1[] ={0xB8,0xED,0x64,0x00,0x00,0xC3};
    WriteProcessMemory(g_process, (LPVOID)Version::GetPubNo, &code1, sizeof(code1), NULL);

	if(version==2)
	{
		BYTE code2[] ={0xB0,0x26,0x90,0x90,0x90,0xC3};
		WriteProcessMemory(g_process, (LPVOID)Version::GetMajorVer, &code2, sizeof(code2), NULL);
    
		code2[1]= 0x2F;
		WriteProcessMemory(g_process, (LPVOID)Version::GetMinorVer, &code2, sizeof(code2), NULL);
	}
}

typedef int (__cdecl * QQIsMaskFlagValid) (unsigned long,unsigned char,unsigned long,int &);
QQIsMaskFlagValid OldIsMaskFlagValid = NULL;
long __cdecl IsMaskFlagValid(unsigned long qq,unsigned char p2,unsigned long p3,int &p4)
{
	long res = 0;
	if(OldIsMaskFlagValid)
	{
		res = OldIsMaskFlagValid(qq,p2,p3,p4);
		if(ShowHideicon && qq != Util::Contact::GetSelfUin())
		{
			p4 = 0;
		}
	}
	return res;
}
/*
typedef int (__cdecl * QQIsVASFlagValid) (unsigned long,unsigned char);
QQIsVASFlagValid  OldIsVASFlagValid = NULL;
int __cdecl IsVASFlagValid(unsigned long qq,unsigned char ico)
{
	int res = 0;
	if(OldIsVASFlagValid)
	{
		res = OldIsVASFlagValid(qq,ico);
		if(ShowSelficon && ico!=58 && qq == Util::Contact::GetSelfUin())
		{
			//DbgPrint(L"ico:%d,%d",ico,res);
			if(res==0 && isCtlIcon(ico)) res = 1;
		}
	}
	return res;
}

typedef int (__cdecl * QQIsVASFlagValidEx) (unsigned long,unsigned char,unsigned int);
QQIsVASFlagValidEx  OldIsVASFlagValidEx = NULL;
int __cdecl IsVASFlagValidEx(unsigned long qq,unsigned char ico,unsigned int p3)
{
	int res = 0;
	if(OldIsVASFlagValidEx)
	{
		res = OldIsVASFlagValidEx(qq,ico,p3);
		if(ShowSelficon && qq == Util::Contact::GetSelfUin())
		{
			//DbgPrint(L"Exico:%d,%d",ico,res);
			//DbgPrint(L"%d",ico);
			if(res==0 && isCtlIcon(ico)) res = 1;
		}
	}
	return res;
}
*/
BOOL isBase64(const wchar_t* str)
{
	wchar_t* string = wcsdup(str);
	trim(string);

	int length = lstrlen(string);
	if( length>1024 || (length + 4)%4!=0 )
	{
		free(string);
		return FALSE;
	}
	for(int i =0;i<length;i++)
	{
		if(!iswalnum(string[i]) && string[i]!= L'+' && string[i]!= L'-' && string[i]!= L'=' )
		{
			free(string);
			return FALSE;
		}
	}
	
	free(string);
	return TRUE;
}
/*
;此处为聊天加密所使用的密钥，如果密钥为空则不会开启加密功能。
;需要加密传输的内容，请在文字头部添加"#"号。程序会自动尝试使用密钥解密。
聊天加密=SHUAX
*/

typedef void (__cdecl * SetClientTransProc) (DWORD p1,wchar_t *p2,DWORD p3,DWORD p4,DWORD p5);
SetClientTransProc QQClientTransProc = NULL;
void __cdecl ClientTransProc(DWORD p1,wchar_t *p2,DWORD p3,DWORD p4,DWORD p5)
{
	if(p4>1)
	{
		if( GetTimeDiff()<10 && p2[0] == L'#' ) 
		{
			wchar_t* str = Jiami(p2 + 1);
			p4 = wcslen(str);
			QQClientTransProc(p1,p4<1024?str:p2,p3,p4,p5);
			free(str);
			return;
		}
		if( isStartWith(p2,L"[密]") )
		{
			wchar_t *str = replace(p2, L"[密]", L" ");
			p4 = wcslen(str);
			QQClientTransProc(p1,str,p3,p4,p5);
			free(str);
			return;
		}
		/*
		if( isStartWith(p2,L"[密]") )
		{
			wchar_t* str = wcsdup(p2+3);
			p4-=3;
			QQClientTransProc(p1,str,p3,p4,p5);
			free(str);
			return;
		}
		*/
		if( isBase64(p2) )
		{
			//DbgPrint(L"decode base64");
			wchar_t* str = Jiemi(p2);
			p4 = wcslen(str);
			QQClientTransProc(p1,str,p3,p4,p5);
			free(str);
			return;
		}
	}
	if(QQClientTransProc) QQClientTransProc(p1,p2,p3,p4,p5);
}



typedef int (__stdcall * QQImageView) (DWORD p1,wchar_t *p2);
QQImageView ImageView = NULL;
int __cdecl MyImageView(DWORD p1,wchar_t *p2)
{
	DWORD ret = (DWORD)ShellExecute(NULL, L"open", p2, NULL, NULL, SW_SHOWNORMAL);
	if(ret<32)
	{
		if(ImageView) return ImageView(p1,p2);
	}

	return 0;
}
void LocalVIP()
{
    if(version) ChangeVersion();

	VipOldProc = (MYISFLAGVALID)InstallHook((DWORD)Util::Contact::IsFlagValid, (DWORD)MyIsFlagValid);
	GetStatus = (MYGETSTATUS)InstallHook((DWORD)Util::Contact::GetStatus, (DWORD)MyGetStatus);
	OldIsServerControlBitOn = (QQIsServerControlBitOn)InstallHook((DWORD)Util::Misc::IsServerControlBitOn, (DWORD)IsServerControlBitOn);


	//OldIsVASFlagValid = (QQIsVASFlagValid)InstallHook((DWORD)Util::Contact::IsVASFlagValid, (DWORD)IsVASFlagValid);
	//OldIsVASFlagValidEx = (QQIsVASFlagValidEx)InstallHook((DWORD)Util::Contact::IsVASFlagValidEx, (DWORD)IsVASFlagValidEx);
	
	wchar_t LiaoTian[256];
	if(GetPrivateProfileString(L"其它功能",L"聊天加密", L"", LiaoTian, 255, tzIniPath))
	{
		FHD_CTX fhd_ctx;
		FHD_Init(&fhd_ctx);
		FHD_Update(&fhd_ctx, (BYTE*)LiaoTian, wcslen(LiaoTian)*sizeof(wchar_t));
		FHD_Final((BYTE*)btea_k, &fhd_ctx);

		BYTE searchkey[] = {0x33, 0xC0, 0x3B, 0xC8, 0x74, 0x26};//33 C0 3B C8 74 26
		long KernelUtil = MemorySearch(L"KernelUtil.dll",searchkey, sizeof(searchkey));
		if(KernelUtil!=-1)
		{
			if( *(DWORD*)(KernelUtil-57) == 0x6AEC8B55 )
			{
				//DbgPrint(L"Found! %X",KernelUtil-57);
				QQClientTransProc = (SetClientTransProc)InstallHook(KernelUtil-57,(DWORD)ClientTransProc);
			}
		}
	}

	//图片查看器
	if( !IsForceImageView )
	{
		BYTE imagekey[] = {0x8B, 0x40, 0x14, 0x33, 0xD2, 0xB9};//8B 40 14 33 D2 B9
		long imageview = MemorySearch(L"AppMisc.dll",imagekey, sizeof(imagekey));

		//DbgPrint(L"%X",imageview);
		if(imageview!=-1)
		{
			//
			//DbgPrint(L"%X",*(DWORD*)(imageview+26));
			if( *(unsigned short*)(imageview+26) == 0x8B55 )
			{
				ImageView = (QQImageView)InstallHook(imageview+26,(DWORD)MyImageView);
			}
		}
	}
}
