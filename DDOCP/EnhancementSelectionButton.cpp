// EnhancementSelectionButton.cpp
//

#include "stdafx.h"
#include "EnhancementSelectionButton.h"
#include "GlobalSupportFunctions.h"

namespace
{
    COLORREF f_selectedColor = RGB(128, 0, 0);
    COLORREF f_unselectedColor = RGB(32, 32, 32);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CEnhancementSelectionButton, CStatic)
    //{{AFX_MSG_MAP(CEnhancementSelectionButton)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CEnhancementSelectionButton::CEnhancementSelectionButton() :
    m_bSelected(false),
    m_bMadeGray(false)
{
    //{{AFX_DATA_INIT(CEnhancementSelectionButton)
    //}}AFX_DATA_INIT
}

BOOL CEnhancementSelectionButton::OnEraseBkgnd(CDC* pDC)
{
    return 0;
}

void CEnhancementSelectionButton::OnPaint()
{
    if (!IsWindowEnabled())
    {
        if (!m_bMadeGray)
        {
            m_bMadeGray = true;
            MakeGrayScale(&m_image, c_transparentColour);
        }
    }

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
        // can be disabled at this point
        if (IsWindowEnabled())
        {
            pdc.FillSolidRect(rect, GetSysColor(COLOR_BTNFACE));
        }
        else
        {
            pdc.FillSolidRect(rect, GetSysColor(COLOR_GRAYTEXT));
        }
    }
    m_image.TransparentBlt(
            pdc.GetSafeHdc(),
            (rect.Width() - 32) / 2,
            (rect.Height() - 32) / 2,
            32,
            32);
    pdc.RestoreDC(-1);
}

void CEnhancementSelectionButton::SetImage(const std::string & imageName)
{
    LoadImageFile(IT_enhancement, imageName, &m_image);
    m_image.SetTransparentColor(c_transparentColour);
}

void CEnhancementSelectionButton::SetSelected(bool selected)
{
    if (selected != m_bSelected)
    {
        m_bSelected = selected;
        Invalidate();   // redraw on state change
    }
}

