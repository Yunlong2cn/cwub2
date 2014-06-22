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
		wcscpy(ip, L"尚未探测对方地址");
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
		wcscpy(time, L"尚未探测");
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
		wcscpy(time, L"尚未探测");
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

		if (wcscmp(tzTime, MYQQ) != 0) //判断是否今日地址
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
			if (wcslen(buff1) >= wcslen(buff2)) wcscpy(buff2, buff1);//保留最长地址
			wcscat(address, buff2);
		}
		else
		{
			wcscat(address, tzWanIP);
		}
	}
	else
	{
		wcscpy(address, L"尚未探测，发图即可探测");
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
		wcscpy(address, L"尚未查询");
	}

	if(wcscmp(tzWanIP, L"尚未探测对方地址") != 0 &&( wcscmp(address, L"查询失败") == 0 ||  wcscmp(address, L"尚未查询")==0 ) )
	{
		//重新查询
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
            wcscpy(address, L"数据库未安装");
			return;
		}
	}
	
	wcscpy(address, L"尚未查询");
}
void QueryInfo(unsigned long QQUIN, wchar_t *tzTips, int mode)
{
	return;
	TCHAR tzNumber[256];
	wsprintf(tzNumber, L"%u", QQUIN);
	
	TCHAR tzVersion[256];
	GetVersion(QQUIN, tzVersion);
    /*
	if(wcsstr(tzVersion,L"未知版本"))
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
	case 1:GetPrivateProfileString(L"提示格式", L"框上格式", L"%D", tzTips, 1023, tzIniPath);break;
	case 2:GetPrivateProfileString(L"提示格式", L"框内格式", L"%V | %W | %L", tzTips, 1023, tzIniPath);break;
	case 3:GetPrivateProfileString(L"提示格式", L"复制格式", L"对方号码：%U\r\n对方版本：%V\r\n公网地址：%W\r\n内网地址：%L\r\n获得时间：%T\r\n本地查询：%B\r\n在线查询：%Z", tzTips, 1023, tzIniPath);break;
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
	case 1:GetPrivateProfileString(L"提示格式", L"左下角格式1", L"%V", BarAddress, 255, tzIniPath);break;
	case 2:GetPrivateProfileString(L"提示格式", L"左下角格式2", L"%D", BarAddress, 255, tzIniPath);break;
	case 3:GetPrivateProfileString(L"提示格式", L"右上角格式1", L"%W", BarAddress, 255, tzIniPath);break;
	case 4:GetPrivateProfileString(L"提示格式", L"右上角格式2", L"%D", BarAddress, 255, tzIniPath);break;
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
