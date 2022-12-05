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
            if (S_OK != LoadImageFile(IT_item, dc.Icon(), &m_image, false))
            {
                if (S_OK != LoadImageFile(IT_ui, dc.Icon(), &m_image, false))
                {
                    LoadImageFile(IT_spell, dc.Icon(), &m_image);
                }
            }
        }
    }
    m_image.SetTransparentColor(c_transparentColour);
    charData->AttachObserver(this);
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
            3,
            3,
            32,
            32);
    // also show the current DC value under the icon
    int dc = m_dc.CalculateDC(m_pCharacter);
    CString text;
    text.Format("DC: %d", dc);

    // shown in a small font
    LOGFONT lf;
    ZeroMemory((PVOID)&lf, sizeof(LOGFONT));
    strcpy_s(lf.lfFaceName, "Consolas");
    lf.lfHeight = 11;
    CFont smallFont;
    smallFont.CreateFontIndirect(&lf);
    pdc.SelectObject(&smallFont);

    // measure the text so its centered horizontally
    CSize textSize = pdc.GetTextExtent(text);
    pdc.SetBkMode(TRANSPARENT); // don't erase the text background
    pdc.TextOut(
            rect.left + (rect.Width() - textSize.cx) / 2,
            rect.bottom - 12,
            text);
    // were done, restore the dc to pristine condition
    pdc.RestoreDC(-1);
    // font resource destroyed automatically by destructor
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

void CDCButton::UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateAbilityValueChanged(Character * charData, AbilityType ability)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateAbilityTomeChanged(Character * charData, AbilityType ability)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateRaceChanged(Character * charData, RaceType race)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateFeatTrained(Character * charData, const std::string & featName)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateFeatRevoked(Character * charData, const std::string & featName)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateEnhancementTrained(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateEnhancementRevoked(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateEnhancementTreeReset(Character * charData)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateGrantedFeatsChanged(Character * charData)
{
    Invalidate(TRUE);
}

void CDCButton::UpdateGearChanged(Character * charData, InventorySlotType slot)
{
    Invalidate(TRUE);
}
