
#include "../fun/plugin.h"
#include "../view/SnapWIndow.h"
CSnapWindow snapHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct CTXBSTR
{
	int zero;//貌似要一直是0
	int count;//引用计数
	int len1;
	int len2;
	wchar_t str[4100];
} TXStr, * pTXStr;

typedef int (__cdecl * MsgBoxShow)(struct HWND__ *,wchar_t *,wchar_t *,unsigned int,struct ITXData *,struct ITXData * *);
MsgBoxShow MessageBoxExByHwnd = NULL;

typedef int (__cdecl * CreateChatFrameType)(DWORD, unsigned long, DWORD *, DWORD);        //创建提示窗口
CreateChatFrameType CreateChatFrame = NULL;

typedef void (__cdecl * WriteMsgTipInChatSession)(DWORD, DWORD, const wchar_t *, DWORD);    //写入提示
typedef void (__cdecl * WriteMsgTipInChatSession2)(DWORD, DWORD, DWORD, const wchar_t *, DWORD);    //写入提示
WriteMsgTipInChatSession WriteMsgTip = NULL;
WriteMsgTipInChatSession2 WriteMsgTip2 = NULL;
DWORD CreateMsgTipPack = NULL;

typedef long (__cdecl * MYRawCreateGFElementByXtml) (wchar_t *,DWORD,DWORD,DWORD);
MYRawCreateGFElementByXtml RawCreateGFElementByXtml = NULL;

typedef int (__cdecl * QQGetAutoRelyContent) (wchar_t ** pmsg); //设置消息
QQGetAutoRelyContent GetAutoRelyContent = NULL;

typedef int (__cdecl * GetRegSubKeyBoolField) (wchar_t *,wchar_t *,wchar_t *,int *,DWORD ,DWORD);
GetRegSubKeyBoolField GetRegBool = NULL;

typedef int (__cdecl * MYConvertXMLStrToTXData) (wchar_t *,wchar_t * *,struct ITXData *,wchar_t *);
MYConvertXMLStrToTXData ConvertXMLStrToTXData = NULL;

typedef unsigned long (__cdecl* MYGetTextColor)(unsigned long);
MYGetTextColor QQGetTextColor = NULL;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int QQMsgBox(HWND hWnd,const wchar_t* lpText,const wchar_t* lpCaption,UINT UType)
{
    if(MessageBoxExByHwnd)
    {
        return MessageBoxExByHwnd(hWnd,(wchar_t*)lpText,(wchar_t*)lpCaption,UType,0,0);
    }
    return 0;
}

DWORD TimePatchBak;

void QQMsgInFrame(unsigned long QQUIN,LPCTSTR lpText,bool Time = true)
{
	HWND hWnd = Util::ChatSession::GetContactChatSessionMainHWnd(QQUIN);
	if(!IsWindow(hWnd) || !g_hwnd_inner) return;    //避免无效窗口

	TXStr MyMsg;
	MyMsg.zero = 0;
	MyMsg.count = 3;
	wcsncpy(MyMsg.str, lpText,4096);
	MyMsg.str[4096]=0;
	MyMsg.len1 = MyMsg.len2 = wcslen(MyMsg.str) * sizeof(wchar_t);

	if(MyMsg.len1<=0) return;//不显示空文本
    
    if(!Time)
    {
		DWORD add = (DWORD)&lpText[0];
		::ReadProcessMemory( g_process, (LPVOID)CreateMsgTipPack, &TimePatchBak, sizeof(DWORD), NULL);
		::WriteProcessMemory(g_process, (LPVOID)CreateMsgTipPack, &add, sizeof(DWORD), NULL);

		BYTE JMPSHORT = 0xEB;
		::WriteProcessMemory(g_process, (LPVOID)(CreateMsgTipPack+256), &JMPSHORT, sizeof(JMPSHORT), NULL);

		MyMsg.len1 = MyMsg.len2 = 0;
		MyMsg.str[0] = 0;
    }

	if(WriteMsgTip&&CreateChatFrame)
	{
		DWORD pointer = 0;//0x11111111;
		CreateChatFrame(QQUIN, 0, &pointer, 0);
		WriteMsgTip(pointer, 0, MyMsg.str, Savelog);
	}
	else
	{
		if(WriteMsgTip2)
		{
			WriteMsgTip2(QQUIN, 0, 0, MyMsg.str,Savelog);
		}
	}
	
	if(!Time)
	{
		::WriteProcessMemory(g_process, (LPVOID)CreateMsgTipPack, &TimePatchBak, sizeof(DWORD), NULL);
		BYTE JMPSHORT = 0x74;
		::WriteProcessMemory(g_process, (LPVOID)(CreateMsgTipPack+256), &JMPSHORT, sizeof(JMPSHORT), NULL);
    }
}

long __cdecl MyRawCreateGFElementByXtml(wchar_t  *p1,DWORD p2,DWORD p3,DWORD p4)
{
	//DbgPrint(L"%s",p1);
	//DbgPrint(L"%s",GetCallStack(&p1));
	
	if(AppBox==0 && wcsstr(p1,L"sp_RichPage")!=0) return 0;
	if(AppBox==0 && wcsstr(p1,L"AppBoxEntryBtn")!=0) return 0;

	if((isBossKey || (wndf && oneminits)) && wcsstr(p1,L"PopupTip.xml")!=0) return 0;

	if(RawCreateGFElementByXtml) return RawCreateGFElementByXtml(p1,p2,p3,p4);
	return 0;
}
/*
TXStr *tempmsg;
bool isMySend;
int __cdecl MyGetAutoRelyContent(wchar_t **pmsg)
{
	static BYTE bak[] = {0x00,0x00,0x00,0x00,0x00};
	static int length = 5;

	void* ptr = (LPVOID)((DWORD)Util::ChatSession::SendAutoReplyMsgToBuddy + 0x3BE);
	if(isMySend) //PATCH
	{
		if(*(BYTE*)ptr==0xE8)
		{
			BYTE dwFill[10];
			memset(dwFill, 0x90, 10);
			::ReadProcessMemory( g_process, ptr, bak, length, NULL);
			::WriteProcessMemory(g_process, ptr, &dwFill, length, NULL);
		}

		*pmsg=(wchar_t *)tempmsg->str;
		return 1;
	}
	else 
	{
		if(bak[0]==0xE8 && *(BYTE*)ptr==0x90) //BAK
		{
			::WriteProcessMemory(g_process, ptr, &bak, length, NULL);
		}
	}

	if(GetAutoRelyContent) return GetAutoRelyContent(pmsg);
	return 0;
}
long __cdecl SendMsg(unsigned long QQUIN,const wchar_t *lpText)
{
	if(GetAutoRelyContent)
	{
		TXStr MyMsg;
		MyMsg.zero = 0;
		MyMsg.count = 3;
		wcsncpy(MyMsg.str, lpText,4096);
		MyMsg.str[4096]=0;
		MyMsg.len1 = MyMsg.len2 = wcslen(MyMsg.str) * sizeof(wchar_t);

		tempmsg = &MyMsg;

		isMySend = true;
		long res = Util::ChatSession::SendAutoReplyMsgToBuddy(QQUIN);
		isMySend = false;

		return res;
	}

	return 0;
}
*/
int __cdecl MyGetRegBool(wchar_t *p1,wchar_t *p2,wchar_t *p3,int *p4,DWORD p5,DWORD p6)
{
	if(wcscmp(p3,L"bSendMsgByEnter")==0)
	{
		GetLocalTime(&EnterTime);
		x = GetTime(EnterTime);
		rand32();
	}
	if(isBossKey)
	{
		if(wcscmp(p3,L"bEnableAllSound")==0
		|| wcscmp(p3,L"bAutoPopupChatWnd")==0
		|| wcscmp(p3,L"bEnableShakeWindowTip")==0
			)
		{
			int res = GetRegBool?GetRegBool(p1,p2,p3,p4,p5,p6):0;
			*p4 = 0;
			return res;
		}
	}
	
	if(wcscmp(p3,L"bExpandPlugFrameEx")==0)
	{
		int res = GetRegBool?GetRegBool(p1,p2,p3,p4,p5,p6):0;

		if(!QQShow_index) *p4 = 0;
		else
		{
			if(PluginList.data[QQShow_index].enable==0) *p4 = 0;
		}

		isOpenQQShow = *p4;
		return res;
	}
	if(GetRegBool) return GetRegBool(p1,p2,p3,p4,p5,p6);
	return 0;
}

int MyConvertXMLStrToTXData(wchar_t *p1,wchar_t * *p2,struct ITXData *p3,wchar_t *p4)
{
	//OutputDebugStringW(p1);
	if(wcsstr(p1,L"default_gtb")!=0)
	{
		char key[]={2,12,15,16,17,18,19};
		for(unsigned int i=0;i<sizeof(key);i++)
		{
			wchar_t filter[256];
			wsprintf(filter,L"<b id=\"%d\" visible=\"1\" privilege=\"1\" />",key[i]);
			wchar_t * str = replace(p1, filter,L"");
			wcscpy(p1,str);
			free(str);
		}
	}
	
	/*
	if(wcsstr(p1,L"SecurityTips")!=0)
	{
		wchar_t tzTips[32];
		if( !GetPrivateProfileString(L"框内提示", L"安全提示", L"交谈中请勿轻信汇款、中奖信息、陌生电话，勿使用外挂软件。", tzTips, 31, tzIniPath) )
		{
			wcscpy(tzTips,L" ");
		}
		tzTips[28]=0;
		wchar_t * str = replace(p1, L"交谈中请勿轻信汇款、中奖信息、陌生电话，勿使用外挂软件。",tzTips);
		wcscpy(p1,str);
		free(str);
	}
	*/
	/*
	if(wcsstr(p1,L"MenuSeverControl")!=0)
	{
		wchar_t * str = replace(p1, L"Text=\"我的QQ中心\" LinkUrl=\"http://ptlogin2.qq.com/qqid?sid=5&amp;ptlang=$LANG$&amp;clientuin=$UIN$&amp;clientkey=$KEY$",L"Text=\"幽香2官网\" LinkUrl=\"http://www.shuax.com/cwub/");
		wcscpy(p1,str);
		free(str);
	}
	*/

	if(wcsstr(p1,L"QQLoginingPanelBottom")!=0)
	{
		return 0;
	}

	if(!Banber && wcsstr(p1,L"LoginLogoConfig")!=0)
	{
		return 0;
	}
	if(!Skins  && wcsstr(p1,L"<Skins>")!=0)
	{
		return 0;
	}
	//
	if(ConvertXMLStrToTXData) return ConvertXMLStrToTXData(p1,p2,p3,p4);
	return 0;
}

unsigned long __cdecl MyGetTextColor(unsigned long QQUIN)
{
	if(QQUIN==Util::Contact::GetSelfUin())
    {
        return RGB(0x20,0xA0,0xFF);
    }
    if(QQGetTextColor) return QQGetTextColor(QQUIN);
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int (__cdecl * QQCreateTXData)(struct ITXData * *);
QQCreateTXData CreateTXData = NULL;

typedef int (__cdecl * QQChatFrameEvent)(unsigned long,int CFChatType,int CFEventType,struct ITXData *);
QQChatFrameEvent ChatFrameEvent = NULL;

typedef long (WINAPI * QQPut1)(struct ITXData *,wchar_t *,int);
typedef long (WINAPI * QQPut2)(struct ITXData *,wchar_t *,wchar_t *);
typedef long (WINAPI * QQBool)(struct ITXData *,wchar_t *,int);
QQPut1 Put1 = NULL;//0xE4
QQPut2 Put2 = NULL;//0xE8
QQBool Bool = NULL;//0xCC

struct ITXData
{

};
void ShowUpBar(const DWORD QQ,const wchar_t *lpText)
{
	if(CreateTXData&&ChatFrameEvent)
	{
		TXStr MyMsg;
		MyMsg.zero = 0;
		MyMsg.count = 3;
		wcsncpy(MyMsg.str, lpText,4096);
		MyMsg.str[4096]=0;
		MyMsg.len1 = MyMsg.len2 = wcslen(MyMsg.str) * sizeof(wchar_t);

		if(MyMsg.len1<=0) return;//不显示空文本

		ITXData *txdata;
		CreateTXData(&txdata);
		
		
        Put1 = (QQPut1)*(DWORD *)(*(DWORD *)txdata + 0xE4);
        Put2 = (QQPut2)*(DWORD *)(*(DWORD *)txdata + 0xE8);
        Bool = (QQBool)*(DWORD *)(*(DWORD *)txdata + 0xCC);

        if(Put1&&Put2&&Bool)
        {
            Put1(txdata,L"nIconIndex", 20);
            Put2(txdata,L"strText", MyMsg.str);
            //Bool(txdata,L"bAutoLink", 0);
            Bool(txdata,L"bCloseButton", 0);
            Put1(txdata,L"nLifetime", 60*1000);
            ChatFrameEvent(QQ, 0, 101, txdata);
		}

	}
}
void QQWrap()
{
	HookLib AppUtil;
	HookLib AFUtil;
	HookLib GF;
	HookLib Common;


	AppUtil.Load(L"AppUtil.dll");
	AFUtil.Load(L"AFUtil.dll");
	GF.Load(L"GF.dll");
	Common.Load(L"Common.dll");

	//
	CreateTXData = (QQCreateTXData)Common.GetFunc("?CreateTXData@Data@Util@@YAHPAPAUITXData@@@Z");
	ChatFrameEvent = (QQChatFrameEvent)AppUtil.GetFunc("?ChatFrameEvent@ChatSession@Util@@YAHKW4CFChatType@ChatFrame@@W4CFEventType@4@PAUITXData@@@Z");
	
	/*
	Put1 = (QQPut1)GF.GetFunc("?Put@Metadata@Util@@YAJPAUITXData@@PA_WH@Z");
	Put2 = (QQPut2)GF.GetFunc("?Put@Metadata@Util@@YAJPAUITXData@@PA_W1@Z");
	Bool = (QQBool)GF.GetFunc("?PutBOOL@Metadata@Util@@YAJPAUITXData@@PA_WH@Z");
	DbgPrint(L"%X,%X,%X",Put1,Put2,Bool);
	*/
	// 对话框
	MessageBoxExByHwnd = (MsgBoxShow)AFUtil.GetFunc("?MessageBoxExByHwnd@Misc@Util@@YAHPAUHWND__@@PA_W1IPAUITXData@@PAPAU4@@Z");
	if(MessageBoxExByHwnd==0)
	{
		MessageBoxExByHwnd = (MsgBoxShow)AppUtil.GetFunc("?MessageBoxExByHwnd@Misc@Util@@YAHPAUHWND__@@PA_W1IPAUITXData@@PAPAU4@@Z");
	}

	// 聊天窗口写字
	CreateChatFrame = (CreateChatFrameType)AppUtil.GetFunc("?CreateChatFrameType@ChatSession@Util@@YAHKHPAPAUIAFChatFrameType@@PAUITXData@@@Z");
	WriteMsgTip = (WriteMsgTipInChatSession)AFUtil.GetFunc("?WriteMsgTipInChatSession@AFChatSession@Util@@YAXPAUIAFChatFrameType@@W4IconIndex@@VCTXStringW@@H@Z");
	CreateMsgTipPack = (DWORD)AFUtil.GetFunc("?CreateMsgTipPack@AFChatSession@Util@@YAHW4IconIndex@@VCTXStringW@@PAPAUITXMsgPack@@@Z");
	if(!CreateChatFrame || !WriteMsgTip)
	{
		snapHandler.offset = 0;
		WriteMsgTip2 = (WriteMsgTipInChatSession2)AppUtil.GetFunc("?WriteMsgTipInChatSession@ChatSession@Util@@YAXKHW4IconIndex@@VCTXStringW@@H@Z");
        CreateMsgTipPack = (DWORD)AppUtil.GetFunc("?CreateMsgTipPack@ChatSession@Util@@YAHKHW4IconIndex@@VCTXStringW@@PAPAUITXMsgPack@@@Z");
	}
	CreateMsgTipPack += 273;
	
	DWORD IsVideoFile = (DWORD)AFUtil.GetFunc("?IsVideoFile@Misc@Util@@YAHVCTXStringW@@@Z");
	if(!IsVideoFile)
	{
		IsVideoFile = (DWORD)AppUtil.GetFunc("?IsVideoFile@Misc@Util@@YAHVCTXStringW@@@Z");
	}
	if(IsVideoFile)
	{
		BYTE ret0[] = {0x31,0xC0,0xC3};
		WriteProcessMemory(g_process,(void*)IsVideoFile, &ret0, sizeof(ret0), NULL);
	}


	RawCreateGFElementByXtml = (MYRawCreateGFElementByXtml)InstallHook((DWORD)GF.GetFunc("?RawCreateGFElementByXtml@GF@Util@@YAJPA_WPAPAUIGFElement@@PAU3@0H@Z"),(DWORD)MyRawCreateGFElementByXtml);
	if(RawCreateGFElementByXtml==0)
	{
		RawCreateGFElementByXtml = (MYRawCreateGFElementByXtml)InstallHook((DWORD)GF.GetFunc("?RawCreateGFElementByXtml@GF@Util@@YAJPA_WPAPAUIGFElement@@PAU3@0@Z"),(DWORD)MyRawCreateGFElementByXtml);
	}
	
	//自动回复
	//GetAutoRelyContent = (QQGetAutoRelyContent)InstallHook((DWORD)Util::Contact::Self::GetAutoRelyContent,(DWORD)MyGetAutoRelyContent);
	//isMySend = false;
	
	//设置QQ秀状态
	GetRegBool = (GetRegSubKeyBoolField)InstallHook((DWORD)Registry::API::GetRegSubKeyBoolField,(DWORD)MyGetRegBool);
	
	//XML2TXData
	ConvertXMLStrToTXData = (MYConvertXMLStrToTXData)InstallHook((DWORD)Util::Convert::ConvertXMLStrToTXData, (DWORD)MyConvertXMLStrToTXData);
	
	//插件过滤
	PluginOldProc = (MYINITPLUGIN)InstallHook((DWORD)Util::Boot::InitPluginCoreConfig, (DWORD)MyInitPlugin);
	
	//自定义群颜色
	QQGetTextColor = (MYGetTextColor)InstallHook((DWORD)AFUtil.GetFunc("?GetTextColor@CUserFlags@Contact@Util@@SAKK@Z"), (DWORD)MyGetTextColor);
}