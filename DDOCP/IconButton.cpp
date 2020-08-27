// IconButton.cpp
//

#include "stdafx.h"
#include "IconButton.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"

namespace
{
    COLORREF f_selectedColor = RGB(128, 0, 0);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CIconButton, CStatic)
    //{{AFX_MSG_MAP(CIconButton)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CIconButton::CIconButton() :
    m_bSelected(false)
{
    //{{AFX_DATA_INIT(CIconButton)
    //}}AFX_DATA_INIT
}

BOOL CIconButton::OnEraseBkgnd(CDC* pDC)
{
    return 0;
}

void CIconButton::OnPaint()
{
    CPaintDC pdc(this); // validates the client area on destruction
    pdc.SaveDC();

    CRect rect;
    GetWindowRect(&rect);
    rect -= rect.TopLeft(); // convert to client rectangle

    // fill the background
    if (m_bSelected)
    {
        pdc.FillSolidRect(rect, GetSysColor(COLOR_HIGHLIGHT));
    }
    else
    {
        pdc.FillSolidRect(rect, GetSysColor(COLOR_BTNFACE));
    }
    m_image.TransparentBlt(
            pdc.GetSafeHdc(),
            (rect.Width() - 32) / 2,
            (rect.Height() - 32) / 2,
            32,
            32);
    pdc.RestoreDC(-1);
}

void CIconButton::SetSelected(bool selected)
{
    if (selected != m_bSelected)
    {
        m_bSelected = selected;
        Invalidate();   // redraw on state change
    }
}

bool CIconButton::IsSelected() const
{
    return m_bSelected;
}

void CIconButton::OnMouseMove(UINT nFlags, CPoint point)
{
    CStatic::OnMouseMove(nFlags, point);
    // also let our parent know about the mouse move event
    ClientToScreen(&point);
    GetParent()->ScreenToClient(&point);
    GetParent()->SendMessage(WM_MOUSEMOVE, nFlags, (LPARAM)MAKELONG(point.x, point.y));
}
