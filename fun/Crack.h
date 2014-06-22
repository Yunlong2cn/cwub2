
void ToCrack()
{
	BYTE key10[] = {0x83, 0xF8, 0x02, 0x8B, 0x37, 0x75, 0x3B};
	long AFUtil = MemorySearch(L"AFUtil.dll",key10, sizeof(key10));
	if(AFUtil!=-1)
	{
		BYTE patch[] = {0xE9, 0x81, 0x00, 0x00, 0x00, 0x90};
		WriteProcessMemory(g_process,(void*)(AFUtil+5), &patch, sizeof(patch), NULL);
	}

    BYTE key1[] = {0x83, 0xC4, 0x08, 0x3B, 0xC3, 0x74, 0x75};//83 C4 08 3B C3 74 75
    long MsgMgr = MemorySearch(L"MsgMgr.dll",key1, sizeof(key1));
    if(MsgMgr!=-1)
    {
        BYTE patch = 0xEB;
        WriteProcessMemory(g_process,(void*)(MsgMgr+5), &patch, sizeof(patch), NULL);
    }
    
    BYTE key2[] = {0x3B, 0xC6, 0x74, 0x7A};//3B C6 74 7A
    long AppUtil = MemorySearch(L"AppUtil.dll",key2, sizeof(key2));
    if(AppUtil!=-1)
    {
        BYTE patch = 0xEB;
        WriteProcessMemory(g_process,(void*)(AppUtil+2), &patch, sizeof(patch), NULL);
		//第二次搜索
		AppUtil = MemorySearch(L"AppUtil.dll",key2, sizeof(key2));
		if(AppUtil!=-1)
		{
			BYTE patch = 0xEB;
			WriteProcessMemory(g_process,(void*)(AppUtil+2), &patch, sizeof(patch), NULL);
		}
    }
	//安全提示
	BYTE bSafeTips[] = {0x83, 0xC4, 0x04, 0x3B, 0xC6, 0x0F, 0x84, 0x17};//83 C4 04 3B C6 0F 84 17
	long iSafeTips = MemorySearch(L"AppFramework.dll",bSafeTips, sizeof(bSafeTips));
	if(iSafeTips!=-1)
	{
		unsigned short patch = 0xE990;
		WriteProcessMemory(g_process,(void*)(iSafeTips+5), &patch, sizeof(patch), NULL);
	}
    
    BYTE keySkin[] = {0x85, 0xC0, 0x74, 0x5C, 0xC7, 0x45};//85 C0 74 5C C7 45
    long SkinMgr = MemorySearch(L"SkinMgr.dll",keySkin, sizeof(keySkin));
    if(SkinMgr!=-1)
    {
        BYTE patch = 0xEB;
        WriteProcessMemory(g_process,(void*)(SkinMgr+2), &patch, sizeof(patch), NULL);
    }
    
    BYTE keyCommon[] = {0x84, 0xC0, 0x74, 0x68, 0x8D, 0x8D};//84 C0 74 68 8D 8D
    long Common = MemorySearch(L"Common.dll",keyCommon, sizeof(keyCommon));
    if(Common!=-1)
    {
        BYTE patch = 0xEB;
        WriteProcessMemory(g_process,(void*)(Common+2), &patch, sizeof(patch), NULL);
    }
        
    BYTE key3[] = {0xE9, 0xF2, 0xFE, 0xFF, 0xFF};//E9 F2 FE FF FF
    long ChatFrameApp = MemorySearch(L"ChatFrameApp.dll",key3, sizeof(key3));
    if(ChatFrameApp!=-1)
    {
        BYTE patch = 0xC3;
        WriteProcessMemory(g_process,(void*)(ChatFrameApp-337), &patch, sizeof(patch), NULL);
    }
    
    BYTE key4[] = {0x83, 0x7E, 0x70, 0x00, 0x75, 0x36};//83 7E 70 00 75 36
    long IM = MemorySearch(L"IM.dll",key4, sizeof(key4));
    if(IM!=-1)
    {
        BYTE patch = 0xEB;
        WriteProcessMemory(g_process,(void*)(IM+4), &patch, sizeof(patch), NULL);
    }
    
    BYTE IM1[] = {0x74, 0x1A, 0x8B, 0xCF, 0xFF, 0xD6};//74 1A 8B CF FF D6
    long IM_1 = MemorySearch(L"IM.dll",IM1, sizeof(IM1));
    if(IM_1!=-1)
    {
        BYTE patch = 0xEB;
        WriteProcessMemory(g_process,(void*)(IM_1), &patch, sizeof(patch), NULL);
    }
    
    BYTE key5[] = {0x83, 0xF8, 0x64, 0x75, 0x0B, 0xFF, 0x15};//83 F8 64 75 0B FF 15
    long AppMisc = MemorySearch(L"AppMisc.dll",key5, sizeof(key5));
    if(AppMisc!=-1)
    {
        BYTE patch[8];
        memset(patch,0x90,8);
        WriteProcessMemory(g_process,(void*)(AppMisc+3), &patch, sizeof(patch), NULL);
    }
    
    BYTE AppMisc1[] = {0x74, 0x42, 0x3B, 0xC3, 0x74, 0x3E};//74 42 3B C3 74 3E
    long AppMisc_1 = MemorySearch(L"AppMisc.dll",AppMisc1, sizeof(AppMisc1));
    if(AppMisc_1!=-1)
    {
        BYTE patch[5];
        memset(patch,0x90,5);
        WriteProcessMemory(g_process,(void*)(AppMisc_1-26), &patch, sizeof(patch), NULL);
    }

    BYTE AppMisc2[] = {0x75, 0x5C, 0x6A, 0x65, 0x56, 0x68};//75 5C 6A 65 56 68
    long AppMisc_2 = MemorySearch(L"AppMisc.dll",AppMisc2, sizeof(AppMisc2));
    if(AppMisc_2!=-1)
    {
        unsigned short patch = 0x6DEB;
        WriteProcessMemory(g_process,(void*)(AppMisc_2), &patch, sizeof(patch), NULL);
    }
    
    BYTE key8[] = {0xB9, 0x29, 0x01, 0x00, 0x00, 0x51};//B9 29 01 00 00 51
    long MainFrame = MemorySearch(L"MainFrame.dll",key8, sizeof(key8));
    if(MainFrame!=-1)
    {
        unsigned short patch = 0x0104-10;
        WriteProcessMemory(g_process,(void*)(MainFrame+1), &patch, sizeof(patch), NULL);
    }
    
    BYTE key9[] = {0x89, 0x50, 0x10, 0xC7, 0x40, 0x18};//89 50 10 C7 40 18
    long LoginPanel = MemorySearch(L"LoginPanel.dll",key9, sizeof(key9));
    if(LoginPanel!=-1)
    {
        unsigned short patch = 0x00FA-10;
        WriteProcessMemory(g_process,(void*)(LoginPanel+6), &patch, sizeof(patch), NULL);
    }

	BYTE key12[] = {0xC7, 0x40, 0x18, 0x27, 0x01, 0x00, 0x00};//C7 40 18 27 01 00 00
	LoginPanel = MemorySearch(L"LoginPanel.dll",key12, sizeof(key12));
	if(LoginPanel!=-1)
	{
		unsigned short patch = 0x00FA;
		WriteProcessMemory(g_process,(void*)(LoginPanel+3), &patch, sizeof(patch), NULL);
	}

	BYTE key6[] = {0x66, 0x3D, 0x14, 0x00, 0x0F, 0x85};//66 3D 14 00 0F 85
	BYTE key7[] = {0xC2, 0x04, 0x00, 0x8D, 0x45};//C2 04 00 8D 45
	long SNSApp1 = MemorySearch(L"..\\Plugin\\Com.Tencent.SNSApp\\Bin\\SNSApp.dll",key6, sizeof(key6));
	long SNSApp2 = MemorySearch(L"..\\Plugin\\Com.Tencent.SNSApp\\Bin\\SNSApp.dll",key7, sizeof(key7));
	//DbgPrint(L"%d %d:%d",SNSApp1,SNSApp2,SNSApp2-SNSApp1);
	if(SNSApp1!=-1 && SNSApp2!=-1 && SNSApp2>SNSApp1)
	{
		WriteJMP(SNSApp1 + 4,SNSApp2 + 3);
	}
	

	BYTE keyQQ[] = {0x85, 0xFF, 0x74, 0x5C, 0x8D}; // 85 FF 74 5C 8D
	long QQProtectPatch = MemorySearch(L"QQ.exe",keyQQ, sizeof(keyQQ));
	if(QQProtectPatch!=-1)
	{
		unsigned short patch = 0x9090;
		WriteProcessMemory(g_process,(void*)(QQProtectPatch+2), &patch, sizeof(patch), NULL);
	}
}