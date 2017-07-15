// GroupLine.cpp
//

#include "stdafx.h"
#include "GroupLine.h"
#include "afxvisualmanager.h"

using MfcControls::CGroupLine;

// Returns a device dependent LOGFONT for use on screen and on printer.
LOGFONT GetDisplayDeviceDependantFont(int deviceLogPixelsY)
{
    // Use the system display font.
    NONCLIENTMETRICS ncm;
    memset(&ncm, 0, sizeof(NONCLIENTMETRICS)); // clear out structure.
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
    LOGFONT lf = ncm.lfMessageFont;

    // This font will already be translated into device sizes, i.e. it will be size
    // 11 for my screen even though it is set up to be size 8:
    // -11 = -MulDiv(8, pDc->GetDeviceCaps(LOGPIXELSY), 72);
    // So we need to translate back into a non-device size using the desktop window dc
    HWND desktop = GetDesktopWindow();
    if (desktop != NULL)
    {
        HDC hDc = GetDC(desktop);
        lf.lfHeight = MulDiv(lf.lfHeight, 72, GetDeviceCaps(hDc, LOGPIXELSY));
        ReleaseDC(desktop, hDc);
    }

    // This size should be the proper size now (8 on my computer), we can now
    // translate it back using the supplied device to get a device dependent font size
    lf.lfHeight = MulDiv(lf.lfHeight, deviceLogPixelsY, 72);
    
    return lf;
}

CGroupLine::CGroupLine()
{
}

CGroupLine::~CGroupLine()
{
}

void CGroupLine::SetWindowText(LPCTSTR lpszString)
{
    SetRedraw(FALSE);
    CWnd::SetWindowText(lpszString);
    SetRedraw(TRUE);
    RedrawWindow();
}

BEGIN_MESSAGE_MAP(CGroupLine, CWnd)
    //{{AFX_MSG_MAP(CGroupLine)
    ON_WM_ENABLE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupLine message handlers

void CGroupLine::OnEnable(BOOL bEnable) 
{
    SetRedraw(FALSE);
    CWnd::OnEnable(bEnable);
    SetRedraw(TRUE);
    RedrawWindow();
}

BOOL CGroupLine::OnEraseBkgnd(CDC* pDC) 
{
    CRect cr;
    GetClientRect(cr); 
    // use the selected theme in use for the background colour selection
    CMFCVisualManager * manager = CMFCVisualManager::GetInstance();
    if (manager != NULL)
    {
        manager->OnFillPopupWindowBackground(pDC, cr);
    }
    else
    {
        pDC->FillSolidRect(cr, GetSysColor(COLOR_BTNFACE));
    }

    return TRUE;
}

void CGroupLine::OnPaint() 
{
    // spacing around the text - chosen to match standard group box
    const int border = 10;
    const int lineBorderLeft = 3;
    const int lineBorderRight = 1;

    CPaintDC dc(this); // device context for painting
    dc.SaveDC();

    CRect clientRect;
    GetClientRect(clientRect);

    CString text;
    GetWindowText(text);

    CRect textRect;
    CFont deviceFont;   // to avoid leak
    if (!text.IsEmpty())
    {
        // Create our new device specific font.
        LOGFONT lf = GetDisplayDeviceDependantFont(dc.GetDeviceCaps(LOGPIXELSY));
        VERIFY(deviceFont.CreateFontIndirect(&lf));
        dc.SelectObject(&deviceFont);
        dc.SetBkMode(TRANSPARENT);
        // draw text
        CSize textExtent = dc.GetTextExtent(text);
        textRect = clientRect;
        textRect.left += border;
        textRect.right = textRect.left + textExtent.cx;
        if (textRect.right > clientRect.right - border)
        {
            // would overflow width so clip it
            textRect.right = clientRect.right - border;
        }
        dc.DrawText(text, textRect, DT_END_ELLIPSIS|DT_LEFT|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
    }

    // draw line
    COLORREF upperCol = ::GetSysColor(COLOR_3DSHADOW);
    COLORREF lowerCol = ::GetSysColor(COLOR_3DHIGHLIGHT);
    int lineUpper = (clientRect.top + clientRect.bottom) / 2;
    int lineLower = lineUpper + 1;
    CPen upperPen(PS_SOLID, 1, upperCol);
    dc.SelectObject(&upperPen);
    dc.MoveTo(clientRect.left, lineUpper);
    if (!text.IsEmpty())
    {
        // break for text
        dc.LineTo(textRect.left - lineBorderLeft, lineUpper);
        dc.MoveTo(textRect.right + lineBorderRight, lineUpper);
    }
    dc.LineTo(clientRect.right, lineUpper);
    CPen lowerPen(PS_SOLID, 1, lowerCol);
    dc.SelectObject(&lowerPen);
    dc.MoveTo(clientRect.left, lineLower);
    if (!text.IsEmpty())
    {
        // break for text
        dc.LineTo(textRect.left - lineBorderLeft, lineLower);
        dc.MoveTo(textRect.right + lineBorderRight, lineLower);
    }
    dc.LineTo(clientRect.right, lineLower);

    dc.RestoreDC(-1) ;
}
