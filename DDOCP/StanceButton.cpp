// StanceButton.cpp
//

#include "stdafx.h"
#include "StanceButton.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"

namespace
{
    COLORREF f_selectedColor = RGB(128, 0, 0);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CStanceButton, CStatic)
    //{{AFX_MSG_MAP(CStanceButton)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CStanceButton::CStanceButton(Character * charData, const Stance & stance) :
    m_pCharacter(charData),
    m_stance(stance),
    m_bSelected(false),
    m_stacks(0)         // stack count updated later
{
    //{{AFX_DATA_INIT(CStanceButton)
    //}}AFX_DATA_INIT
    if (S_OK != LoadImageFile(IT_enhancement, stance.Icon(), &m_image, false))
    {
        // see if its a feat icon we need to use
        LoadImageFile(IT_feat, stance.Icon(), &m_image);
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_bSelected = m_pCharacter->IsStanceActive(stance.Name());
}

BOOL CStanceButton::OnEraseBkgnd(CDC* pDC)
{
    return 0;
}

void CStanceButton::OnPaint()
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
    //// stances debug
    //CString stacks;
    //stacks.Format("%d", m_stacks);
    //pdc.TextOut(0, 0, stacks);
}

void CStanceButton::SetSelected(bool selected)
{
    if (selected != m_bSelected)
    {
        m_bSelected = selected;
        Invalidate();   // redraw on state change
    }
}

bool CStanceButton::IsSelected() const
{
    return m_bSelected;
}

const Stance & CStanceButton::GetStance() const
{
    return m_stance;
}

void CStanceButton::AddStack()
{
    ++m_stacks;
    //// stances debug
    //if (IsWindow(GetSafeHwnd()))
    //{
    //    Invalidate();
    //}
}

void CStanceButton::RevokeStack()
{
    --m_stacks;
    //// stances debug
    //if (IsWindow(GetSafeHwnd()))
    //{
    //    Invalidate();
    //}
}

size_t CStanceButton::NumStacks() const
{
    return m_stacks;
}

bool CStanceButton::IsYou(const Stance & stance)
{
    return (stance == m_stance);
}

