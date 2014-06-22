#ifndef __SNAP_WINDOW_H__
#define __SNAP_WINDOW_H__
 
class CSnapWindow
{
public:
	int snap_x;
	int snap_y;
	int offset;
public:
    CSnapWindow();
    void OnWindowPosChanging(LPARAM lParam);
private:
    RECT snap_wn;
    RECT snap_wa;
};
CSnapWindow::CSnapWindow()
{
    //������Լ����ð�
    snap_x = 12;
    snap_y = 12;
    offset = 5;
}
/*
        case WM_WINDOWPOSCHANGING:
            snapHandler.OnWindowPosChanging(lParam);
            break;
*/
void CSnapWindow::OnWindowPosChanging(LPARAM lParam)
{
    WINDOWPOS* lpwndpos = (WINDOWPOS *)lParam;
	//DbgPrint(L"Flag:%X",lpwndpos->flags);

    //��ô��ڴ�С��ǧ�����GetClientRect�����Ҿͱ�����
    GetWindowRect(lpwndpos->hwnd,&snap_wn);
	int shadow = offset;
	//if((lpwndpos->flags&0x20) == 0x20)
	if(IsZoomed(lpwndpos->hwnd))
	{
		//DbgPrint(L"���");
		shadow += 2;
	}
	snap_wn.left += shadow;
	snap_wn.top += shadow;
	snap_wn.right += shadow;
	snap_wn.bottom += shadow;

    snap_wn.right = snap_wn.right - snap_wn.left;
    snap_wn.bottom = snap_wn.bottom - snap_wn.top;
 
    //��ù�������С�����ǳ�ȥ���������������
	SystemParametersInfo( SPI_GETWORKAREA, 0, &snap_wa, 0 );
	//DbgPrint(L"%d,%d,%d,%d",snap_wa.left,snap_wa.top,snap_wa.right,snap_wa.bottom);
	//DbgPrint(L"%d,%d,%d,%d",snap_wn.left,snap_wn.top,snap_wn.right,snap_wn.bottom);
	
    // Left border
    if(lpwndpos->x + shadow >= -snap_x + snap_wa.left && lpwndpos->x + shadow <= snap_x + snap_wa.left) {
        lpwndpos->x = snap_wa.left - shadow;
    }
	 // Right border
	else if(lpwndpos->x + snap_wn.right - shadow  <= snap_wa.right + snap_x && lpwndpos->x + snap_wn.right - shadow  >= snap_wa.right - snap_x) {
		lpwndpos->x = snap_wa.right - snap_wn.right + shadow;
	}
    // Top border
    if(lpwndpos->y + shadow >= -snap_y + snap_wa.top && lpwndpos->y + shadow <= snap_y + snap_wa.top) {
        lpwndpos->y = snap_wa.top - shadow;
    }
    // Bottom border
    else if( lpwndpos->y + snap_wn.bottom - shadow <= snap_wa.bottom + snap_y && lpwndpos->y + snap_wn.bottom - shadow  >= snap_wa.bottom - snap_y) {
        lpwndpos->y = snap_wa.bottom - snap_wn.bottom + shadow;
    }
	//DbgPrint(L"%d,%d",lpwndpos->x,lpwndpos->y);
}
 
#endif //__SNAP_WINDOW_H__