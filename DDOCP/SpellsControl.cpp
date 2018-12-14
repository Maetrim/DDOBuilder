// SpellsControl.cpp
//

#include "stdafx.h"
#include "SpellsControl.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"
#include "BreakdownItem.h"

namespace
{
    const size_t c_controlSpacing = 3;
    enum StaticImages
    {
        SI_SpellSlotFixed = 0,
        SI_SpellSlotTrainable,
        SI_count
    };
    const int c_spellSlotImageSize = 36; // 36 * 36 pixels
}

// global image data used for drawing all enhancements trees.
// this is only initialised once
bool s_spellImagesInitiliased = false;
CImage s_staticImages[SI_count];

void CSpellsControl::InitialiseStaticImages()
{
    if (!s_spellImagesInitiliased)
    {
        // load all the standard static images used when rendering
        LoadImageFile(IT_ui, "SpellSlotFixed", &s_staticImages[SI_SpellSlotFixed]);
        LoadImageFile(IT_ui, "SpellSlotTrainable", &s_staticImages[SI_SpellSlotTrainable]);
        s_spellImagesInitiliased = true;
    }
}

CSpellsControl::CSpellsControl() :
    m_pCharacter(NULL),
    m_bitmapSize(CSize(0, 0)),
    m_tipCreated(false),
    m_pTooltipItem(NULL),
    m_editSpellLevel(0),
    m_editSpellIndex(0),
    m_clientSize(0, 0, 300, 300),
    m_bHVisible(false),
    m_bVVisible(false)
{
    InitialiseStaticImages();
}

CSpellsControl::~CSpellsControl()
{
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSpellsControl, CStatic)
    //{{AFX_MSG_MAP(CSpellsControl)
    ON_WM_PAINT()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_LBUTTONDOWN()
    ON_CBN_SELENDOK(IDC_COMBO_SPELLSELECT, OnSpellSelectOk)
    ON_CBN_SELENDCANCEL(IDC_COMBO_SPELLSELECT, OnSpellSelectCancel)
    ON_MESSAGE(WM_MOUSEHOVER, OnHoverComboBox)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

/////////////////////////////////////////////////////////////////////////////
// CSpellsControl message handlers

void CSpellsControl::OnPaint()
{
    // size is based on current window extent
    CRect rect;
    GetWindowRect(rect);
    ScreenToClient(&rect);

    if (!IsWindow(m_comboSpellSelect.GetSafeHwnd()))
    {
        CComboBox test;
        m_comboSpellSelect.Create(
                WS_CHILD | WS_VSCROLL | WS_TABSTOP
                | CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS,
                CRect(77,36,119,250),
                this,
                IDC_COMBO_SPELLSELECT);
        CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
        m_comboSpellSelect.SetFont(pDefaultGUIFont, TRUE);
        CRect rctHorizontal(0, 0, rect.right, rect.bottom);
        CRect rctVertical(0, 0, rect.right, rect.bottom);

        m_scrollHorizontal.Create(
                WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
                rctHorizontal,
                this,
                0);
        m_scrollVertical.Create(
                WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_RIGHTALIGN,
                rctVertical,
                this,
                1);
        ProcessScrollBars(rect.Width(), rect.Height());
    }
    if (!m_tipCreated)
    {
        m_tooltip.Create(this);
        m_tipCreated = true;
    }
    int x_offset = m_scrollHorizontal.IsWindowVisible() ? m_scrollHorizontal.GetScrollPos() : 0;
    int y_offset = m_scrollVertical.IsWindowVisible() ? m_scrollVertical.GetScrollPos() : 0;

    // adjust the rect based on shown scrollbars, this stops flickering of the
    // scrollbars
    if (m_bVVisible)
    {
        rect.right -= GetSystemMetrics(SM_CXVSCROLL);
    }
    if (m_bHVisible)
    {
        rect.bottom -= GetSystemMetrics(SM_CYHSCROLL);
    }

    CPaintDC pdc(this); // validates the client area on destruction
    pdc.SaveDC();
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

    m_hitBoxes.clear();

    // now render the number of spells for each level
    for (int spellLevel = 0; spellLevel < (int)m_spellsPerLevel.size(); ++spellLevel)
    {
        // how many spells at this level?
        int count = m_spellsPerLevel[spellLevel];
        if (count > 0)
        {
            // get the current trained spells at this level
            std::list<TrainedSpell> trainedSpells =
                    m_pCharacter->TrainedSpells(m_class, spellLevel + 1); // 1 based
            // now work out draw positions
            int top = spellLevel * (c_spellSlotImageSize + c_controlSpacing) - y_offset;
            CString levelLabel;
            levelLabel.Format("Level %d", spellLevel +1);
            CSize labelSize = memoryDc.GetTextExtent(levelLabel);
            memoryDc.TextOut(
                    c_controlSpacing - x_offset, 
                    top + (c_spellSlotImageSize - labelSize.cy) / 2,
                    levelLabel);
            // must have trainable spells at this level to be able to show spells slots
            // even if they are fixed known spells.
            int offset = 0;
            // handle fixed spells here, increment offset for each
            for (int fs = 0; fs < (int)m_fixedSpells[spellLevel].size(); ++fs)
            {
                int left = offset * (c_spellSlotImageSize + c_controlSpacing)
                        + labelSize.cx + c_controlSpacing * 2 - x_offset;
                CRect spellRect(
                        left,
                        top,
                        left + c_spellSlotImageSize,
                        top + c_spellSlotImageSize);
                s_staticImages[SI_SpellSlotFixed].SetTransparentColor(c_transparentColour);
                s_staticImages[SI_SpellSlotFixed].TransparentBlt(
                        memoryDc.GetSafeHdc(),
                        spellRect.left,
                        spellRect.top,
                        c_spellSlotImageSize,
                        c_spellSlotImageSize);
                SpellHitBox hitBox(spellLevel + 1, -(fs+1), spellRect);
                m_hitBoxes.push_back(hitBox);
                // show the fixed spell icon
                std::list<FixedSpell>::iterator it = m_fixedSpells[spellLevel].begin();
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
                            spellRect.left + 2,
                            spellRect.top + 2,
                            32,
                            32);
                }
                // next spell moves across
                offset += 1;
            }
            // now handle trainable spells
            for (int spellIndex = 0; spellIndex < count; ++spellIndex)
            {
                int left = offset * (c_spellSlotImageSize + c_controlSpacing)
                        + labelSize.cx + c_controlSpacing * 2 - x_offset;
                CRect spellRect(
                        left,
                        top,
                        left + c_spellSlotImageSize,
                        top + c_spellSlotImageSize);
                s_staticImages[SI_SpellSlotTrainable].SetTransparentColor(c_transparentColour);
                s_staticImages[SI_SpellSlotTrainable].TransparentBlt(
                        memoryDc.GetSafeHdc(),
                        spellRect.left,
                        spellRect.top,
                        c_spellSlotImageSize,
                        c_spellSlotImageSize);
                SpellHitBox hitBox(spellLevel + 1, spellIndex, spellRect);
                m_hitBoxes.push_back(hitBox);
                // show the selected spell icon, if a selected spell
                if (spellIndex < (int)trainedSpells.size())
                {
                    std::list<TrainedSpell>::iterator it = trainedSpells.begin();
                    std::advance(it, spellIndex);
                    const Spell & spell = FindSpellByName((*it).SpellName());
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
                                spellRect.left + 2,
                                spellRect.top + 2,
                                32,
                                32);
                    }
                }
                // next spell moves across
                offset += 1;
            }
        }
    }

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

void CSpellsControl::OnSize(UINT nType, int cx, int cy)
{
    // get the current scroll positions for correct paint operations
    CSize size = RequiredSize();
    m_clientSize = CRect(0, 0, size.cx, size.cy);

    if (IsWindow(m_scrollHorizontal.GetSafeHwnd()))
    {
        CRect rect;
        GetWindowRect(rect);
        ScreenToClient(&rect);
        ProcessScrollBars(rect.Width(), rect.Height());
        Invalidate();
    }
}

BOOL CSpellsControl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

const SpellHitBox * CSpellsControl::FindByPoint(CRect * pRect) const
{
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);
    // see if we need to highlight the item under the cursor
    const SpellHitBox * item = NULL;
    std::list<SpellHitBox>::const_iterator it = m_hitBoxes.begin();
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

void CSpellsControl::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which spell the mouse may be over
    bool fixedSpell = false;
    CRect itemRect;
    const SpellHitBox * item = FindByPoint(&itemRect);
    if (item != NULL
            && item != m_pTooltipItem)
    {
        fixedSpell = (item->SpellLevel() < 0);  // fixed spells have -ve indexes
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
    // as the mouse is over the spells, ensure the status bar message prompts available actions
    if (fixedSpell)
    {
        GetMainFrame()->SetStatusBarPromptText("This is a granted spell and cannot be changed");
    }
    else
    {
        GetMainFrame()->SetStatusBarPromptText("Left click spell slot to choose/change spell");
    }
}

LRESULT CSpellsControl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    GetMainFrame()->SetStatusBarPromptText("Ready.");
    return 0;
}

void CSpellsControl::SetCharacter(Character * pCharacter, ClassType ct)
{
    m_pCharacter = pCharacter;
    m_class = ct;
    // fixed spells change if character changes
    // we get updated with list for new character
    for (size_t i = 0; i < MAX_SPELL_LEVEL; ++i)
    {
        m_fixedSpells[i].clear();
        // negative spell levels for auto-assigned spells
        int autoSpellLevel = -((int)i + 1);
        std::vector<Spell> autoSpells = FilterSpells(ct, autoSpellLevel);
        for (size_t si = 0; si < autoSpells.size(); ++si)
        {
            FixedSpell spell(autoSpells[si].Name(), i + 1);
            m_fixedSpells[i].push_back(spell);
        }
    }
    UpdateSpells();
    if (m_pCharacter == NULL)
    {
        // no character == no spells to display
        m_spellsPerLevel.clear();
        if (IsWindow(GetSafeHwnd()))
        {
            Invalidate(TRUE);
        }
    }
}

void CSpellsControl::SetTrainableSpells(const std::vector<size_t> & spellsPerLevel)
{
    m_spellsPerLevel = spellsPerLevel;
    // re-draw on spell count change
    if (IsWindow(GetSafeHwnd()))
    {
        Invalidate(TRUE);
    }
}

LRESULT CSpellsControl::OnHoverComboBox(WPARAM wParam, LPARAM lParam)
{
    // wParam = selected index
    // lParam = control ID
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    if (wParam >= 0)
    {
        // we have a selection, get the spells name
        CString spellName;
        m_comboSpellSelect.GetLBText(wParam, spellName);
        if (!spellName.IsEmpty())
        {
            // now we have the spell name, look it up
            CRect rctWindow;
            m_comboSpellSelect.GetWindowRect(&rctWindow);
            rctWindow.right = rctWindow.left + m_comboSpellSelect.GetDroppedWidth();
            // tip is shown to the left or the right of the combo box
            CPoint tipTopLeft(rctWindow.left, rctWindow.top);
            CPoint tipAlternate(rctWindow.right, rctWindow.top);
            Spell spell = FindSpellByName((LPCTSTR)spellName);
            m_tooltip.SetOrigin(tipTopLeft, tipAlternate, true);
            m_tooltip.SetSpell(
                    m_pCharacter,
                    spell,
                    m_class,
                    m_editSpellLevel,
                    9);
            m_tooltip.Show();
            m_showingTip = true;
        }
    }
    return 0;
}

void CSpellsControl::ShowTip(const SpellHitBox & item, CRect itemRect)
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

void CSpellsControl::HideTip()
{
    // tip not shown if not over an assay
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
        m_pTooltipItem = NULL;
    }
}

void CSpellsControl::SetTooltipText(
        const SpellHitBox & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    size_t spellLevel = 1;
    size_t maxSpellLevel = 1;
    // find the spell to show info about
    std::string spellName;
    if (item.SpellIndex() >= 0)
    {
        // this is a trained spell
        std::list<TrainedSpell> spells = m_pCharacter->TrainedSpells(
                m_class,
                item.SpellLevel());
        if ((size_t)item.SpellIndex() < spells.size())
        {
            std::list<TrainedSpell>::const_iterator si = spells.begin();
            std::advance(si, item.SpellIndex());
            spellName = (*si).SpellName();
            spellLevel = (*si).Level();
            maxSpellLevel = SpellSlotsForClass(m_class, m_pCharacter->ClassLevels(m_class)).size();
        }
        else
        {
            // no spell trained in this slot
            spellName = "No spell trained"; // special spell in the Spells.xml file
        }
    }
    else
    {
        // negative indexes are fixed spells
        size_t spellIndex = (size_t)abs(item.SpellIndex()) - 1;
        ASSERT(spellIndex < m_fixedSpells[item.SpellLevel()-1].size());
        std::list<FixedSpell>::const_iterator si = m_fixedSpells[item.SpellLevel()-1].begin();
        std::advance(si, spellIndex);
        spellName = (*si).Name();
        spellLevel = (*si).Level();
        maxSpellLevel = SpellSlotsForClass(m_class, m_pCharacter->ClassLevels(m_class)).size();
    }
    // now we have the spell name, look it up
    Spell spell = FindSpellByName(spellName);
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetSpell(
            m_pCharacter,
            spell,
            m_class,
            spellLevel,
            maxSpellLevel);
    m_tooltip.Show();
}

void CSpellsControl::UpdateSpells()
{
    // update our cached lists
    for (size_t i = 0; i < MAX_SPELL_LEVEL; ++i)
    {
        // first revoke the spell effects
        RevokeSpellEffects(m_trainedSpells[i]);
        m_trainedSpells[i].clear();
        // now query the character for all the spells trained at each level
        // for this class
        if (m_pCharacter != NULL)
        {
            m_trainedSpells[i] = m_pCharacter->TrainedSpells(m_class, i + 1); // spell levels are 1 based
            ApplySpellEffects(m_trainedSpells[i]);
        }
    }
    if (IsWindow(GetSafeHwnd()))
    {
        Invalidate();   // redraw
    }
}


void CSpellsControl::OnLButtonDown(UINT nFlags, CPoint point)
{
    CStatic::OnLButtonDown(nFlags, point);
    if (m_hitBoxes.size() > 0)
    {
        // determine which spell slot has been clicked on
        const SpellHitBox * item = FindByPoint();
        if (item != NULL)
        {
            // only allow spell selection if this is not a fixed spell (spellIndex < 0)
            if (item->SpellIndex() >= 0)
            {
                m_editSpellLevel = item->SpellLevel();
                m_editSpellIndex = item->SpellIndex();
                // try and train a spell in this slot. We may need to
                // revoke an existing spell also if the selection changes

                // display a drop list combo just below the spell item
                // get the list of trainable spells for this class and level
                std::string currentSelection;
                std::vector<Spell> spells = FilterSpells(m_class, item->SpellLevel());
                RemoveTrained(&spells, &currentSelection);

                m_comboSpellSelect.SetImageList(NULL);
                m_comboSpellSelect.ResetContent();

                m_spellImagesList.DeleteImageList();
                m_spellImagesList.Create(
                        32,             // all icons are 32x32 pixels
                        32,
                        ILC_COLOR32,
                        0,
                        spells.size());
                std::vector<Spell>::const_iterator it = spells.begin();
                while (it != spells.end())
                {
                    (*it).AddImage(&m_spellImagesList);
                    ++it;
                }
                m_comboSpellSelect.SetImageList(&m_spellImagesList);

                // now add the spell names to the combo control
                int sel = CB_ERR;   // assume no selection
                it = spells.begin();
                size_t spellIndex = 0;
                while (it != spells.end())
                {
                    size_t pos = m_comboSpellSelect.AddString((*it).Name().c_str());
                    m_comboSpellSelect.SetItemData(pos, spellIndex);
                    if ((*it).Name() == currentSelection)
                    {
                        // this is the default selection in the combo
                        sel = pos;
                    }
                    ++spellIndex;
                    ++it;
                }
                // hide the current tip
                HideTip();
                // position the drop list combo under the spell slot being trained
                CRect comboRect(item->Rect());
                comboRect += CPoint(0, c_spellSlotImageSize);
                comboRect.right = comboRect.left + 250;
                comboRect.bottom = comboRect.top + 960;   // 20 items visible in drop list
                m_comboSpellSelect.MoveWindow(comboRect);
                m_comboSpellSelect.SetCurSel(sel);
                m_comboSpellSelect.SetDroppedWidth(350); // wider to show extra text
                // set control visible to the user
                m_comboSpellSelect.ShowWindow(SW_SHOW);
                m_comboSpellSelect.SetFocus();
                m_comboSpellSelect.ShowDropDown();
            }
        }
    }
}

void CSpellsControl::OnSpellSelectOk()
{
    // spell selection complete
    m_comboSpellSelect.ShowWindow(SW_HIDE); // hide control
    int sel = m_comboSpellSelect.GetCurSel();
    if (sel != CB_ERR)
    {
        // get the true index of the item as combo is sorted
        sel = m_comboSpellSelect.GetItemData(sel);
        std::list<TrainedSpell> trainedSpells =
                m_pCharacter->TrainedSpells(m_class, m_editSpellLevel);
        std::vector<Spell> spells = FilterSpells(m_class, m_editSpellLevel);
        RemoveTrained(&spells, NULL);

        // revoke any previous spell selection this selection is replacing
        if (m_editSpellIndex < trainedSpells.size())
        {
            // need to revoke this spells as replacing current selection
            std::list<TrainedSpell>::iterator it = trainedSpells.begin();
            std::advance(it, m_editSpellIndex);
            m_pCharacter->RevokeSpell(m_class, m_editSpellLevel, (*it).SpellName());
        }
        // now train this spell selection
        ASSERT(sel >= 0 && (size_t)sel < spells.size());
        std::vector<Spell>::iterator it = spells.begin();
        std::advance(it, sel);
        m_pCharacter->TrainSpell(m_class, m_editSpellLevel, (*it).Name());
    }
    HideTip();
}

void CSpellsControl::OnSpellSelectCancel()
{
    // spell selection aborted, just hide the control
    m_comboSpellSelect.ShowWindow(SW_HIDE);
    HideTip();
}

void CSpellsControl::RemoveTrained(
        std::vector<Spell> * spells,
        std::string * currentSelection)
{
    // remove already trained spells from the list except the one
    // that is already selected in this slot (if any)
    std::list<TrainedSpell> trainedSpells =
            m_pCharacter->TrainedSpells(m_class, m_editSpellLevel);
    if (m_editSpellIndex < trainedSpells.size())
    {
        // just remove the current selection from the trained list
        std::list<TrainedSpell>::iterator it = trainedSpells.begin();
        std::advance(it, m_editSpellIndex);
        if (currentSelection != NULL)
        {
            *currentSelection = (*it).SpellName();
        }
        trainedSpells.erase(it);    // remove from remove list
    }
    for (size_t si = 0; si < spells->size(); ++si)
    {
        std::list<TrainedSpell>::iterator it = trainedSpells.begin();
        while (it != trainedSpells.end())
        {
            if ((*spells)[si].Name() == (*it).SpellName())
            {
                // need to remove this one
                (*spells).erase((*spells).begin() + si);
                // make sure we check the next as we have removed an index
                --si;
                break;
            }
            ++it;
        }
    }
}

void CSpellsControl::AddFixedSpell(const std::string & spellName, size_t level)
{
    // add the spell at the relevant level
    FixedSpell spell(spellName, level);
    m_fixedSpells[level-1].push_back(spell);
    if (IsWindow(GetSafeHwnd()))
    {
        Invalidate();   // redraw
    }
}

void CSpellsControl::RevokeFixedSpell(const std::string & spellName, size_t level)
{
    // remove the named spell from the relevant level
    FixedSpell spell(spellName, level);
    std::list<FixedSpell>::iterator it = m_fixedSpells[level-1].begin();
    while (it != m_fixedSpells[level-1].end())
    {
        if ((*it) == spell)
        {
            // this is the one to delete
            m_fixedSpells[level-1].erase(it);
            break;
        }
        ++it;
    }
    if (IsWindow(GetSafeHwnd()))
    {
        Invalidate();   // redraw
    }
}

void CSpellsControl::ApplySpellEffects(const std::list<TrainedSpell> & spells)
{
    // first determine the caster level for this class
    size_t casterLevel = CasterLevel();
    // now apply the effects for each spell known
    std::list<TrainedSpell>::const_iterator it = spells.begin();
    while (it != spells.end())
    {
        m_pCharacter->ApplySpellEffects((*it).SpellName(), casterLevel);
        ++it;
    }
}

void CSpellsControl::RevokeSpellEffects(const std::list<TrainedSpell> & spells)
{
    // first determine the caster level for this class
    size_t casterLevel = CasterLevel();
    // now revoke the effects for each spell known
    std::list<TrainedSpell>::const_iterator it = spells.begin();
    while (it != spells.end())
    {
        m_pCharacter->RevokeSpellEffects((*it).SpellName(), casterLevel);
        ++it;
    }
}

size_t CSpellsControl::CasterLevel() const
{
    size_t casterLevel = m_pCharacter->ClassLevels(m_class);
    switch (m_class)
    {
    case Class_Artificer:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Artificer)->Total();
        break;
    case Class_Cleric:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Cleric)->Total();
        break;
    case Class_Druid:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Druid)->Total();
        break;
    case Class_FavoredSoul:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_FavoredSoul)->Total();
        break;
    case Class_Paladin:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Paladin)->Total();
        break;
    case Class_Sorcerer:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Sorcerer)->Total();
        break;
    case Class_Ranger:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Ranger)->Total();
        break;
    case Class_Warlock:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Warlock)->Total();
        break;
    case Class_Wizard:
        casterLevel = (size_t)FindBreakdown(Breakdown_CasterLevel_Wizard)->Total();
        break;
    default:
        break;
    }
    return casterLevel;
}

CSize CSpellsControl::RequiredSize()
{
    CSize size(0, 0);
    // need to measure the standard text size for basic width.
    CDC * pDC = GetDC();
    pDC->SaveDC();
    pDC->SelectStockObject(DEFAULT_GUI_FONT);
    CString levelLabel("Level 1");
    CSize labelSize = pDC->GetTextExtent(levelLabel);
    size.cx = labelSize.cx;
    for (size_t spellLevel = 0; spellLevel < m_spellsPerLevel.size(); ++spellLevel)
    {
        // how many spells at this level?
        size_t count = m_spellsPerLevel[spellLevel];
        int cx = 0;
        if (count > 0)
        {
            int top = spellLevel * (c_spellSlotImageSize + c_controlSpacing);
            size.cy = max(size.cy, (int)(top + c_spellSlotImageSize));

            cx += (c_spellSlotImageSize + c_controlSpacing) * m_fixedSpells[spellLevel].size();
            cx += (c_spellSlotImageSize + c_controlSpacing) * count;
        }
        size.cx = max(size.cx, labelSize.cx + cx);
    }
    size.cx += c_controlSpacing;
    size.cy += c_controlSpacing;
    pDC->RestoreDC(-1);
    ReleaseDC(pDC);
    return size;
}

void CSpellsControl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    // its horizontal scroll bar 
    int nCurPos = m_scrollHorizontal.GetScrollPos();
    int nPrevPos = nCurPos;
    // decide what to do for each different scroll event
    switch(nSBCode)
        {
        case SB_LEFT:
            nCurPos = 0;
            break;
        case SB_RIGHT:
            nCurPos = m_scrollHorizontal.GetScrollLimit()-1;
            break;
        case SB_LINELEFT:
            nCurPos = max(nCurPos - 6, 0);
            break;
        case SB_LINERIGHT:
            nCurPos = min(nCurPos + 6, m_scrollHorizontal.GetScrollLimit()-1);
            break;
        case SB_PAGELEFT:
            nCurPos = max(nCurPos - m_clientSize.Width(), 0);
            break;
        case SB_PAGERIGHT:
            nCurPos = min(nCurPos + m_clientSize.Width(), m_scrollHorizontal.GetScrollLimit()-1);
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            nCurPos = nPos;
            break;
        }

    m_scrollHorizontal.SetScrollPos(nCurPos);
    Invalidate();
}

void CSpellsControl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    // its horizontal scroll bar 
    int nCurPos = m_scrollVertical.GetScrollPos();
    int nPrevPos = nCurPos;
    // decide what to do for each different scroll event
    switch(nSBCode)
        {
        case SB_LEFT:
            nCurPos = 0;
            break;
        case SB_RIGHT:
            nCurPos = m_scrollVertical.GetScrollLimit()-1;
            break;
        case SB_LINELEFT:
            nCurPos = max(nCurPos - 6, 0);
            break;
        case SB_LINERIGHT:
            nCurPos = min(nCurPos + 6, m_scrollVertical.GetScrollLimit()-1);
            break;
        case SB_PAGELEFT:
            nCurPos = max(nCurPos - m_clientSize.Height(), 0);
            break;
        case SB_PAGERIGHT:
            nCurPos = min(nCurPos + m_clientSize.Height(), m_scrollVertical.GetScrollLimit()-1);
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            nCurPos = nPos;
            break;
        }

    m_scrollVertical.SetScrollPos(nCurPos);
    Invalidate();
}

void CSpellsControl::ProcessScrollBars(int cx, int cy)
{
    bool newHVis = false;
    bool newVVis = false;
    // setup the scrollbars if required
    if (cx < m_clientSize.Width())
    {
        newHVis = true;
    }
    // do the same for the vertical scrollbar
    if (cy < m_clientSize.Height())
    {
        newVVis = true;
    }
    // check to see whether showing 1 scrollbar causes the other to need to be
    // shown
    if (newVVis
            && !newHVis
            && cx - GetSystemMetrics(SM_CXVSCROLL) < m_clientSize.Width())
    {
        newHVis = true;
    }
    if (newHVis
            && !newVVis
            && cy - GetSystemMetrics(SM_CYHSCROLL) < m_clientSize.Height())
    {
        newVVis = true;
    }
    // locations the scroll bars will be displayed
    CRect rctHorz(0, cy - GetSystemMetrics(SM_CYHSCROLL), cx, cy);
    CRect rctVert(cx - GetSystemMetrics(SM_CXVSCROLL), 0, cx, cy);

    // if there has been a change in visibility state, get the scrollbars
    // allow correct scrolling
    //if (newHVis
    //        && !m_bHVisible)
    //{
    //    cx -= GetSystemMetrics(SM_CXVSCROLL); // lose the space used by the scrollbar
    //}
    //if (newVVis
    //        && !m_bVVisible)
    //{
    //    cy -= GetSystemMetrics(SM_CYHSCROLL); // lose the space used by the scrollbar
    //}
    // show/hide the horizontal scrollbars
    m_scrollHorizontal.ShowWindow(newHVis ? SW_SHOW : SW_HIDE);
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = cx;
    if (newVVis)
    {
        si.nMax = m_clientSize.Width() + GetSystemMetrics(SM_CXVSCROLL);
    }
    else
    {
        si.nMax = m_clientSize.Width();
    }
    si.nMin = 0;
    m_scrollHorizontal.SetScrollInfo(&si);

    // show/hide/setup the vertical scrollbar
    m_scrollVertical.ShowWindow(newVVis ? SW_SHOW : SW_HIDE);
    si.nPage = cy;
    if (newVVis)
    {
        si.nMax = m_clientSize.Height() + GetSystemMetrics(SM_CYHSCROLL);
    }
    else
    {
        si.nMax = m_clientSize.Height();
    }
    si.nMin = 0;
    m_scrollVertical.SetScrollInfo(&si);
    // save state of scrollbars
    m_bHVisible = newHVis;
    m_bVVisible = newVVis;
    if (m_bHVisible)
    {
        rctVert.bottom -= GetSystemMetrics(SM_CYHSCROLL);
    }
    if (m_bVVisible)
    {
        rctHorz.right -= GetSystemMetrics(SM_CXVSCROLL);
    }
    m_scrollHorizontal.MoveWindow(rctHorz);
    m_scrollVertical.MoveWindow(rctVert);
}
