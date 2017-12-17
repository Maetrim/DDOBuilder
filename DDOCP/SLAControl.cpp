// SLAControl.cpp
//

#include "stdafx.h"
#include "SLAControl.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"

namespace
{
    const size_t c_controlSpacing = 3;
    enum StaticImages
    {
        SI_SpellSlotFixed = 0,
        SI_count
    };
    const int c_slaSlotImageSize = 36; // 36 * 36 pixels
    COLORREF c_transparentColour = RGB(255, 128, 255);
}

// global image data used for drawing all enhancements trees.
// this is only initialised once
bool s_slaImagesInitiliased = false;
CImage s_slaStaticImages[SI_count];

void CSLAControl::InitialiseStaticImages()
{
    if (!s_slaImagesInitiliased)
    {
        // load all the standard static images used when rendering
        LoadImageFile(IT_ui, "SpellSlotFixed", &s_slaStaticImages[SI_SpellSlotFixed]);
        s_slaImagesInitiliased = true;
    }
}

CSLAControl::CSLAControl() :
    m_pCharacter(NULL),
    m_bitmapSize(CSize(0, 0)),
    m_bCreateHitBoxes(false),
    m_tipCreated(false),
    m_pTooltipItem(NULL)
{
    InitialiseStaticImages();
}

CSLAControl::~CSLAControl()
{
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSLAControl, CStatic)
    //{{AFX_MSG_MAP(CSLAControl)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

/////////////////////////////////////////////////////////////////////////////
// CSLAControl message handlers

void CSLAControl::OnPaint()
{
    if (!m_tipCreated)
    {
        m_tooltip.Create(this);
        m_tipCreated = true;
    }

    CPaintDC pdc(this); // validates the client area on destruction
    pdc.SaveDC();
    // size is based on current window extent
    CRect rect;
    GetClientRect(rect);
    // ensure we have a background bitmap
    if (m_bitmapSize != CSize(rect.Width(), rect.Height()))
    {
        m_cachedDisplay.DeleteObject(); // ensure
        // create the bitmap we will render to
        m_cachedDisplay.CreateCompatibleBitmap(
                &pdc,
                rect.Width(),
                rect.Height());
        m_bitmapSize = CSize(rect.Width(), rect.Height());
    }
    // draw to a compatible device context and then splat to screen
    CDC memoryDc;
    memoryDc.CreateCompatibleDC(&pdc);
    memoryDc.SaveDC();
    memoryDc.SelectObject(&m_cachedDisplay);
    memoryDc.SelectStockObject(DEFAULT_GUI_FONT);
    memoryDc.SetBkMode(TRANSPARENT);

    // first fill the background with a default brush
    COLORREF bkColor = ::GetSysColor(COLOR_3DFACE);
    CBrush bkBrush(bkColor);
    memoryDc.FillRect(rect, &bkBrush);

    if (m_bCreateHitBoxes)
    {
        m_hitBoxes.clear();
    }

    // now render the SLAs
    size_t count = m_SLAs.size();
    if (count > 0)
    {
        // now work out draw positions
        size_t top = c_controlSpacing;
        size_t left = c_controlSpacing;
        for (int fs = 0; fs < (int)m_SLAs.size(); ++fs)
        {
            CRect slaRect(
                    left,
                    top,
                    left + c_slaSlotImageSize,
                    top + c_slaSlotImageSize);
            if (slaRect.right > rect.right)
            {
                // no room to fit, bump down and left
                left = c_controlSpacing;
                top += c_controlSpacing + c_slaSlotImageSize;
                slaRect -= slaRect.TopLeft();
                slaRect += CPoint(left, top);
            }
            s_slaStaticImages[SI_SpellSlotFixed].SetTransparentColor(c_transparentColour);
            s_slaStaticImages[SI_SpellSlotFixed].TransparentBlt(
                    memoryDc.GetSafeHdc(),
                    slaRect.left,
                    slaRect.top,
                    c_slaSlotImageSize,
                    c_slaSlotImageSize);
            if (m_bCreateHitBoxes)
            {
                SLAHitBox hitBox(fs, slaRect);
                m_hitBoxes.push_back(hitBox);
            }
            // show the fixed SLA icon
            std::list<SLA>::iterator it = m_SLAs.begin();
            std::advance(it, fs);
            const Spell & spell = FindSpellByName((*it).Name());
            CImage spellImage;
            HRESULT result = LoadImageFile(
                    IT_spell,
                    spell.Icon().c_str(),
                    &spellImage);
            if (result != S_OK)
            {
                result = LoadImageFile(
                        IT_spell,
                        "NoImage",
                        &spellImage);
            }
            if (result == S_OK)
            {
                spellImage.TransparentBlt(
                        memoryDc.GetSafeHdc(),
                        slaRect.left + 2,
                        slaRect.top + 2,
                        32,
                        32);
            }
            // move across for next SLA
            left += c_controlSpacing + c_slaSlotImageSize;
        }
    }
    m_bCreateHitBoxes = false;

    // now draw to display
    pdc.BitBlt(
            0,
            0,
            m_bitmapSize.cx,
            m_bitmapSize.cy,
            &memoryDc,
            0,
            0,
            SRCCOPY);
    memoryDc.RestoreDC(-1);
    memoryDc.DeleteDC();
    pdc.RestoreDC(-1);
}

void CSLAControl::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);
    // resize needs new hit boxes and a redraw as positions of items may have changed
    m_bCreateHitBoxes = true;
    Invalidate();
}

BOOL CSLAControl::OnEraseBkgnd(CDC* pDC)
{
    // no need to erase as we draw the entire client area on a paint action
    // this stops it flickering when we do.
    return TRUE;
}

const SLAHitBox * CSLAControl::FindByPoint(CRect * pRect) const
{
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);
    // see if we need to highlight the item under the cursor
    const SLAHitBox * item = NULL;
    std::list<SLAHitBox>::const_iterator it = m_hitBoxes.begin();
    while (item == NULL && it != m_hitBoxes.end())
    {
        if ((*it).IsInRect(point))
        {
            // mouse is over this item
            item = &(*it);
            if (pRect != NULL)
            {
                *pRect = (*it).Rect();
            }
        }
        ++it;
    }
    return item;
}

void CSLAControl::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which SLA the mouse may be over
    CRect itemRect;
    const SLAHitBox * item = FindByPoint(&itemRect);
    if (item != NULL
            && item != m_pTooltipItem)
    {
        // over a new item or a different item
        m_pTooltipItem = item;
        ShowTip(*item, itemRect);
    }
    else
    {
        if (m_showingTip
                && item != m_pTooltipItem)
        {
            // no longer over the same item
            HideTip();
        }
    }
    // as the mouse is over the SLAs, ensure the status bar message prompts available actions
    GetMainFrame()->SetStatusBarPromptText("This is a granted spell like ability and cannot be changed here");
}

LRESULT CSLAControl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    GetMainFrame()->SetStatusBarPromptText("Ready.");
    return 0;
}

void CSLAControl::SetCharacter(Character * pCharacter)
{
    m_pCharacter = pCharacter;
    m_SLAs.clear();
    if (m_pCharacter == NULL)
    {
        // no character == no SLAs to display
        m_bCreateHitBoxes = true;
        if (IsWindow(GetSafeHwnd()))
        {
            Invalidate(TRUE);
        }
    }
}

void CSLAControl::ShowTip(const SLAHitBox & item, CRect itemRect)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    ClientToScreen(&itemRect);
    CPoint tipTopLeft(itemRect.left, itemRect.bottom);
    CPoint tipAlternate(itemRect.left, itemRect.top);
    SetTooltipText(item, tipTopLeft, tipAlternate);
    m_showingTip = true;
    // track the mouse so we know when it leaves our window
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
}

void CSLAControl::HideTip()
{
    // tip not shown if not over an SLA
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
        m_pTooltipItem = NULL;
    }
}

void CSLAControl::SetTooltipText(
        const SLAHitBox & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    // find the SLA to show info about
    std::string slaName;
    std::list<SLA>::const_iterator si = m_SLAs.begin();
    std::advance(si, item.SLAIndex());
    slaName = (*si).Name();
    // now we have the spell name, look it up
    Spell spell = FindSpellByName(slaName);
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetSpell(
            m_pCharacter,
            spell,
            Class_Unknown,
            1,
            1);
    m_tooltip.Show();
}

void CSLAControl::AddSLA(const std::string & slaName)
{
    // add the spell at the relevant level
    SLA spell(slaName);
    m_SLAs.push_back(spell);
    if (IsWindow(GetSafeHwnd()))
    {
        m_bCreateHitBoxes = true;
        Invalidate();   // redraw
    }
}

void CSLAControl::RevokeSLA(const std::string & slaName)
{
    // remove the named spell from the relevant level
    SLA spell(slaName);
    std::list<SLA>::iterator it = m_SLAs.begin();
    while (it != m_SLAs.end())
    {
        if ((*it) == spell)
        {
            // this is the one to delete
            m_SLAs.erase(it);
            break;
        }
        ++it;
    }
    if (IsWindow(GetSafeHwnd()))
    {
        m_bCreateHitBoxes = true;
        Invalidate();   // redraw
    }
}
