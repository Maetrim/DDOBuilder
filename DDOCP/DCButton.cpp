// DCButton.cpp
//

#include "stdafx.h"
#include "DCButton.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CDCButton, CStatic)
    //{{AFX_MSG_MAP(CDCButton)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CDCButton::CDCButton(Character * charData, const DC & dc) :
    m_pCharacter(charData),
    m_dc(dc),
    m_stacks(0)         // stack count updated later
{
    //{{AFX_DATA_INIT(CDCButton)
    //}}AFX_DATA_INIT
    if (S_OK != LoadImageFile(IT_enhancement, dc.Icon(), &m_image, false))
    {
        // see if its a feat icon we need to use
        if (S_OK != LoadImageFile(IT_feat, dc.Icon(), &m_image, false))
        {
            LoadImageFile(IT_item, dc.Icon(), &m_image);
        }
    }
    m_image.SetTransparentColor(c_transparentColour);
}

BOOL CDCButton::OnEraseBkgnd(CDC* pDC)
{
    return 0;
}

void CDCButton::OnPaint()
{
    CPaintDC pdc(this); // validates the client area on destruction
    pdc.SaveDC();

    CRect rect;
    GetWindowRect(&rect);
    rect -= rect.TopLeft(); // convert to client rectangle

    // fill the background
    pdc.FillSolidRect(rect, GetSysColor(COLOR_BTNFACE));
    m_image.TransparentBlt(
            pdc.GetSafeHdc(),
            (rect.Width() - 32) / 2,
            (rect.Height() - 32) / 2,
            32,
            32);
    pdc.RestoreDC(-1);
}

const DC & CDCButton::GetDCItem() const
{
    return m_dc;
}

void CDCButton::AddStack()
{
    ++m_stacks;
}

void CDCButton::RevokeStack()
{
    --m_stacks;
}

size_t CDCButton::NumStacks() const
{
    return m_stacks;
}

bool CDCButton::IsYou(const DC & dc)
{
    return (dc == m_dc);
}

