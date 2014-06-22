void speak(PVOID pvoid)
{
    static long count = 0;
    if(count<1)
    {
        InterlockedIncrement(&count);
        if(pVoice) pVoice->Speak((const wchar_t*)pvoid, 0, NULL);
        InterlockedDecrement(&count);
    }
    free(pvoid);
}


void QQMsgToText(struct ITXMsgPack *imp,unsigned long QQUIN)
{
    const wchar_t * msg = NULL;
	GetMsgAbstract(&msg,(unsigned long)imp);
    wchar_t* buf1 = (wchar_t *)malloc(sizeof(wchar_t) * (wcslen(msg)+1));
    wcscpy(buf1,msg);


    wchar_t *p1 = replace(buf1, L"[����]", L"");
    wchar_t *p2 = replace(p1, L"[ͼƬ]", L"");
    wchar_t *p3 = replace(p2, L"(ͨ��iPhone QQ���ͣ���������ʣ� http://apple.qq.com/ )", L"");
    wchar_t *p4 = replace(p3, L"(����Ϣ�����ĺ���ͨ���ֻ�QQ���ͣ������ֻ�QQ���¼�� http://mobile.qq.com/c )", L"");
    wchar_t *p5 = replace(p4, L"[Emoticon]", L"");
    wchar_t *p6 = replace(p5, L"[Image]", L"");
    
    wchar_t *text = replace(p6, L"����ʾ�����û�����ʹ��Q+ Web��http://web.qq.com/��", L"");
    
    trim(text);
    
    if(text[0]!=0)
    {
        unsigned int maxText  = GetPrivateProfileInt(L"�����ʶ�", L"�ʶ�����", 32, tzIniPath);
        if(wcslen(text)>maxText) text[maxText]=0;

        const wchar_t * nick = NULL;
        const wchar_t * name = NULL;
        GetPublicName(&name,QQUIN);
        GetNickname(&nick,QQUIN);

		//OutputDebugString(nick);
		//OutputDebugString(name);

        wchar_t format[256];
        GetPrivateProfileString(L"�����ʶ�",L"��Ϣ�ʶ���ʽ", L"%N˵��%M", format, 255, tzIniPath);

        wchar_t *m1 = replace(format, L"%S", nick);
        wchar_t *m2 = replace(m1, L"%N", name);
        wchar_t *m3 = replace(m2, L"%M", text);
        free(m1);
        free(m2);       
        _beginthread(speak,0,(LPVOID)m3);
    }
    free(buf1);
    free(p1);
    free(p2);
    free(p3);
    free(p4);
    free(p5);
    free(p6);
    free(text);
}