#include "Uxtheme.h"
#pragma comment(lib, "Uxtheme.Lib")

HWND g_hCtrlView;

#define IDC_BUTTON1                     1001
#define IDC_BUTTON2                     1002
#define IDC_BUTTON3                     1003
#define IDC_BUTTON4                     1004
#define IDC_BUTTON5                     1005


int OrgEnablePlugin(wchar_t *path)
{
    if(PluginOldProc) PluginOldProc(path);
    return MyEnablePlugin(path);
}
int OrgDisablePlugin(wchar_t *path)
{
    //if(PluginOldProc) PluginOldProc(path);
    return MyDisablePlugin(path);
}

void AddItem(bool p1,const wchar_t *p2,const wchar_t *p3,const wchar_t *p4,int p5)
{
    LV_ITEM lvItem = {0};
	lvItem.mask = LVIF_COLUMNS | LVIF_GROUPID | LVIF_TEXT;
	lvItem.iGroupId = p5;

    lvItem.iItem = ListView_GetItemCount(g_hCtrlView);
    ListView_InsertItem(g_hCtrlView,&lvItem);

	lvItem.mask = LVIF_TEXT;
    lvItem.iSubItem = 0;
	lvItem.pszText = (wchar_t *)p2;
    ListView_SetItem(g_hCtrlView,&lvItem);

    lvItem.iSubItem++;
	lvItem.pszText = (wchar_t *)p3;
    ListView_SetItem(g_hCtrlView,&lvItem);

    lvItem.iSubItem++;
	lvItem.pszText = (wchar_t *)p4;
    ListView_SetItem(g_hCtrlView,&lvItem);

    ListView_SetCheckState(g_hCtrlView,lvItem.iItem,p1);
}

void SetDefCheck(int i)
{
	int index = PluginList.data[i].index;
	if(index!=-1)
	{
		//DbgPrint(L"%d,%d %s",i,index,plugin_map[index].path);
		ListView_SetCheckState(g_hCtrlView,i,plugin_map[index].suggestion!=1);
	}
	else ListView_SetCheckState(g_hCtrlView,i,1);
}
/*
int GetLev(int Enabled)
{
    int lev = (int)::abs(Enabled/8.0+0.5);
    return 5-lev%6;
}
*/

int ShowControlMenu(HWND hwnd)
{
    //鼠标坐标
    POINT pt;
    GetCursorPos(&pt);

    //
    HMENU hMenu = CreatePopupMenu();

    AppendMenu(hMenu, MF_BYPOSITION, WM_APP+1, _T("启用"));
    AppendMenu(hMenu, MF_BYPOSITION, WM_APP+2, _T("禁用"));
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_BYPOSITION, WM_APP+3, _T("推荐"));

    //显示菜单
    SetForegroundWindow(hwnd);
    int id = TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
    
    DestroyMenu(hMenu);
    return id-WM_APP;
}
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {
		DWORD dwStyle = ::GetWindowLong(hwndDlg, GWL_STYLE);
		DWORD dwExStyle = ::GetWindowLong(hwndDlg, GWL_EXSTYLE);
		//dwExStyle &= ~ WS_EX_APPWINDOW;
		//dwExStyle |= WS_EX_TOOLWINDOW;
		//::SetWindowLong(hwndDlg, GWL_EXSTYLE, dwExStyle);

		RECT r = {0,0,600,400};
		AdjustWindowRectEx(&r,dwStyle,0,dwExStyle);

		RECT w;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &w, 0);
		int width = r.right - r.left;
		int height = r.bottom - r.top;
		int x = w.left + ((w.right - w.left) / 2) - (width / 2);
		int y = w.top + ((w.bottom - w.top) / 2) - (height / 2);
		
		SetWindowPos(hwndDlg, 0, x, y, width, height, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER);
		SetWindowText(hwndDlg,L"幽香2 插件管理器");

        wchar_t tips[1024];
		int Enabled = 0;
		for(int p=0;p<PluginList.length;p++)
		{
			if(PluginList.data[p].enable) Enabled++;
		}
        wsprintf(tips,L"当前插件共%d款，其中启用%d款。打勾项为启用插件。微博和广告插件的设置需要您手动编辑配置文件。",PluginList.length,Enabled);


		HWND hStatic = ::CreateWindowEx(0 ,
			WC_STATIC,
			tips,
			WS_CHILD|WS_VISIBLE,
			10, 12, 700, 22,
			hwndDlg, NULL, NULL, NULL);
		SendMessage(hStatic,   WM_SETFONT,   (WPARAM)g_hFont,   MAKELPARAM(TRUE,0));


		HWND hBtn;
		const wchar_t szName[][5]=
		{
			L"全部启用",
			L"全部禁用",
			L"推荐设置",
		};
		for(int i=0;i<3;i++)
		{
			hBtn= ::CreateWindowEx(0 ,
				WC_BUTTON,
				szName[i],
				WS_CHILD|WS_VISIBLE,
				10+90*i, 400-40, 75, 30,
				hwndDlg, HMENU(IDC_BUTTON3+i), NULL, NULL);
			SendMessage(hBtn,   WM_SETFONT,   (WPARAM)g_hFont,   MAKELPARAM(TRUE,0));
		}
		hBtn= ::CreateWindowEx(0 ,
			WC_BUTTON,
			L"确定",
			WS_CHILD|WS_VISIBLE,
			600 - 10 - 75 - 90, 400-40, 75, 30,
			hwndDlg, HMENU(IDC_BUTTON1), NULL, NULL);
		SendMessage(hBtn,   WM_SETFONT,   (WPARAM)g_hFont,   MAKELPARAM(TRUE,0));
		hBtn= ::CreateWindowEx(0 ,
			WC_BUTTON,
			L"取消",
			WS_CHILD|WS_VISIBLE,
			600 - 10 - 75, 400-40, 75, 30,
			hwndDlg, HMENU(IDC_BUTTON2), NULL, NULL);
		SendMessage(hBtn,   WM_SETFONT,   (WPARAM)g_hFont,   MAKELPARAM(TRUE,0));

		//
		g_hCtrlView = ::CreateWindowEx(WS_EX_CLIENTEDGE ,
			WC_LISTVIEW,
			L"",
			WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS,
			10, 40, 600-20, 310,
			hwndDlg, NULL, NULL, NULL);
		//

		if(IsVistaSystem()) SetWindowTheme(g_hCtrlView, L"Explorer", NULL);
		ListView_SetExtendedListViewStyle(g_hCtrlView,LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES|LVS_EX_DOUBLEBUFFER|LVS_EX_HEADERDRAGDROP|
			LVS_EX_LABELTIP|LVS_EX_SIMPLESELECT|LVS_EX_BORDERSELECT);
        LV_COLUMN lvc;
        lvc.mask = LVCF_TEXT | LVCF_WIDTH;
        int index=0;

        lvc.cx = 85;
        lvc.pszText = (wchar_t *)L"插件名称";
        ListView_InsertColumn(g_hCtrlView,index++,&lvc);

        lvc.cx = 135;
        lvc.pszText = (wchar_t *)L"插件路径";
        ListView_InsertColumn(g_hCtrlView,index++,&lvc);
        
        lvc.cx = 330;
        lvc.pszText = (wchar_t *)L"插件描述";
        ListView_InsertColumn(g_hCtrlView,index++,&lvc);
		
		ListView_EnableGroupView(g_hCtrlView,true);
		LVGROUP lgrp = {0};
		lgrp.mask = LVGF_HEADER | LVGF_GROUPID;
		if(IsVistaSystem())
		{
			lgrp.cbSize = sizeof(LVGROUP);
			lgrp.mask |= LVGF_STATE;
			lgrp.state = LVGS_COLLAPSIBLE;
		}
		else
		{
			lgrp.cbSize = LVGROUP_V5_SIZE;
		}
		LPTSTR extn[] = { _T("建议启用"), _T("建议禁用"), _T("可以禁用") };

		lgrp.iGroupId = 2;
		lgrp.pszHeader = extn[2];
		lgrp.cchHeader = wcslen(extn[2]);
		ListView_InsertGroup(g_hCtrlView,-1,(LPARAM)&lgrp); 

		lgrp.iGroupId = 0;
		lgrp.pszHeader = extn[0];
		ListView_InsertGroup(g_hCtrlView,-1,(LPARAM)&lgrp); 
		
		lgrp.iGroupId = 1;
		lgrp.pszHeader = extn[1];
		ListView_InsertGroup(g_hCtrlView,-1,(LPARAM)&lgrp); 

        for(int p=0;p<PluginList.length;p++)
        {			
            int i = PluginList.data[p].index;
			if(i!=-1) AddItem(PluginList.data[p].enable,plugin_map[i].name,PluginList.data[p].c_str,plugin_map[i].desc,plugin_map[i].suggestion);
			else AddItem(PluginList.data[p].enable,L"未知插件",PluginList.data[p].c_str,L"未找到此插件描述，请根据插件路径自行判断。",2);
        }
		SetFocus(g_hCtrlView);
		//
    }
    return FALSE;

    case WM_CLOSE:
        EndDialog(hwndDlg, 0);
        return TRUE;
    
    case WM_NOTIFY:
        switch (((NMHDR*)lParam)-> code)
        {
            case NM_RCLICK:
                if(((NMHDR*)lParam)->hwndFrom == g_hCtrlView)
                {
                    int menu = ShowControlMenu(hwndDlg);
					int count = ListView_GetSelectedCount(g_hCtrlView);
					if(count)
					{
						int index = -1;
						for(int i=0;i<count;i++)
						{
							index = ListView_GetNextItem(g_hCtrlView,index,LVNI_SELECTED);
							switch(menu)
							{
								case 1:ListView_SetCheckState(g_hCtrlView,index,1);break;
								case 2:ListView_SetCheckState(g_hCtrlView,index,0);break;
								case 3:SetDefCheck(index);break;
							}
						}
                    }
                }
                break;
        }
        return TRUE;

    case WM_COMMAND:
		{
			int count = ListView_GetItemCount(g_hCtrlView);
			switch(LOWORD(wParam))
			{
			case IDC_BUTTON1:
				for(int i=0;i<count;i++)
				{
					bool now = ListView_GetCheckState(g_hCtrlView,i);
					if(PluginList.data[i].enable!=now)
					{
						PluginList.data[i].enable = now;
						if(now)
						{
							OrgEnablePlugin(PluginList.data[i].c_str);
						}
						else OrgDisablePlugin(PluginList.data[i].c_str);
						SetPrivateProfileInt(L"插件管理",PluginList.data[i].c_str, now, configPath);
					}
				}
				EndDialog(hwndDlg, 0);
				return TRUE;
			case IDC_BUTTON2:
				EndDialog(hwndDlg, 0);
				return TRUE;
			}
			for(int i=0;i<count;i++)
			{
				switch(LOWORD(wParam))
				{
				case IDC_BUTTON3:ListView_SetCheckState(g_hCtrlView,i,1);break;
				case IDC_BUTTON4:ListView_SetCheckState(g_hCtrlView,i,0);break;
				case IDC_BUTTON5:SetDefCheck(i);;break;
				}
			}
			SetFocus(g_hCtrlView);
			return TRUE;
		}
    }

    return FALSE;
}