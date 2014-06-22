#ifndef __QQDEF_H__
#define __QQDEF_H__


class CTXStringW;
struct ITXMsgPack;

#pragma comment(lib, "lib\\KernelUtil.lib")
#pragma comment(lib, "lib\\AppUtil.lib")
#pragma comment(lib, "lib\\Common.lib")

#define DLLIMPORT __declspec (dllimport)

namespace Registry
{
    namespace API
    {
        DLLIMPORT int __cdecl GetRegSubKeyBoolField(wchar_t *,wchar_t *,wchar_t *,int *,enum __MIDL___MIDL_itf_IRegistry_0000_0003,enum __MIDL___MIDL_itf_IRegistry_0000_0004);
    }
}
namespace Util
{
    namespace ChatSession
    {
        //DLLIMPORT int __cdecl CreateChatFrameType(unsigned long,int,struct IAFChatFrameType * *,struct ITXData *);
        //DLLIMPORT void __cdecl WriteMsgTipInChatSession(unsigned long,int,enum IconIndex,class CTXStringW,int);
        DLLIMPORT long __cdecl SendAutoReplyMsgToBuddy(unsigned long);
        DLLIMPORT void __cdecl OpenContactChatSession(unsigned long,struct ITXData *);
        DLLIMPORT int __cdecl IsChatSessionOpen(unsigned long,int);
    }
    namespace Misc
    {
        DLLIMPORT void __cdecl SwitchUser(void);
        DLLIMPORT int __cdecl IsServerControlBitOn(unsigned long);
        DLLIMPORT int __cdecl IsTencentTrusted(wchar_t const *);
        //DLLIMPORT int __cdecl MessageBoxExByHwnd(struct HWND__ *,wchar_t *,wchar_t *,unsigned int,struct ITXData *,struct ITXData * *);
        //DLLIMPORT int __cdecl MessageBoxExByHwnd(struct HWND__ *,wchar_t *,wchar_t *,unsigned int,struct ITXData *,struct ITXData * *);
    }
    namespace AFChatSession
    {
        DLLIMPORT void __cdecl WriteMsgTipInChatSession(struct IAFChatFrameType *,enum IconIndex,class CTXStringW,int);
    }
    namespace Contact
    {
        DLLIMPORT int __cdecl IsFlagValid(unsigned long,unsigned long);
        DLLIMPORT int __cdecl IsNewFlagValid(unsigned long,unsigned long);
        DLLIMPORT int __cdecl IsVASFlagValid(unsigned long,unsigned char);
        DLLIMPORT unsigned long __cdecl IsVASFlagValidEx(unsigned long,unsigned char,unsigned int);
        DLLIMPORT long __cdecl IsMaskFlagValid(unsigned long,unsigned char,unsigned long,int &);
        
        DLLIMPORT int __cdecl IsStranger(unsigned long);
        DLLIMPORT unsigned long __cdecl GetSelfUin(void);
        DLLIMPORT unsigned short __cdecl GetIMVersion(unsigned long);
        DLLIMPORT unsigned short __cdecl GetStatus(unsigned long);
        DLLIMPORT class CTXStringW __cdecl GetNickname(unsigned long);
        DLLIMPORT class CTXStringW __cdecl GetPublicName(unsigned long);
        
        namespace Self
        {
            DLLIMPORT int __cdecl GetAutoRelyContent(class CTXStringW &);
        }
    }
    namespace ChatSession
    {
        DLLIMPORT struct HWND__ * __cdecl GetContactChatSessionMainHWnd(unsigned long);
    }
    namespace Group
    {
        DLLIMPORT int __cdecl CheckMsgImage(struct ITXMsgPack *,class CTXStringW &);
    }
    namespace Msg
    {
        DLLIMPORT class CTXStringW __cdecl GetMsgAbstract(struct ITXMsgPack *);
    }
    namespace Convert
    {
        DLLIMPORT int __cdecl ConvertXMLStrToTXData(wchar_t *,wchar_t * *,struct ITXData *,wchar_t *);
    }
    namespace Boot
    {
        DLLIMPORT int __cdecl InitPluginCoreConfig(wchar_t *);
    }
    namespace Com
    {
        DLLIMPORT long __stdcall CreateObjectFromDllFile(wchar_t const *,struct _GUID const &,struct _GUID const &,void * *,struct IUnknown *);
    }
    namespace CoreCenter
    {
        DLLIMPORT int __cdecl EnablePlugin(wchar_t *);
        DLLIMPORT int __cdecl DisablePlugin(wchar_t *);
    }
}

namespace Version
{
    DLLIMPORT unsigned long __cdecl GetBuildVer(void);
    DLLIMPORT unsigned char __cdecl GetMajorVer(void);
    DLLIMPORT unsigned char __cdecl GetMinorVer(void);
    DLLIMPORT unsigned long __cdecl GetPubNo(void);
    DLLIMPORT int __cdecl Init(void);
}

namespace FS
{
    DLLIMPORT int __cdecl IsFileExist(wchar_t const *);
}

//DLLIMPORT void __cdecl TXLog_DoTXLogVW(struct tagLogObj *,wchar_t const *,wchar_t const *,char *);

//typedef unsigned long (WINAPI *GetAccountName)(struct ITXMsgPack *,unsigned long);
//GetAccountName GetNickname = NULL;
//GetAccountName GetPublicName = NULL;

//typedef unsigned long (WINAPI *MsgAbstract)(struct ITXMsgPack *,struct ITXMsgPack *);
//MsgAbstract GetMsgAbstract = NULL;


typedef void (WINAPI *MyGetAccountName)(const wchar_t **,unsigned long);
#define GetNickname ((MyGetAccountName)Util::Contact::GetNickname)
#define GetPublicName ((MyGetAccountName)Util::Contact::GetPublicName)
#define GetMsgAbstract ((MyGetAccountName)Util::Msg::GetMsgAbstract)

#endif // __QQDEF_H__
