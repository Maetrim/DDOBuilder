// MFCButtonMMRelay.cpp : implementation file
//

#include "stdafx.h"
#include "DDOCP.h"
#include "MFCButtonMMRelay.h"


// CMFCButtonMMRelay
static const COLORREF clrDefault = (COLORREF) -1;

IMPLEMENT_DYNAMIC(CMFCButtonMMRelay, CMFCButton)

CMFCButtonMMRelay::CMFCButtonMMRelay()
{
}

CMFCButtonMMRelay::~CMFCButtonMMRelay()
{
}

BEGIN_MESSAGE_MAP(CMFCButtonMMRelay, CMFCButton)
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

void CMFCButtonMMRelay::SetTransparent()
{
    m_Image.SetTransparentColor(c_transparentColour);
    m_ImageHot.SetTransparentColor(c_transparentColour);
    m_ImageDisabled.SetTransparentColor(c_transparentColour);
    m_ImageChecked.SetTransparentColor(c_transparentColour);
    m_ImageCheckedHot.SetTransparentColor(c_transparentColour);
    m_ImageCheckedDisabled.SetTransparentColor(c_transparentColour);
}
// CMFCButtonMMRelay message handlers

void CMFCButtonMMRelay::OnMouseMove(UINT nFlags, CPoint point)
{
    CMFCButton::OnMouseMove(nFlags, point);
    // also let our parent know about the mouse move event
    ClientToScreen(&point);
    GetParent()->ScreenToClient(&point);
    GetParent()->SendMessage(WM_MOUSEMOVE, nFlags, (LPARAM)MAKELONG(point.x, point.y));
}

void CMFCButtonMMRelay::OnRButtonUp(UINT nFlags, CPoint point) 
{
    NMHDR hdr;
    hdr.code = NM_RCLICK;
    hdr.hwndFrom = this->GetSafeHwnd();
    hdr.idFrom = GetDlgCtrlID();
    GetParent()->SendMessage(WM_NOTIFY, (WPARAM)hdr.idFrom, (LPARAM)&hdr);
}
