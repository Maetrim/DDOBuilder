// FeatsClassControl.cpp : implementation file
//
#include "stdafx.h"
#include "FeatsClassControl.h"
#include "GlobalSupportFunctions.h"

// CFeatsClassControl
namespace
{
    const int c_FeatColumnWidth = 200;      // pixels
    const UINT UWM_UPDATE = ::RegisterWindowMessage(_T("Update"));
    const int c_dudLevel = MAX_LEVEL + 1;
}

IMPLEMENT_DYNAMIC(CFeatsClassControl, CWnd)

CFeatsClassControl::CFeatsClassControl() :
    m_pCharacter(NULL),
    m_bMenuDisplayed(false),
    m_maxRequiredFeats(0),
    m_numClassColumns(0),
    m_bUpdatePending(false),
    m_highlightedLevelLine(c_dudLevel)   // starts invalid
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
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_REGISTERED_MESSAGE(UWM_UPDATE, UpdateControl)
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
        HitChecks classDropArrow((HitType)(HT_Class1 + cc), rctDropArrow, 0);
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
                || (classSelection == Class_Epic && cc == 0))
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
        HitChecks classSelect((HitType)(HT_LevelClass1 + cc), rctItem, level);
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
        // if we have an actual trained feat, draw it
        TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                level,
                tfts[featIndex]);
        // name is blank if no feat currently trained
        if (tf.FeatName() != "")
        {
            // draw the selected feat
            // first draw its icon
            size_t imageIndex = FeatImageIndex(tf.FeatName());
            CPoint pos = rctItem.TopLeft();
            pos.x += 2;
            pos.y += 2;
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
        else
        {
            // draw a feat that needs to be selected
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
    return 0;
}


void CFeatsClassControl::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which row the mouse is over, and change highlight if required
    size_t overLevel = c_dudLevel;
    for (size_t level = 0; level < MAX_LEVEL; ++level)
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
    // track the mouse so we know when it leaves our window
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);

    __super::OnMouseMove(nFlags, point);
}


void CFeatsClassControl::OnLButtonUp(UINT nFlags, CPoint point)
{
    // so whether the user has clicked on a specific item
    GetCursorPos(&point);
    ScreenToClient(&point);
    size_t data;
    HitType ht = HitCheck(point, &data);
    switch (ht)
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
        // set he class for this level if its not already this class
        SetClassLevel(m_pCharacter->Class(0), data);
        break;
    case HT_LevelClass2:
        // set he class for this level if its not already this class
        SetClassLevel(m_pCharacter->Class(1), data);
        break;
    case HT_LevelClass3:
        // set he class for this level if its not already this class
        SetClassLevel(m_pCharacter->Class(2), data);
        break;
    }
}

HitType CFeatsClassControl::HitCheck(CPoint mouse, size_t * data) const
{
    HitType type = HT_None;
    *data = 0;
    for (size_t i = 0; i < m_hitChecks.size(); ++i)
    {
        if (m_hitChecks[i].PointInRect(mouse))
        {
            type = m_hitChecks[i].Type();
            *data = m_hitChecks[i].Data();
            break;
        }
    }
    return type;
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
            if (m_pCharacter->IsClassAvailable((ClassType)ci)
                    && ci != type2
                    && ci != type3
                    || ci == Class_Unknown)
            {
                // this class is selectable
                CString text = EnumEntryText(
                        (ClassType)ci,
                        classTypeMap);
                if (ci == type1)
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
            m_pCharacter->SetClass1(0, ct); // level does not matter
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
            if (m_pCharacter->IsClassAvailable((ClassType)ci)
                    && ci != type1
                    && ci != type3
                    || ci == Class_Unknown)
            {
                // this class is selectable
                CString text = EnumEntryText(
                        (ClassType)ci,
                        classTypeMap);
                if (ci == type1)
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
            m_pCharacter->SetClass2(0, ct); // level does not matter
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
            if (m_pCharacter->IsClassAvailable((ClassType)ci)
                    && ci != type1
                    && ci != type2
                    || ci == Class_Unknown)
            {
                // this class is selectable
                CString text = EnumEntryText(
                        (ClassType)ci,
                        classTypeMap);
                if (ci == type1)
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
            m_pCharacter->SetClass3(0, ct); // level does not matter
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

LRESULT CFeatsClassControl::UpdateControl(WPARAM, LPARAM)
{
    SetupControl();
    Invalidate();
    m_bUpdatePending = false;
    return 0;
}

void CFeatsClassControl::SetClassLevel(ClassType ct, size_t level)
{
    if (level < MAX_CLASS_LEVEL)
    {
        if (!m_pCharacter->LevelData(level).HasClass()
                || m_pCharacter->LevelData(level).Class() != ct)
        {
            m_pCharacter->SetClass(level, ct);
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
    const std::list<Feat> & allFeats = AllFeats();
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

