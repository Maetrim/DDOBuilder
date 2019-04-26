// FeatsClassControl.cpp : implementation file
//
#include "stdafx.h"
#include "FeatsClassControl.h"
#include "GlobalSupportFunctions.h"

// CFeatsClassControl
const int c_FeatColumnWidth = 150;      // pixles

IMPLEMENT_DYNAMIC(CFeatsClassControl, CWnd)

CFeatsClassControl::CFeatsClassControl() :
    m_pCharacter(NULL),
    m_bMenuDisplayed(false),
    m_maxRequiredFeats(0),
    m_numClassColumns(0)
{
    m_classImagesSmall.Create(16, 15, ILC_COLOR24, 0, Class_Count);
    CBitmap images;
    images.LoadBitmap(IDR_MENUICONS_TOOLBAR);
    m_classImagesSmall.Add(&images, (CBitmap*)NULL);
    m_classImagesLarge.Create(
            32,             // all icons are 32x32 pixels
            32,
            ILC_COLOR32,
            0,
            Class_Count);
    for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
    {
        CImage image;
        HRESULT result = LoadImageFile(
                IT_ui,
                (LPCTSTR)EnumEntryText((ClassType)ci, classTypeMap),
                &image);
        if (result == S_OK)
        {
            CBitmap bitmap;
            bitmap.Attach(image.Detach());
            m_classImagesLarge.Add(&bitmap, c_transparentColour); // standard mask color (purple)
        }
    }
    // create the image list for the feats
    const std::list<Feat> & allFeats = AllFeats();
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
END_MESSAGE_MAP()
#pragma warning(pop)

// CFeatsClassControl message handlers
void CFeatsClassControl::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
}

void CFeatsClassControl::SetupControl()
{
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
        for (size_t level = 0; level < MAX_LEVEL; ++level)
        {
            std::vector<TrainableFeatTypes> availableFeatSlots =
                    m_pCharacter->TrainableFeatTypeAtLevel(level);
            m_availableFeats.push_back(availableFeatSlots);
            m_maxRequiredFeats = max(m_maxRequiredFeats, availableFeatSlots.size());
        }
    }
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
        for (size_t level = 0; level < MAX_LEVEL; ++level)
        {
            CRect rctItem = m_levelRect;
            bool bFeatsAvailable = (m_availableFeats[level].size() > 0);
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
            memoryDc.Draw3dRect(
                    rctItem,
                    ::GetSysColor(COLOR_BTNHIGHLIGHT),
                    ::GetSysColor(COLOR_BTNSHADOW));
            CString text;
            text.Format("%d", level + 1);
            CSize csText = memoryDc.GetTextExtent(text);
            memoryDc.TextOut(
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
                memoryDc.Draw3dRect(
                        rctItem,
                        ::GetSysColor(COLOR_BTNHIGHLIGHT),
                        ::GetSysColor(COLOR_BTNSHADOW));
                ClassType ct = m_pCharacter->Class(cc);
                // only drawn if it matches current class selection
                if (m_pCharacter->Class(cc) == classSelection
                        || (classSelection == Class_Epic && cc == 0))
                {
                    if (bFeatsAvailable)
                    {
                        // draw the class using the large icon
                        CPoint pos = rctItem.TopLeft();
                        pos.x += (rctItem.Width() - 32) / 2;    // centre it
                        pos.y += (rctItem.Height() - 32) / 2;    // centre it
                        m_classImagesLarge.Draw(
                                &memoryDc,
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
                                &memoryDc,
                                classSelection,
                                pos,
                                ILD_TRANSPARENT);
                    }
                }
            }
            // draw the feat selections (if any)
            for (size_t fc = 0; fc < m_maxRequiredFeats; ++fc)
            {
                rctItem.left = m_featRects[fc].left;
                rctItem.right = m_featRects[fc].right;
                memoryDc.Draw3dRect(
                        rctItem,
                        ::GetSysColor(COLOR_BTNHIGHLIGHT),
                        ::GetSysColor(COLOR_BTNSHADOW));
            }
            // additional columns for stats [str][dex][con][int][wis][cha][bab]
            for (size_t stats = Ability_Unknown; stats < Ability_Count; ++stats)
            {
                rctItem.left = m_statRects[stats].left;
                rctItem.right = m_statRects[stats].right;
                memoryDc.Draw3dRect(
                        rctItem,
                        ::GetSysColor(COLOR_BTNHIGHLIGHT),
                        ::GetSysColor(COLOR_BTNSHADOW));
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
                CSize csText = memoryDc.GetTextExtent(strText);
                memoryDc.TextOut(
                        rctItem.left + (rctItem.Width() - csText.cx) / 2,
                        rctItem.top + (rctItem.Height() - csText.cy) / 2,
                        strText);
            }
            // set top of next item
            top = rctItem.bottom + 1;
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
    //pDC->Draw3dRect(
    //        rctItem,
    //        ::GetSysColor(COLOR_BTNHIGHLIGHT),
    //        ::GetSysColor(COLOR_BTNSHADOW));
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
        HitChecks classDropArrow((HitType)(HT_Class1 + cc), rctDropArrow);
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

LRESULT CFeatsClassControl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // if the menu is being displayed, leave the item hilighted in the control
    // gets re-drawn when menu is dismissed/closed
    if (!m_bMenuDisplayed)
    {
        Invalidate();
    }
    return 0;
}
