// FeatsClassControl.cpp : implementation file
//
#include "stdafx.h"
#include "FeatsClassControl.h"
#include "GlobalSupportFunctions.h"
#include <algorithm>

// CFeatsClassControl
namespace
{
    const int c_FeatColumnWidth = 200;      // pixels
    const UINT UWM_UPDATE = ::RegisterWindowMessage(_T("Update"));
    const UINT UWM_TOGGLE_INCLUDED = ::RegisterWindowMessage(_T("ToggleIgnoreItem"));
    const int c_dudLevel = MAX_LAMANNIA_LEVEL + 1;
}

IMPLEMENT_DYNAMIC(CFeatsClassControl, CWnd)

CFeatsClassControl::CFeatsClassControl() :
    m_pCharacter(NULL),
    m_bMenuDisplayed(false),
    m_maxRequiredFeats(0),
    m_numClassColumns(0),
    m_bUpdatePending(false),
    m_highlightedLevelLine(c_dudLevel),   // starts invalid
    m_showingTip(false),
    m_tipCreated(false),
    m_tooltipItem(HT_None, CRect(0, 0, 0, 0), 0, 0),
    m_featSelectItem(HT_None, CRect(0, 0, 0, 0), 0, 0),
    m_bAlternateFeat(false)
{
    m_featSelector.SetCanRemoveItems();
    m_classImagesSmall.Create(16, 15, ILC_COLOR24, 0, Class_Count);
    CBitmap images;
    images.LoadBitmap(IDR_MENUICONS_TOOLBAR);
    m_classImagesSmall.Add(&images, (CBitmap*)NULL);

    CBitmap classImagesLarge;
    m_classImagesLarge.Create(32, 32, ILC_COLOR24, 0, Class_Count);
    classImagesLarge.LoadBitmap(IDR_CLASS_IMAGES_LARGE);
    m_classImagesLarge.Add(&classImagesLarge, (CBitmap*)NULL);

    // create the image list for the feats
    const std::list<Feat> & allFeats = StandardFeats();
    m_imagesFeats.Create(
            32,
            32,
            ILC_COLOR32,
            0,
            allFeats.size());
    std::list<Feat>::const_iterator it = allFeats.begin();
    while (it != allFeats.end())
    {
        (*it).AddImage(&m_imagesFeats);
        ++it;
    }
}

CFeatsClassControl::~CFeatsClassControl()
{
}

void CFeatsClassControl::SetCharacter(Character * pCharacter)
{
    m_hitChecks.clear();
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
    SetupControl();
    Invalidate();
}

#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CFeatsClassControl, CWnd)
    ON_WM_PAINT()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_MBUTTONUP()
    ON_WM_RBUTTONUP()
    ON_REGISTERED_MESSAGE(UWM_UPDATE, UpdateControl)
    ON_CBN_SELENDOK(IDC_COMBO_FEATSELECT, OnFeatSelectOk)
    ON_CBN_SELENDCANCEL(IDC_COMBO_FEATSELECT, OnFeatSelectCancel)
    ON_MESSAGE(WM_MOUSEHOVER, OnHoverComboBox)
    ON_REGISTERED_MESSAGE(UWM_TOGGLE_INCLUDED, OnToggleFeatIgnore)
END_MESSAGE_MAP()
#pragma warning(pop)

// CFeatsClassControl message handlers
void CFeatsClassControl::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
}

CSize CFeatsClassControl::RequiredSize()
{
    CSize requiredSize(0, 0);
    if (m_pCharacter != NULL)
    {
        SetupControl();

        CDC screenDC;
        screenDC.Attach(::GetDC(NULL));
        screenDC.SaveDC();
        screenDC.SelectStockObject(DEFAULT_GUI_FONT);

        // calculate the width of the control required
        CSize levelCol = screenDC.GetTextExtent(" 30 ");
        levelCol.cy = max(20, levelCol.cy); // min height required for icon + border
        // blank item in top left
        CRect rctItem(0, 0, levelCol.cx, levelCol.cy);
        m_levelRect = rctItem;
        int width = m_levelRect.Width();
        for (size_t i = 0; i < m_numClassColumns; ++i)
        {
            width += 36;  // 36 pixels per class rect
        }
        for (size_t i = 0; i < m_maxRequiredFeats; ++i)
        {
            width += c_FeatColumnWidth;
        }
        // additional columns for stats [bab][str][dex][con][int][wis][cha]
        for (size_t stats = Ability_Unknown; stats < Ability_Count; ++stats)
        {
            CString strText;
            if (stats != Ability_Unknown)
            {
                strText.Format(" %3.3s ", EnumEntryText((AbilityType)stats, abilityTypeMap));
            }
            else
            {
                strText = " BAB ";
            }
            strText = " " + strText + " ";
            CSize csText = screenDC.GetTextExtent(strText);
            width += (csText.cx + 1);
        }
        int height = m_levelRect.Height();
        for (size_t level = 0; level < m_pCharacter->MaxLevel(); ++level)
        {
            bool bFeatsAvailable = (m_availableFeats[level].size() > 0);
            if (bFeatsAvailable)
            {
                // 36 pixels for height for this item
                height += 36;
            }
            else
            {
                height += 21;
            }
        }
        requiredSize.cx = width;
        requiredSize.cy = height;
        screenDC.RestoreDC(-1);
        ::ReleaseDC(NULL, screenDC.Detach());
    }
    return requiredSize;
}

void CFeatsClassControl::SetupControl()
{
    LockWindowUpdate();
    if (!m_tipCreated)
    {
        m_tooltip.Create(this);
        m_tooltip2.Create(this, true);
        m_tipCreated = true;
    }
    if (!IsWindow(m_featSelector.GetSafeHwnd()))
    {
        m_featSelector.Create(
                WS_CHILD | WS_VSCROLL | WS_TABSTOP
                | CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS,
                CRect(77,36,119,250),
                this,
                IDC_COMBO_FEATSELECT);
        CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
        m_featSelector.SetFont(pDefaultGUIFont, TRUE);
    }
    // work out the available feat slots at each level
    m_availableFeats.clear();
    m_maxRequiredFeats = 0;
    m_numClassColumns = 0;
    if (m_pCharacter != NULL)
    {
        m_numClassColumns++;    // always at least 1 class column
        if (m_pCharacter->Class1() != Class_Unknown)
        {
            m_numClassColumns++;        // need to show class 2 column
            if (m_pCharacter->Class2() != Class_Unknown)
            {
                m_numClassColumns++;        // need to show class 3 column
            }
        }
        for (size_t level = 0; level < m_pCharacter->MaxLevel(); ++level)
        {
            std::vector<TrainableFeatTypes> availableFeatSlots =
                    m_pCharacter->TrainableFeatTypeAtLevel(level);
            m_availableFeats.push_back(availableFeatSlots);
            m_maxRequiredFeats = max(m_maxRequiredFeats, availableFeatSlots.size());
        }
    }
    UnlockWindowUpdate();
}

void CFeatsClassControl::OnPaint()
{
    CPaintDC dc(this);
    CRect rctWindow;
    GetWindowRect(rctWindow);
    ScreenToClient(rctWindow);

    // double buffer drawing to avoid flickering
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(
            &dc,
            rctWindow.Width(),
            rctWindow.Height());
    // draw to a compatible device context and then splat to screen
    // to avoid flickering of the control
    CDC memoryDc;
    memoryDc.CreateCompatibleDC(&dc);
    memoryDc.SaveDC();
    memoryDc.SelectObject(&bitmap);
    memoryDc.SelectStockObject(DEFAULT_GUI_FONT);
    memoryDc.SetBkMode(TRANSPARENT);

    CBrush selectedBackgroundBrush;
    CBrush normalBackgroundBrush;
    selectedBackgroundBrush.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
    normalBackgroundBrush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

    // fill background
    memoryDc.FillRect(rctWindow, &normalBackgroundBrush);
    memoryDc.Draw3dRect(
            rctWindow,
            ::GetSysColor(COLOR_BTNHIGHLIGHT),
            ::GetSysColor(COLOR_BTNSHADOW));

    if (m_pCharacter != NULL)
    {
        // the height of all other items depends on whether there are feat
        // selections available for that level.
        size_t top = DrawTopLine(&memoryDc, rctWindow);
        for (size_t level = 0; level < m_pCharacter->MaxLevel(); ++level)
        {
            m_levelRects[level] = CRect(0, top, rctWindow.right, top);
            top = DrawLevelLine(&memoryDc, top, level, rctWindow);
            m_levelRects[level].bottom = top - 1;
        }
    }
    // now draw to display
    dc.BitBlt(
            0,
            0,
            rctWindow.Width(),
            rctWindow.Height(),
            &memoryDc,
            0,
            0,
            SRCCOPY);
    memoryDc.RestoreDC(-1);
    memoryDc.DeleteDC();
}

size_t CFeatsClassControl::DrawTopLine(
        CDC * pDC,
        CRect rctWindow)
{
    m_hitChecks.clear();

    CBrush selectedBackgroundBrush;
    CBrush normalBackgroundBrush;
    selectedBackgroundBrush.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
    normalBackgroundBrush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

    // [][class1^][class2^][class3^][Feat1    ][Feat2    ][Feat3    ][str][dex][con][int][wis][cha][BAB]
    // level column, get its max width
    CSize levelCol = pDC->GetTextExtent(" 30 ");
    levelCol.cy = max(20, levelCol.cy); // min height required for icon + border
    // blank item in top left
    CRect rctItem(0, 0, levelCol.cx, levelCol.cy);
    m_levelRect = rctItem;
    CPoint pos(rctItem.right + 4, 2);
    // class columns (variable)
    for (size_t cc = 0; cc < m_numClassColumns; ++cc)
    {
        rctItem.left = rctItem.right + 1;
        rctItem.right = rctItem.left + 35;
        pDC->Draw3dRect(
                rctItem,
                ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));
        ClassType ct = m_pCharacter->Class(cc);
        m_classImagesSmall.Draw(pDC, ct, pos, ILD_TRANSPARENT);
        CRect rctDropArrow(pos.x, pos.y, pos.x + 12, pos.y + 12);
        rctDropArrow += CPoint(18, 2);
        pDC->DrawFrameControl(rctDropArrow, DFC_SCROLL, DFCS_SCROLLDOWN);
        HitCheckItem classDropArrow((HitType)(HT_Class1 + cc), rctDropArrow, 0, 0);
        m_hitChecks.push_back(classDropArrow);
        pos.x += rctItem.Width();
        m_classRects[cc] = rctItem; // copy for later use
    }
    // feat columns (variable)
    for (size_t fc = 0; fc < m_maxRequiredFeats; ++fc)
    {
        rctItem.left = rctItem.right + 1;
        rctItem.right = rctItem.left + c_FeatColumnWidth;
        pDC->Draw3dRect(
                rctItem,
                ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));
        CSize csText = pDC->GetTextExtent("1"); // just need the height of text
        pDC->TextOut(
                rctItem.left,
                rctItem.top + (rctItem.Height() - csText.cy) / 2,
                " Feat Selection");
        pos.x += rctItem.Width();
        m_featRects[fc] = rctItem;      // copy for later use
    }
    // additional columns for stats [str][dex][con][int][wis][cha][bab]
    for (size_t stats = Ability_Unknown; stats < Ability_Count; ++stats)
    {
        CString strText;
        if (stats != Ability_Unknown)
        {
            strText.Format(" %3.3s ", EnumEntryText((AbilityType)stats, abilityTypeMap));
        }
        else
        {
            strText = " BAB ";
        }
        strText = " " + strText + " ";
        CSize csText = pDC->GetTextExtent(strText);
        rctItem.left = rctItem.right + 1;
        rctItem.right = rctItem.left + csText.cx;
        pDC->Draw3dRect(
                rctItem,
                ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));
        pDC->TextOut(
                rctItem.left,
                rctItem.top + (rctItem.Height() - csText.cy) / 2,
                strText);
        pos.x += rctItem.Width();
        m_statRects[stats] = rctItem;      // copy for later use
    }
    return levelCol.cy + 1;
}

size_t CFeatsClassControl::DrawLevelLine(
        CDC * pDC,
        size_t top,
        size_t level,
        CRect rctWindow)
{
    // switch the fill colour and text colour if this is the highlighted line
    CBrush fillBrush;
    if (level == m_highlightedLevelLine)
    {
        pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        fillBrush.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
    }
    else
    {
        pDC->SetTextColor(RGB(0, 0, 0));  // black text
        fillBrush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
    }
    CRect rctItem = m_levelRect;
    const std::vector<TrainableFeatTypes>& featOptions = m_availableFeats[level];
    bool bFeatsAvailable = (featOptions.size() > 0);
    rctItem.top = top;
    if (bFeatsAvailable)
    {
        // 36 pixels for height for this item
        rctItem.bottom = top + 35;
    }
    else
    {
        rctItem.bottom = top + m_levelRect.Height();
    }
    pDC->Draw3dRect(
            rctItem,
            ::GetSysColor(COLOR_BTNHIGHLIGHT),
            ::GetSysColor(COLOR_BTNSHADOW));
    CRect rctFill(rctItem);
    rctFill.DeflateRect(1, 1, 1, 1);
    pDC->FillRect(rctFill, &fillBrush);
    CString text;
    text.Format("%d", level + 1);
    CSize csText = pDC->GetTextExtent(text);
    pDC->TextOut(
            rctItem.left + (rctItem.Width() - csText.cx) / 2,
            rctItem.top + (rctItem.Height() - csText.cy) / 2,
            text);
    // draw the selected class icon
    ClassType classSelection = m_pCharacter->LevelData(level).HasClass()
            ? m_pCharacter->LevelData(level).Class()
            : Class_Unknown;
    for (size_t cc = 0; cc < m_numClassColumns; ++cc)
    {
        rctItem.left = m_classRects[cc].left;
        rctItem.right = m_classRects[cc].right;
        pDC->Draw3dRect(
                rctItem,
                ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));
        CRect rctFill(rctItem);
        rctFill.DeflateRect(1, 1, 1, 1);
        pDC->FillRect(rctFill, &fillBrush);
        ClassType ct = m_pCharacter->Class(cc);
        // only drawn if it matches current class selection
        if (m_pCharacter->Class(cc) == classSelection
                || (classSelection == Class_Epic && cc == 0)
                || (classSelection == Class_Legendary && cc == 0))
        {
            if (bFeatsAvailable)
            {
                // draw the class using the large icon
                CPoint pos = rctItem.TopLeft();
                pos.x += (rctItem.Width() - 32) / 2;    // centre it
                pos.y += (rctItem.Height() - 32) / 2;    // centre it
                m_classImagesLarge.Draw(
                        pDC,
                        classSelection,
                        pos,
                        ILD_TRANSPARENT);
            }
            else
            {
                // draw class using the small icon
                CPoint pos = rctItem.TopLeft();
                pos.x += (rctItem.Width() - 16) / 2;    // centre it
                pos.y += (rctItem.Height() - 15) / 2;    // centre it
                m_classImagesSmall.Draw(
                        pDC,
                        classSelection,
                        pos,
                        ILD_TRANSPARENT);
            }
        }
        // add the hit check for this item
        HitCheckItem classSelect((HitType)(HT_LevelClass1 + cc), rctItem, level, 0);
        m_hitChecks.push_back(classSelect);
    }
    // draw the feat selections (if any)
    for (size_t fc = 0; fc < m_maxRequiredFeats; ++fc)
    {
        rctItem.left = m_featRects[fc].left;
        rctItem.right = m_featRects[fc].right;
        pDC->Draw3dRect(
                rctItem,
                ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));
        CRect rctFill(rctItem);
        rctFill.DeflateRect(1, 1, 1, 1);
        pDC->FillRect(rctFill, &fillBrush);
        DrawFeat(pDC, rctItem, fc, level);
    }
    // additional columns for stats [str][dex][con][int][wis][cha][bab]
    for (size_t stats = Ability_Unknown; stats < Ability_Count; ++stats)
    {
        rctItem.left = m_statRects[stats].left;
        rctItem.right = m_statRects[stats].right;
        pDC->Draw3dRect(
                rctItem,
                ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));
        CRect rctFill(rctItem);
        rctFill.DeflateRect(1, 1, 1, 1);
        pDC->FillRect(rctFill, &fillBrush);
        CString strText;
        size_t value;
        switch (stats)
        {
        case Ability_Unknown:       value = m_pCharacter->BaseAttackBonus(level); break;
        case Ability_Strength:      value = m_pCharacter->AbilityAtLevel(Ability_Strength, level, true); break;
        case Ability_Dexterity:     value = m_pCharacter->AbilityAtLevel(Ability_Dexterity, level, true); break;
        case Ability_Constitution:  value = m_pCharacter->AbilityAtLevel(Ability_Constitution, level, true); break;
        case Ability_Intelligence:  value = m_pCharacter->AbilityAtLevel(Ability_Intelligence, level, true); break;
        case Ability_Wisdom:        value = m_pCharacter->AbilityAtLevel(Ability_Wisdom, level, true); break;
        case Ability_Charisma:      value = m_pCharacter->AbilityAtLevel(Ability_Charisma, level, true); break;
        }
        strText.Format("%d", value);
        CSize csText = pDC->GetTextExtent(strText);
        pDC->TextOut(
                rctItem.left + (rctItem.Width() - csText.cx) / 2,
                rctItem.top + (rctItem.Height() - csText.cy) / 2,
                strText);
    }
    // set top of next item
    top = rctItem.bottom + 1;
    return top;
}

void CFeatsClassControl::DrawFeat(
        CDC * pDC,
        CRect rctItem,
        size_t featIndex,
        size_t level)
{
    // if there an actual feat in this slot?
    std::vector<TrainableFeatTypes> tfts = m_availableFeats[level];
    if (tfts.size() > featIndex)
    {
        // add the hit check location for this feat
        HitCheckItem feat(HT_Feat, rctItem, level, featIndex);
        m_hitChecks.push_back(feat);

        // if we have an actual trained feat, draw it
        TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                level,
                tfts[featIndex]);
        // name is blank if no feat currently trained
        if (tf.FeatName() != "")
        {
            // does this tf have an alternate?
            if (tf.HasAlternateFeatName())
            {
                // draw the alternate and the main feat item
                size_t imageIndex = FeatImageIndex(tf.AlternateFeatName());
                CPoint pos = rctItem.TopLeft();
                pos.x += 2;
                pos.y += 1;
                m_imagesFeats.Draw(
                        pDC,
                        imageIndex,
                        pos,
                        ILD_TRANSPARENT);
                // now draw the main feat icon
                imageIndex = FeatImageIndex(tf.FeatName());
                pos = rctItem.TopLeft();
                pos.x += 18;        // half way across previous icon
                pos.y += 1;
                m_imagesFeats.Draw(
                        pDC,
                        imageIndex,
                        pos,
                        ILD_TRANSPARENT);
                const Feat & feat = FindFeat(tf.FeatName());
                const Feat & featAlt = FindFeat(tf.AlternateFeatName());
                CString text = TrainableFeatTypeLabel(tfts[featIndex]);
                CString displayText = feat.Name().c_str();
                displayText += "\r\n";
                displayText += text;
                displayText += " Alternate: ";
                displayText += featAlt.Name().c_str();
                // show the feat name
                rctItem.left += 54; // space away from the icon
                CRect rctDummy(rctItem);
                int height = pDC->DrawText(displayText, rctDummy, DT_END_ELLIPSIS | DT_CALCRECT);
                rctItem.top += (rctItem.Height() - height) / 2; // center vertically
                pDC->DrawText(displayText, rctItem, DT_END_ELLIPSIS);
            }
            else
            {
                // draw the selected feat
                // first draw its icon
                size_t imageIndex = FeatImageIndex(tf.FeatName());
                CPoint pos = rctItem.TopLeft();
                pos.x += 2;
                pos.y += 1;
                m_imagesFeats.Draw(
                        pDC,
                        imageIndex,
                        pos,
                        ILD_TRANSPARENT);
                const Feat & feat = FindFeat(tf.FeatName());
                CString text = TrainableFeatTypeLabel(tfts[featIndex]);
                CString displayText = feat.Name().c_str();
                displayText += "\r\n";
                displayText += text;
                // show the feat name
                rctItem.left += 36; // space away from the icon
                CRect rctDummy(rctItem);
                int height = pDC->DrawText(displayText, rctDummy, DT_END_ELLIPSIS | DT_CALCRECT);
                rctItem.top += (rctItem.Height() - height) / 2; // center vertically
                pDC->DrawText(displayText, rctItem, DT_END_ELLIPSIS);
            }
        }
        else
        {
            // draw a feat that needs to be selected.
            // we can re-use the class [?] unknown icon
            CPoint pos = rctItem.TopLeft();
            pos.x += 2;
            pos.y += 1;
            m_classImagesLarge.Draw(
                    pDC,
                    Class_Unknown,
                    pos,
                    ILD_TRANSPARENT);
            CString text = TrainableFeatTypeLabel(tfts[featIndex]);
            CString displayText = "Click to select a feat\r\n";
            displayText += text;
            // show the feat name
            rctItem.left += 36; // space away from the icon
            CRect rctDummy(rctItem);
            int height = pDC->DrawText(displayText, rctDummy, DT_END_ELLIPSIS | DT_CALCRECT);
            rctItem.top += (rctItem.Height() - height) / 2; // center vertically
            pDC->DrawText(displayText, rctItem, DT_END_ELLIPSIS);
        }
    }
    else
    {
        // no trainable feat, leave the position blank
    }
}

LRESULT CFeatsClassControl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // if the menu is being displayed, leave the item highlighted in the control
    // gets re-drawn when menu is dismissed/closed
    if (!m_bMenuDisplayed)
    {
        m_highlightedLevelLine = c_dudLevel;
        Invalidate();
    }
    if (m_showingTip)
    {
        HideTip();
        m_tooltipItem = HitCheckItem(HT_None, CRect(0, 0, 0, 0), 0, 0);
    }
    return 0;
}


void CFeatsClassControl::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_pCharacter != NULL)
    {
        // determine which row the mouse is over, and change highlight if required
        size_t overLevel = c_dudLevel;
        for (size_t level = 0; level < m_pCharacter->MaxLevel(); ++level)
        {
            if (m_levelRects[level].PtInRect(point))
            {
                overLevel = level;
                break;
            }
        }
        if (overLevel != m_highlightedLevelLine)
        {
            m_highlightedLevelLine = overLevel;
            Invalidate();
        }
        // hit check the mouse location and display feat tooltips if required
        HitCheckItem ht = HitCheck(point);
        if (ht.Type() == HT_Feat)
        {
            // mouse is over a feat, display tooltip if its a trained feat
            if (m_showingTip
                    && ht.Type() == m_tooltipItem.Type()
                    && ht.Level() == m_tooltipItem.Level()
                    && ht.Data() == m_tooltipItem.Data())
            {
                // already showing the right tip, do nothing
            }
            else
            {
                HideTip();
                m_tooltipItem = HitCheckItem(HT_None, CRect(0, 0, 0, 0), 0, 0);
                // get the selected feat name (if there is one)
                std::vector<TrainableFeatTypes> tfts = m_availableFeats[ht.Level()];
                if (tfts.size() > ht.Data())
                {
                    // if we have an actual trained feat, draw it
                    TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                            ht.Level(),
                            tfts[ht.Data()]);
                    std::string featName = tf.FeatName();
                    if (featName != "")
                    {
                        const Feat & feat = FindFeat(tf.FeatName());
                        // determine whether there is a feat swap warning if training at level 1
                        bool warn = false;
                        if (ht.Level() == 0
                                && tfts[ht.Data()] != TFT_Automatic
                                && tfts[ht.Data()] != TFT_GrantedFeat
                                && featName != " No Selection")
                        {
                            warn = !m_pCharacter->IsFeatTrainable(
                                    ht.Level(),
                                    tfts[ht.Data()],
                                    feat,
                                    false,
                                    true)
                                    || (featName == "Completionist" && (ht.Level() == 0));
                        }
                        CPoint tipTopLeft(ht.Rect().left, ht.Rect().bottom);
                        CPoint tipAlternate(ht.Rect().left, ht.Rect().top);
                        ClientToScreen(&tipTopLeft);
                        ClientToScreen(&tipAlternate);
                        m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
                        m_tooltip.SetFeatItem(*m_pCharacter, &feat, warn, ht.Level(), false);
                        m_tooltip.Show();
                        m_showingTip = true;
                        m_tooltipItem = ht;
                        if (tf.HasAlternateFeatName())
                        {
                            CDC * pDC = GetDC();
                            CSize windowSize;
                            m_tooltip.GetWindowSize(pDC, &windowSize);
                            ReleaseDC(pDC);
                            const Feat & feat = FindFeat(tf.AlternateFeatName());
                            tipTopLeft += CPoint(0, windowSize.cy + 5);
                            tipAlternate -= CPoint(0, windowSize.cy + 5);
                            m_tooltip2.SetOrigin(tipTopLeft, tipAlternate, false);
                            m_tooltip2.SetFeatItem(*m_pCharacter, &feat, false, ht.Level(), false);
                            m_tooltip2.Show();
                        }
                    }
                }
            }
        }
        else if (ht.Type() == HT_LevelClass1
                || ht.Type() == HT_LevelClass2
                || ht.Type() == HT_LevelClass3)
        {
            // look up the selected class
            const LevelTraining & levelData = m_pCharacter->LevelData(ht.Level());
            ClassType expectedClass = levelData.HasClass() ? levelData.Class() : Class_Unknown;
            if (ht.Level() == 0)
            {
                switch (m_pCharacter->Race())
                {
                case Race_AasimarScourge: expectedClass = Class_Ranger; break;
                case Race_BladeForged: expectedClass = Class_Paladin; break;
                case Race_DeepGnome: expectedClass = Class_Wizard; break;
                case Race_KoboldShamen: expectedClass = Class_Sorcerer; break;
                case Race_Morninglord: expectedClass = Class_Cleric; break;
                case Race_PurpleDragonKnight: expectedClass = Class_Fighter; break;
                case Race_RazorclawShifter: expectedClass = Class_Barbarian; break;
                case Race_ShadarKai: expectedClass = Class_Rogue; break;
                case Race_TabaxiTrailblazer: expectedClass = Class_Monk; break;
                case Race_TieflingScoundrel: expectedClass = Class_Bard; break;
                }
            }
            CPoint tipTopLeft(ht.Rect().left, ht.Rect().bottom);
            CPoint tipAlternate(ht.Rect().left, ht.Rect().top);
            ClientToScreen(&tipTopLeft);
            ClientToScreen(&tipAlternate);
            m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
            m_tooltip.SetLevelItem(*m_pCharacter, ht.Level(), &levelData, expectedClass);
            m_tooltip.Show();
            m_showingTip = true;
            m_tooltipItem = ht;
        }
        else
        {
            HideTip();
            m_tooltipItem = HitCheckItem(HT_None, CRect(0, 0, 0, 0), 0, 0);
        }
        // track the mouse so we know when it leaves our window
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = 1;
        _TrackMouseEvent(&tme);

        __super::OnMouseMove(nFlags, point);
    }
}

void CFeatsClassControl::OnLButtonUp(UINT nFlags, CPoint point)
{
    // so whether the user has clicked on a specific item
    GetCursorPos(&point);
    ScreenToClient(&point);
    HitCheckItem ht = HitCheck(point);
    switch (ht.Type())
    {
    case HT_None:
        // not on anything that can be interacted with
        break;
    case HT_Class1:
        // show the class select drop list menu
        DoClass1();
        break;
    case HT_Class2:
        // show the class select drop list menu
        DoClass2();
        break;
    case HT_Class3:
        // show the class select drop list menu
        DoClass3();
        break;
    case HT_LevelClass1:
        // set the class for this level if its not already this class
        SetClassLevel(m_pCharacter->Class(0), ht.Level());
        break;
    case HT_LevelClass2:
        // set the class for this level if its not already this class
        SetClassLevel(m_pCharacter->Class(1), ht.Level());
        break;
    case HT_LevelClass3:
        // set the class for this level if its not already this class
        SetClassLevel(m_pCharacter->Class(2), ht.Level());
        break;
    case HT_Feat:
        {
            m_featSelectItem = ht;
            std::vector<TrainableFeatTypes> tfts = m_availableFeats[ht.Level()];
            TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                    ht.Level(),
                    tfts[ht.Data()]);
            // get a list of the available feats at current level that the user can train
            std::vector<Feat> availableFeats = m_pCharacter->TrainableFeats(
                    tfts[ht.Data()],
                    ht.Level(),
                    tf.FeatName());
            // include a "No Selection" feat, if > 1 feat in list
            // (i.e. user cannot un-train single feat selections)
            if (availableFeats.size() > 1
                    && tf.FeatName() != "")         // must already have a selection also
            {
                Feat feat;
                feat.Set_Name(" No Selection");     // space causes to appear at top of list
                feat.Set_Description("Clears the current feat selection");
                feat.Set_Icon("NoSelection");
                availableFeats.push_back(feat);
            }
            std::sort(availableFeats.begin(), availableFeats.end());

            // lock the window to stop flickering while updates are done
            m_featSelector.LockWindowUpdate();
            m_featSelector.SetImageList(NULL);
            m_featSelector.ResetContent();

            m_imagesFeatSelector.DeleteImageList();
            m_imagesFeatSelector.Create(
                    32,             // all icons are 32x32 pixels
                    32,
                    ILC_COLOR32,
                    0,
                    availableFeats.size());
            std::vector<Feat>::const_iterator it = availableFeats.begin();
            while (it != availableFeats.end())
            {
                (*it).AddImage(&m_imagesFeatSelector);
                ++it;
            }
            m_featSelector.SetImageList(&m_imagesFeatSelector);

            // now add the feat names to the combo control
            int sel = CB_ERR;
            it = availableFeats.begin();
            size_t featIndex = 0;
            while (it != availableFeats.end())
            {
                char buffer[_MAX_PATH];
                strcpy_s(buffer, (*it).Name().c_str());
                size_t pos = m_featSelector.AddString((*it).Name().c_str());
                m_featSelector.SetItemData(pos, featIndex);
                if (tf.FeatName() == (*it).Name())
                {
                    sel = pos;
                }
                ++featIndex;
                ++it;
            }
            m_bAlternateFeat = false;
            m_featSelector.SetCurSel(sel);
            m_featSelector.UnlockWindowUpdate();
            // position the drop list combo over the feat selection position
            CRect comboRect(ht.Rect());
            comboRect.bottom = comboRect.top + 960;   // 20 items visible in drop list
            m_featSelector.MoveWindow(comboRect);
            m_featSelector.SetCurSel(sel);
            m_featSelector.SetDroppedWidth(350); // wider to show extra text
            // set control visible to the user
            m_featSelector.ShowWindow(SW_SHOW);
            m_featSelector.SetFocus();
            m_featSelector.ShowDropDown();
        }
        break;
    }
}

void CFeatsClassControl::OnMButtonUp(UINT nFlags, CPoint point)
{
    // the middle button allows selection of an alternate feat only
    // for a previously trained feat
    GetCursorPos(&point);
    ScreenToClient(&point);
    HitCheckItem ht = HitCheck(point);
    if (ht.Type() == HT_Feat)
    {
        std::vector<TrainableFeatTypes> tfts = m_availableFeats[ht.Level()];
        TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                ht.Level(),
                tfts[ht.Data()]);
        if (tf.FeatName() != "")
        {
            m_featSelectItem = ht;
            std::vector<TrainableFeatTypes> tfts = m_availableFeats[ht.Level()];
            TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                    ht.Level(),
                    tfts[ht.Data()]);
            // get a list of the available feats at current level that the user can train
            std::vector<Feat> availableFeats = m_pCharacter->TrainableFeats(
                    tfts[ht.Data()],
                    ht.Level(),
                    tf.FeatName());
            // include a "No Selection" feat, if > 1 feat in list
            // (i.e. user cannot un-train single feat selections)
            if (availableFeats.size() > 1
                    && tf.FeatName() != "")         // must already have a selection also
            {
                Feat feat;
                feat.Set_Name(" No Selection");     // space causes to appear at top of list
                feat.Set_Description("Clears the current feat selection");
                feat.Set_Icon("NoSelection");
                availableFeats.push_back(feat);
            }
            std::sort(availableFeats.begin(), availableFeats.end());

            // lock the window to stop flickering while updates are done
            m_featSelector.LockWindowUpdate();
            m_featSelector.SetImageList(NULL);
            m_featSelector.ResetContent();

            m_imagesFeatSelector.DeleteImageList();
            m_imagesFeatSelector.Create(
                    32,             // all icons are 32x32 pixels
                    32,
                    ILC_COLOR32,
                    0,
                    availableFeats.size());
            std::vector<Feat>::const_iterator it = availableFeats.begin();
            while (it != availableFeats.end())
            {
                (*it).AddImage(&m_imagesFeatSelector);
                ++it;
            }
            m_featSelector.SetImageList(&m_imagesFeatSelector);

            // now add the feat names to the combo control
            int sel = CB_ERR;
            it = availableFeats.begin();
            size_t featIndex = 0;
            while (it != availableFeats.end())
            {
                char buffer[_MAX_PATH];
                strcpy_s(buffer, (*it).Name().c_str());
                size_t pos = m_featSelector.AddString((*it).Name().c_str());
                m_featSelector.SetItemData(pos, featIndex);
                if (tf.FeatName() == (*it).Name())
                {
                    sel = pos;
                }
                ++featIndex;
                ++it;
            }
            m_bAlternateFeat = true;
            m_featSelector.SetCurSel(sel);
            m_featSelector.UnlockWindowUpdate();
            // position the drop list combo over the feat selection position
            CRect comboRect(ht.Rect());
            comboRect.bottom = comboRect.top + 960;   // 20 items visible in drop list
            m_featSelector.MoveWindow(comboRect);
            m_featSelector.SetCurSel(sel);
            m_featSelector.SetDroppedWidth(350); // wider to show extra text
            // set control visible to the user
            m_featSelector.ShowWindow(SW_SHOW);
            m_featSelector.SetFocus();
            m_featSelector.ShowDropDown();
        }
    }
}

void CFeatsClassControl::OnRButtonUp(UINT nFlags, CPoint point)
{
    // see which level item the user clicked on. if its a heroic level
    // line, then we allow them to move a class level up/down as appropriate
    GetCursorPos(&point);
    ScreenToClient(&point);
    HitCheckItem ht = HitCheck(point);
    if (ht.Level() >= 0
            && ht.Level() < MAX_CLASS_LEVELS
            && ht.Type() != HT_None)
    {
        // yes, we can display a pop up menu
        ClientToScreen(&point);
        CMenu menu;
        menu.LoadMenu(IDR_POPUP_MOVECLASS);
        CMenu *pMenu = menu.GetSubMenu(0);
        // enable / disable move up/down for multiple steps
        ClassType c1 = m_pCharacter->LevelData(ht.Level()).HasClass()
                ? m_pCharacter->LevelData(ht.Level()).Class()
                : Class_Unknown;
        for (size_t i = ID_MOVECLASS_UP1; i <= ID_MOVECLASS_UP19; ++i)
        {
            bool enable = false;
            int targetLevel = ht.Level() - (i - ID_MOVECLASS_UP1 + 1);
            if (targetLevel >= 0
                    && ht.Level() > (size_t)targetLevel)
            {
                // this is a possible move
                ClassType c2 = m_pCharacter->LevelData(targetLevel).HasClass()
                        ? m_pCharacter->LevelData(targetLevel).Class()
                        : Class_Unknown;
                enable = (c1 != c2);
            }
            if (enable)
            {
                pMenu->EnableMenuItem(i, MF_BYCOMMAND | MF_ENABLED);
            }
            else
            {
                pMenu->EnableMenuItem(i, MF_BYCOMMAND | MF_DISABLED);
            }
        }
        for (size_t i = ID_MOVECLASS_DOWN1; i <= ID_MOVECLASS_DOWN19; ++i)
        {
            bool enable = false;
            size_t targetLevel = ht.Level() + (i - ID_MOVECLASS_DOWN1 + 1);
            if (targetLevel < MAX_CLASS_LEVELS)
            {
                ClassType c2 = m_pCharacter->LevelData(targetLevel).HasClass()
                        ? m_pCharacter->LevelData(targetLevel).Class()
                        : Class_Unknown;
                enable = (c1 != c2);
            }
            if (enable)
            {
                pMenu->EnableMenuItem(i, MF_BYCOMMAND | MF_ENABLED);
            }
            else
            {
                pMenu->EnableMenuItem(i, MF_BYCOMMAND | MF_DISABLED);
            }
        }
        CWinAppEx * pApp = dynamic_cast<CWinAppEx*>(AfxGetApp());
        UINT sel = pApp->GetContextMenuManager()->TrackPopupMenu(
                pMenu->GetSafeHmenu(),
                point.x,
                point.y,
                this);
        if (sel != 0)
        {
            if (sel >= ID_MOVECLASS_UP1 && sel <= ID_MOVECLASS_UP19)
            {
                int targetLevel = ht.Level() - (sel - ID_MOVECLASS_UP1 + 1);
                SwapClasses(targetLevel, ht.Level());
            }
            else if (sel >= ID_MOVECLASS_DOWN1 && sel <= ID_MOVECLASS_DOWN19)
            {
                size_t targetLevel = ht.Level() + (sel - ID_MOVECLASS_DOWN1 + 1);
                SwapClasses(targetLevel, ht.Level());
            }
        }
    }
}

HitCheckItem CFeatsClassControl::HitCheck(CPoint mouse) const
{
    static HitCheckItem noHit(HT_None, CRect(0, 0, 0, 0), 0, 0);
    for (size_t i = 0; i < m_hitChecks.size(); ++i)
    {
        if (m_hitChecks[i].PointInRect(mouse))
        {
            return m_hitChecks[i];
        }
    }
    return noHit;
}

void CFeatsClassControl::DoClass1()
{
    if (m_pCharacter != NULL)
    {
        // build the menu of classes that can be selected
        CMenu menu;
        menu.CreatePopupMenu();
        ClassType type1 = m_pCharacter->Class1();
        ClassType type2 = m_pCharacter->Class2();
        ClassType type3 = m_pCharacter->Class3();
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            if (ci != Class_Epic && ci != Class_Legendary)
            {
                bool bAlignmentRestricted = !m_pCharacter->IsClassAvailable((ClassType)ci);
                bool bClassRestricted = m_pCharacter->IsClassRestricted((ClassType)ci);
                if (ci != type2
                        && ci != type3
                        || ci == Class_Unknown)
                {
                    // this class is selectable
                    CString text = EnumEntryText(
                            (ClassType)ci,
                            classTypeMap);
                    if (bAlignmentRestricted)
                    {
                        // add text to class name to show why it cannot be selected
                        text += " (Wrong Alignment)";
                        menu.AppendMenu(
                                MF_STRING | MF_DISABLED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else if (bClassRestricted
                            && !SameArchetype((ClassType)ci, type1)) // ensure ui allows switch of base types in same selection
                    {
                        // add text to class name to show why it cannot be selected
                        text += " (Archetype Restricted)";
                        menu.AppendMenu(
                                MF_STRING | MF_DISABLED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else if (ci == type1)
                    {
                        menu.AppendMenu(
                                MF_STRING | MF_CHECKED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else
                    {
                        menu.AppendMenu(
                                MF_STRING,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                }
            }
        }
        CPoint p(m_classRects[0].left,
                m_classRects[0].bottom);
        ClientToScreen(&p);
        CWinAppEx * pApp = dynamic_cast<CWinAppEx*>(AfxGetApp());
        UINT sel = pApp->GetContextMenuManager()->TrackPopupMenu(
                menu.GetSafeHmenu(),
                p.x,
                p.y,
                this);
        if (sel != 0)
        {
            sel -= ID_CLASS_UNKNOWN;
            ClassType ct = (ClassType)(sel);
            m_pCharacter->SetClass1(ct);
        }
    }
}

void CFeatsClassControl::DoClass2()
{
    if (m_pCharacter != NULL)
    {
        // build the menu of classes that can be selected
        CMenu menu;
        menu.CreatePopupMenu();
        ClassType type1 = m_pCharacter->Class1();
        ClassType type2 = m_pCharacter->Class2();
        ClassType type3 = m_pCharacter->Class3();
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            if (ci != Class_Epic && ci != Class_Legendary)
            {
                bool bAlignmentRestricted = !m_pCharacter->IsClassAvailable((ClassType)ci);
                bool bClassRestricted = m_pCharacter->IsClassRestricted((ClassType)ci);
                if (ci != type1
                        && ci != type3
                        || ci == Class_Unknown)
                {
                    // this class is selectable
                    CString text = EnumEntryText(
                            (ClassType)ci,
                            classTypeMap);
                    if (bAlignmentRestricted)
                    {
                        // add text to class name to show why it cannot be selected
                        text += " (Wrong Alignment)";
                        menu.AppendMenu(
                                MF_STRING | MF_DISABLED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else if (bClassRestricted
                            && !SameArchetype((ClassType)ci, type2)) // ensure ui allows switch of base types in same selection
                    {
                        // add text to class name to show why it cannot be selected
                        text += " (Archetype Restricted)";
                        menu.AppendMenu(
                                MF_STRING | MF_DISABLED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else if (ci == type1)
                    {
                        menu.AppendMenu(
                                MF_STRING | MF_CHECKED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else
                    {
                        menu.AppendMenu(
                                MF_STRING,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                }
            }
        }
        CPoint p(m_classRects[1].left,
                m_classRects[1].bottom);
        ClientToScreen(&p);
        CWinAppEx * pApp = dynamic_cast<CWinAppEx*>(AfxGetApp());
        UINT sel = pApp->GetContextMenuManager()->TrackPopupMenu(
                menu.GetSafeHmenu(),
                p.x,
                p.y,
                this);
        if (sel != 0)
        {
            sel -= ID_CLASS_UNKNOWN;
            ClassType ct = (ClassType)(sel);
            m_pCharacter->SetClass2(ct);
        }
    }
}

void CFeatsClassControl::DoClass3()
{
    if (m_pCharacter != NULL)
    {
        // build the menu of classes that can be selected
        CMenu menu;
        menu.CreatePopupMenu();
        ClassType type1 = m_pCharacter->Class1();
        ClassType type2 = m_pCharacter->Class2();
        ClassType type3 = m_pCharacter->Class3();
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            if (ci != Class_Epic && ci != Class_Legendary)
            {
                bool bAlignmentRestricted = !m_pCharacter->IsClassAvailable((ClassType)ci);
                bool bClassRestricted = m_pCharacter->IsClassRestricted((ClassType)ci);
                if (ci != type1
                        && ci != type2
                        || ci == Class_Unknown)
                {
                    // this class is selectable
                    CString text = EnumEntryText(
                            (ClassType)ci,
                            classTypeMap);
                    if (bAlignmentRestricted)
                    {
                        // add text to class name to show why it cannot be selected
                        text += " (Wrong Alignment)";
                        menu.AppendMenu(
                                MF_STRING | MF_DISABLED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else if (bClassRestricted
                            && !SameArchetype((ClassType)ci, type3)) // ensure ui allows switch of base types in same selection
                    {
                        // add text to class name to show why it cannot be selected
                        text += " (Archetype Restricted)";
                        menu.AppendMenu(
                                MF_STRING | MF_DISABLED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else if (ci == type1)
                    {
                        menu.AppendMenu(
                                MF_STRING | MF_CHECKED,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                    else
                    {
                        menu.AppendMenu(
                                MF_STRING,
                                ID_CLASS_UNKNOWN + ci,
                                text);
                    }
                }
            }
        }
        CPoint p(m_classRects[2].left,
                m_classRects[2].bottom);
        ClientToScreen(&p);
        CWinAppEx * pApp = dynamic_cast<CWinAppEx*>(AfxGetApp());
        UINT sel = pApp->GetContextMenuManager()->TrackPopupMenu(
                menu.GetSafeHmenu(),
                p.x,
                p.y,
                this);
        if (sel != 0)
        {
            sel -= ID_CLASS_UNKNOWN;
            ClassType ct = (ClassType)(sel);
            m_pCharacter->SetClass3(ct);
        }
    }
}

void CFeatsClassControl::UpdateClassChoiceChanged(Character * charData)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CFeatsClassControl::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CFeatsClassControl::UpdateFeatTrained(
        Character * charData,
        const std::string & featName)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CFeatsClassControl::UpdateFeatRevoked(
        Character * charData,
        const std::string & featName)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CFeatsClassControl::UpdateAbilityValueChanged(
        Character * charData,
        AbilityType ability)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CFeatsClassControl::UpdateAbilityTomeChanged(
        Character * charData,
        AbilityType ability)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CFeatsClassControl::UpdateRaceChanged(
        Character * charData,
        RaceType race)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

LRESULT CFeatsClassControl::UpdateControl(WPARAM, LPARAM)
{
    SetupControl();
    Invalidate();
    m_bUpdatePending = false;
    return 0;
}

void CFeatsClassControl::SetClassLevel(ClassType ct, size_t level)
{
    if (level < MAX_CLASS_LEVELS)
    {
        if (!m_pCharacter->LevelData(level).HasClass()
                || m_pCharacter->LevelData(level).Class() != ct)
        {
            m_pCharacter->SetClass(level, ct);
            // ensure class tooltip updates
            HideTip();
            m_tooltipItem = HitCheckItem(HT_None, CRect(0, 0, 0, 0), 0, 0);
        }
    }
    else
    {
        ::MessageBeep(MB_OK);
    }
}

size_t CFeatsClassControl::FeatImageIndex(const std::string & name) const
{
    size_t featImageIndex = 0;
    const std::list<Feat> & allFeats = StandardFeats();
    std::list<Feat>::const_iterator it = allFeats.begin();
    while (it != allFeats.end())
    {
        if ((*it).Name() == name)
        {
            // this is the one we want
            break;
        }
        ++it;
        ++featImageIndex;
    }
    return featImageIndex;
}

void CFeatsClassControl::OnFeatSelectOk()
{
    HideTip();
    // feat selection complete
    m_featSelector.ShowWindow(SW_HIDE); // hide control
    int sel = m_featSelector.GetCurSel();
    if (sel != CB_ERR)
    {
        // user has selected a feat, train it!
        std::vector<TrainableFeatTypes> tfts = m_availableFeats[m_featSelectItem.Level()];
        CString featName;
        m_featSelector.GetLBText(sel, featName);
        Feat feat = FindFeat((LPCTSTR)featName);
        if (m_bAlternateFeat)
        {
            // just train it
            m_pCharacter->TrainAlternateFeat(
                    (LPCTSTR)featName,
                    tfts[m_featSelectItem.Data()],
                    m_featSelectItem.Level());
        }
        else
        {
            if (m_pCharacter->ShowUnavailable())
            {
                // need to evaluate the feat properly
                m_pCharacter->ToggleShowUnavailable();
                if (m_pCharacter->IsFeatTrainable(m_featSelectItem.Level(), tfts[m_featSelectItem.Data()], feat, true))
                {
                    m_pCharacter->TrainFeat(
                            (LPCTSTR)featName,
                            tfts[m_featSelectItem.Data()],
                            m_featSelectItem.Level());
                }
                else
                {
                    // tried to select an untrainable feat
                    ::MessageBeep(MB_OK);
                }
                // restore
                m_pCharacter->ToggleShowUnavailable();
            }
            else
            {
                // just train it
                m_pCharacter->TrainFeat(
                        (LPCTSTR)featName,
                        tfts[m_featSelectItem.Data()],
                        m_featSelectItem.Level());
            }
        }
    }
}

void CFeatsClassControl::OnFeatSelectCancel()
{
    // feat selection aborted, just hide the control
    m_featSelector.ShowWindow(SW_HIDE);
    HideTip();
}

LRESULT CFeatsClassControl::OnHoverComboBox(WPARAM wParam, LPARAM lParam)
{
    // wParam = selected index
    // lParam = control ID
    if (m_showingTip)
    {
        m_tooltip.Hide();
        m_tooltip2.Hide();
    }
    if (wParam >= 0)
    {
        // we have a selection, get the feats name
        CString featName;
        m_featSelector.GetLBText(wParam, featName);
        if (!featName.IsEmpty())
        {
            CRect rctWindow;
            m_featSelector.GetWindowRect(&rctWindow);
            rctWindow.right = rctWindow.left + m_featSelector.GetDroppedWidth();
            std::vector<TrainableFeatTypes> tfts = m_availableFeats[m_featSelectItem.Level()];
            // tip is shown to the left or the right of the combo box
            const Feat & feat = FindFeat((LPCTSTR)featName);
            CPoint tipTopLeft(rctWindow.left, rctWindow.top);
            CPoint tipAlternate(rctWindow.right, rctWindow.top);
            bool warn = false;
            if (m_featSelectItem.Level() == 0
                    && tfts[m_featSelectItem.Data()] != TFT_Automatic
                    && tfts[m_featSelectItem.Data()] != TFT_GrantedFeat
                    && featName != " No Selection")
            {
                warn = !m_pCharacter->IsFeatTrainable(
                        m_featSelectItem.Level(),
                        tfts[m_featSelectItem.Data()],
                        feat,
                        false,
                        false)
                        || (featName == "Completionist" && (m_featSelectItem.Level() == 0));
            }
            m_tooltip.SetOrigin(tipTopLeft, tipAlternate, true);
            m_tooltip.SetFeatItem(*m_pCharacter, &feat, warn, m_featSelectItem.Level(), false);
            m_tooltip.Show();
            m_showingTip = true;
        }
    }
    return 0;
}

void CFeatsClassControl::HideTip()
{
    // tip not shown if not over a tip item
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_tooltip2.Hide();
        m_showingTip = false;
    }
}

void CFeatsClassControl::SwapClasses(size_t level1, size_t level2)
{
    m_pCharacter->SwapClasses(level1, level2);
}

LRESULT CFeatsClassControl::OnToggleFeatIgnore(WPARAM wParam, LPARAM lParam)
{
    // wParam = index of clicked item
    // lParam = (CString*)name of feat
    int selection = static_cast<int>(wParam);
    CString* pFeatName = static_cast<CString*>((void*)lParam);
    std::string featName = (LPCTSTR)(*pFeatName);
    if (!m_pCharacter->ShowIgnoredItems())
    {
        m_featSelector.DeleteString(selection);
    }
    if (m_pCharacter->IsInIgnoreList(featName))
    {
        m_pCharacter->RemoveFromIgnoreList(featName);
    }
    else
    {
        m_pCharacter->AddToIgnoreList(featName);
    }
    return 0;
}

