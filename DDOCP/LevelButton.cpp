// LevelButton.cpp
//

#include "stdafx.h"
#include "LevelButton.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const COLORREF c_transparentColour = RGB(255, 128, 255);
    const COLORREF c_pinkWarningColour = RGB(0xFF, 0xB6, 0xC1); // Light Pink
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CLevelButton, CStatic)
    //{{AFX_MSG_MAP(CLevelButton)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CLevelButton::CLevelButton() :
    m_level(0),
    m_class(Class_Unknown),
    m_bSelected(false),
    m_bHasIssue(false)
{
    //{{AFX_DATA_INIT(CLevelButton)
    //}}AFX_DATA_INIT
    HRESULT result = LoadImageFile(
            IT_ui,
            (LPCTSTR)EnumEntryText(m_class, classTypeMap),
            &m_image);
    m_image.SetTransparentColor(c_transparentColour);

    // create the font used
    LOGFONT lf;
    ZeroMemory((PVOID)&lf, sizeof(LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0));
    lf = nm.lfMenuFont;
    m_font.CreateFontIndirect(&lf);
}

BOOL CLevelButton::OnEraseBkgnd(CDC* pDC)
{
    return 0;
}

void CLevelButton::OnPaint()
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
        if (m_bHasIssue)
        {
            pdc.FillSolidRect(rect, c_pinkWarningColour);
        }
        else
        {
            pdc.FillSolidRect(rect, GetSysColor(COLOR_BTNFACE));
        }
    }
    m_image.TransparentBlt(
            pdc.GetSafeHdc(),
            (rect.Width() - 32) / 2,
            4,          // always 4 pixels from the top
            32,
            32);
    // and lastly add the level text
    pdc.SelectObject(&m_font);
    CString text;
    text.Format("Level %d", m_level);
    CSize ts = pdc.GetTextExtent(text);
    pdc.SetBkMode(TRANSPARENT);
    pdc.TextOut((rect.Width() - ts.cx) / 2, 38, text);
    pdc.RestoreDC(-1);
}

void CLevelButton::SetLevel(size_t level)
{
    m_level = level;
}

void CLevelButton::SetClass(ClassType ct)
{
    m_class = ct;
    m_image.Destroy();
    // load the new icon to display
    HRESULT result = LoadImageFile(
            IT_ui,
            (LPCTSTR)EnumEntryText(m_class, classTypeMap),
            &m_image);
    m_image.SetTransparentColor(c_transparentColour);
    Invalidate();
}

void CLevelButton::SetSelected(bool selected)
{
    if (selected != m_bSelected)
    {
        m_bSelected = selected;
        Invalidate();   // redraw on state change
    }
}

bool CLevelButton::IsSelected() const
{
    return m_bSelected;
}

void CLevelButton::SetIssueState(bool hasIssue)
{
    if (m_bHasIssue != hasIssue)
    {
        m_bHasIssue = hasIssue;
        Invalidate();
    }
}
