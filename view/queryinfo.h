#ifndef __INFO_H__
#define __INFO_H__

wchar_t * getipwhere (char *ip,char *lan);

void GetInnerIP(unsigned long QQUIN, const wchar_t *Type,wchar_t *ip)
{
	TCHAR MYQQ[64];
	wsprintf(MYQQ, L"%X", QQUIN);

	TCHAR buff[256];

	if ( GetPrivateProfileString(Type, MYQQ, L"", buff, 255, tzCachePath) )
	{
		wcscpy(ip, buff);
	}
	else
	{
		wcscpy(ip, L"��δ̽��Է���ַ");
	}
}
void GetWANIP(unsigned long QQUIN, wchar_t *wanip)
{
    GetInnerIP(QQUIN,L"WAN",wanip);
}

void GetLANIP(unsigned long QQUIN, wchar_t *lanip)
{
	GetInnerIP(QQUIN,L"LAN",lanip);
}

void GetTime(unsigned long QQUIN, wchar_t *time)
{
	TCHAR MYQQ[64];
	wsprintf(MYQQ, L"%X", QQUIN);

	TCHAR buff[256];

	if ( GetPrivateProfileString(L"TIME", MYQQ, L"", buff, 255, tzCachePath) )
	{
		wcscpy(time, buff);
	}
	else
	{
		wcscpy(time, L"��δ̽��");
	}
}

void GetTime2(unsigned long QQUIN, wchar_t *time)
{
	TCHAR MYQQ[64];
	wsprintf(MYQQ, L"%X", QQUIN);

	TCHAR buff[256];

	if ( GetPrivateProfileString(L"TIME", MYQQ, L"", buff, 255, tzCachePath) )
	{
        int year,month,day;
        swscanf(buff,L"%4d%2d%2d",&year,&month,&day);
		wsprintf(time,L"%04d.%02d.%02d",year,month,day);
	}
	else
	{
		wcscpy(time, L"��δ̽��");
	}
}

void GetAddress(unsigned long QQUIN, wchar_t *address)
{
	TCHAR MYQQ[64];
	wsprintf(MYQQ, L"%X", QQUIN);

	TCHAR tzWanIP[256];
	GetPrivateProfileString(L"WAN", MYQQ, L"", tzWanIP, 255, tzCachePath);
	if (tzWanIP[0] != 0)
	{
		wchar_t tzLanIP[256];
		GetPrivateProfileString(_T("LAN"), MYQQ, _T("0.0.0.0"), tzLanIP, 255, tzCachePath);

		TCHAR tzTime[256];
		GetTime(QQUIN, tzTime);

		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);
		wsprintf(MYQQ, _T("%04d%02d%02d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay);

		if (wcscmp(tzTime, MYQQ) != 0) //�ж��Ƿ���յ�ַ
		{
			wcscpy(address, L"*");
		}
		else
		{
			wcscpy(address, L"");
		}


        wchar_t *buff1 = getipwhere(Unicode2Ansi(tzWanIP),Unicode2Ansi(tzLanIP));
        
		TCHAR buff2[256];
		GetPrivateProfileString(L"IP", tzWanIP, L"", buff2, 255, tzQQWryPath);
		if (buff1[0] != 0 || buff2[0] != 0)
		{
			if (wcslen(buff1) >= wcslen(buff2)) wcscpy(buff2, buff1);//�������ַ
			wcscat(address, buff2);
		}
		else
		{
			wcscat(address, tzWanIP);
		}
	}
	else
	{
		wcscpy(address, L"��δ̽�⣬��ͼ����̽��");
	}
}

void GetAddressOnline(unsigned long QQUIN,wchar_t *address)
{
	TCHAR tzWanIP[256];
	GetWANIP(QQUIN, tzWanIP);

	TCHAR buff[256];

	GetPrivateProfileString(L"IP", tzWanIP, L"", buff, 255, tzQQWryPath);
	if (buff[0] != 0)
	{
		wcscpy(address, buff);
	}
	else
	{
		wcscpy(address, L"��δ��ѯ");
	}

	if(wcscmp(tzWanIP, L"��δ̽��Է���ַ") != 0 &&( wcscmp(address, L"��ѯʧ��") == 0 ||  wcscmp(address, L"��δ��ѯ")==0 ) )
	{
		//���²�ѯ
		PARAM *tmp = (PARAM *)malloc(sizeof(PARAM));

		char *ip = Unicode2Ansi(tzWanIP);
		tmp->ip.s_addr = inet_addr(ip);
		tmp->qq = QQUIN;
		tmp->isTips = false;
		free(ip);

		_beginthread(QueryOnline, 0, (LPVOID)tmp);
	}
}

void GetAddressDB(unsigned long QQUIN, wchar_t *address)
{
	TCHAR MYQQ[64];
	wsprintf(MYQQ, L"%X", QQUIN);

	TCHAR tzWanIP[256];
	GetPrivateProfileString(L"WAN", MYQQ, L"", tzWanIP, 255, tzCachePath);
	if (tzWanIP[0] != 0)
	{
		wchar_t tzLanIP[256];
		GetPrivateProfileString(_T("LAN"), MYQQ, _T("0.0.0.0"), tzLanIP, 255, tzCachePath);

		wchar_t *wryaddress = getipwhere(Unicode2Ansi(tzWanIP),Unicode2Ansi(tzLanIP));
		if (wryaddress[0] != 0)
		{
            wcscpy(address, wryaddress);
            return;
		}
		else
		{
            wcscpy(address, L"���ݿ�δ��װ");
			return;
		}
	}
	
	wcscpy(address, L"��δ��ѯ");
}
void QueryInfo(unsigned long QQUIN, wchar_t *tzTips, int mode)
{
	return;
	TCHAR tzNumber[256];
	wsprintf(tzNumber, L"%u", QQUIN);
	
	TCHAR tzVersion[256];
	GetVersion(QQUIN, tzVersion);
    /*
	if(wcsstr(tzVersion,L"δ֪�汾"))
    {
        _beginthread(QueryVersionFromNet,0,(void*)WORD2BB(Util::Contact::GetIMVersion(QQUIN)));
    }
	*/
	TCHAR tzWanIP[256];
	GetWANIP(QQUIN, tzWanIP);

	TCHAR tzLanIP[256];
	GetLANIP(QQUIN, tzLanIP);

	TCHAR tzTime[256];
	GetTime2(QQUIN, tzTime);

	TCHAR tzAddress[256];
	GetAddress(QQUIN, tzAddress);

	TCHAR tzAddress1[256];
	GetAddressOnline(QQUIN, tzAddress1);

	TCHAR tzAddress2[256];
	GetAddressDB(QQUIN, tzAddress2);
	
	const wchar_t * nick = NULL;
	const wchar_t * name = NULL;
	GetPublicName(&name,QQUIN);
	GetNickname(&nick,QQUIN);

	switch(mode)
	{
	case 1:GetPrivateProfileString(L"��ʾ��ʽ", L"���ϸ�ʽ", L"%D", tzTips, 1023, tzIniPath);break;
	case 2:GetPrivateProfileString(L"��ʾ��ʽ", L"���ڸ�ʽ", L"%V | %W | %L", tzTips, 1023, tzIniPath);break;
	case 3:GetPrivateProfileString(L"��ʾ��ʽ", L"���Ƹ�ʽ", L"�Է����룺%U\r\n�Է��汾��%V\r\n������ַ��%W\r\n������ַ��%L\r\n���ʱ�䣺%T\r\n���ز�ѯ��%B\r\n���߲�ѯ��%Z", tzTips, 1023, tzIniPath);break;
	}

	wchar_t *line = replace(tzTips, L"\\n", L"\r\n");
	line = replace2(line, L"%V", tzVersion);
	line = replace2(line, L"%V", tzVersion);
	line = replace2(line, L"%W", tzWanIP);
	line = replace2(line, L"%L", tzLanIP);
	line = replace2(line, L"%T", tzTime);
	line = replace2(line, L"%D", tzAddress);
	line = replace2(line, L"%Z", tzAddress1);
	line = replace2(line, L"%B", tzAddress2);
	line = replace2(line, L"%U", tzNumber);
	line = replace2(line, L"%S", nick);
	line = replace2(line, L"%N", name);

	wcsncpy(tzTips, line, 4096);
	tzTips[4096]=0;
	free(line);
	
	trim(tzTips);
}

void GetBarAddress(unsigned long QQUIN, wchar_t *BarAddress,int mode)
{
	TCHAR tzWanIP[256];
	GetWANIP(QQUIN, tzWanIP);
	
	TCHAR tzAddress[256];
	GetAddress(QQUIN, tzAddress);

	TCHAR tzAddress1[256];
	GetAddressOnline(QQUIN, tzAddress1);

	TCHAR tzAddress2[256];
	GetAddressDB(QQUIN, tzAddress2);

	TCHAR tzVersion[256];
	GetVersion(QQUIN, tzVersion);

	TCHAR tzLanIP[256];
	GetLANIP(QQUIN, tzLanIP);

	switch(mode)
	{
	case 1:GetPrivateProfileString(L"��ʾ��ʽ", L"���½Ǹ�ʽ1", L"%V", BarAddress, 255, tzIniPath);break;
	case 2:GetPrivateProfileString(L"��ʾ��ʽ", L"���½Ǹ�ʽ2", L"%D", BarAddress, 255, tzIniPath);break;
	case 3:GetPrivateProfileString(L"��ʾ��ʽ", L"���ϽǸ�ʽ1", L"%W", BarAddress, 255, tzIniPath);break;
	case 4:GetPrivateProfileString(L"��ʾ��ʽ", L"���ϽǸ�ʽ2", L"%D", BarAddress, 255, tzIniPath);break;
	}

	wchar_t *line = replace(BarAddress, L"%V", tzVersion);
	line = replace2(line, L"%L", tzLanIP);
	line = replace2(line, L"%D", tzAddress);
	line = replace2(line, L"%W", tzWanIP);
	line = replace2(line, L"%Z", tzAddress1);
	line = replace2(line, L"%B", tzAddress2);

	wcsncpy(BarAddress, line, 255);
	BarAddress[255]=0;
	/*
	if((mode+2)%2==0)
	{
		unsigned int length = 17;
		if(BarAddress[0]==L'*') length++;
		if(wcslen(BarAddress)>length) BarAddress[length]=0;
	}
	*/
	/*
	unsigned int length = 0;
	int i = 0;
	if(BarAddress[0]==L'*') i = 1;
	for(;BarAddress[i]!=0;i++)
	{
		wchar_t wch = BarAddress[i];
		if(wch<=0xFF) length++;
		else length+=2;
		if(length>34)
		{
			BarAddress[i] = 0;
			break;
		}
	}
	*/

	free(line);
	
	trim(BarAddress);
}
#endif // __INFO_H__
