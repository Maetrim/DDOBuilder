// SkillsHeaderCtrl.cpp
//

#include "stdafx.h"
#include "SkillsHeaderCtrl.h"

CSkillsHeaderCtrl::CSkillsHeaderCtrl()
{
}

CSkillsHeaderCtrl::~CSkillsHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CSkillsHeaderCtrl, CHeaderCtrl)
    //{{AFX_MSG_MAP(CSkillsHeaderCtrl)
    ON_WM_SETCURSOR()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkillsHeaderCtrl message handlers
BOOL CSkillsHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    return TRUE;
}

void CSkillsHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // Disabling this message handler prevents the column from resizing
    // when a double click is done on the column separator line.
}

void CSkillsHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    int column = GetColumnUnderMouse(point);
    NMHEADER nmh;
    ZeroMemory(&nmh, sizeof(NMHEADER));
    nmh.hdr.hwndFrom = GetSafeHwnd();
    nmh.hdr.code = HDN_ITEMCLICK;
    nmh.iItem = column;
    GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmh);
}

int CSkillsHeaderCtrl::GetColumnUnderMouse(CPoint point)
{
    int column = -1;        // assume none
    HD_ITEM hi;
    hi.mask = HDI_WIDTH;    // We want the column width

    int xPos = 0;
    int numColumns = GetItemCount();
    for (int ci = 0; ci < numColumns; ++ci)
    {
        GetItem(ci, &hi);
        if (point.x >= xPos && point.x <= xPos + hi.cxy)
        {
            column = ci;
        }
        xPos += hi.cxy;
    }
    return column;
}
