// InfoTip.cpp
//

#include "stdafx.h"
#include "InfoTip.h"

#include "Character.h"
#include "EnhancementTreeItem.h"
#include "Feat.h"
#include "GlobalSupportFunctions.h"
#include "LevelTraining.h"

namespace
{
    const int c_controlSpacing = 3;
}

CInfoTip::CInfoTip() :
    m_origin(CPoint(0, 0)),
    m_bRightAlign(false),
    m_bAlternate(false)
{
    // create the fonts used
    LOGFONT lf;
    ZeroMemory((PVOID)&lf, sizeof(LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0));
    lf = nm.lfMenuFont;
    m_standardFont.CreateFontIndirect(&lf);
    lf.lfWeight = FW_BOLD;
    m_boldFont.CreateFontIndirect(&lf);
}

CInfoTip::~CInfoTip()
{
}

BEGIN_MESSAGE_MAP(CInfoTip, CWnd)
    //{{AFX_MSG_MAP(CInfoTip)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CInfoTip::Create(CWnd* pParentWnd, bool bAlternate) 
{
    m_bAlternate = bAlternate;
    // Must have a parent
    ASSERT(pParentWnd != NULL);

    BOOL bSuccess = CreateEx(
            NULL, 
            AfxRegisterWndClass(0), 
            NULL, 
            WS_POPUP, 
            0, 
            0, 
            0, 
            0, 
            pParentWnd->GetSafeHwnd(), 
            NULL, 
            NULL);

    return bSuccess;
}

void CInfoTip::Show()
{
    CDC * pDC = GetDC();
    CSize windowSize;
    GetWindowSize(pDC, &windowSize);
    ReleaseDC(pDC);

    if (!m_bRightAlign)
    {
        // before actually showing the tip, determine whether it fits on the screen
        // if it extends below the bottom or past the right we move the window
        // accordingly to make sure it is visible. Do this for the monitor the
        // origin point for this tooltip is on.
        HMONITOR hMonitor = MonitorFromPoint(m_origin, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo;
        memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);
        CRect monitorSize(monitorInfo.rcWork);
        if (m_origin.x + windowSize.cx > monitorSize.right)
        {
            // move the tip left to ensure all text visible
            m_origin.x = monitorSize.right - windowSize.cx;
        }
        // alternate position is above
        if (m_origin.y + windowSize.cy > monitorSize.bottom)
        {
            // move the tip above the origin position to ensure content visible
            m_origin.y = m_alternate.y - windowSize.cy; // alternate position
        }
    }
    else
    {
        // if it extends beyond the left we use the alternate position
        // accordingly to make sure it is visible. 
        // we move the tool tip up as required
        HMONITOR hMonitor = MonitorFromPoint(m_origin, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo;
        memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);
        CRect monitorSize(monitorInfo.rcWork);
        if (m_origin.x - windowSize.cx < monitorSize.left)
        {
            // need to use the alternate position
            m_origin = m_alternate;
        }
        else
        {
            m_origin.x -= (windowSize.cx + 2);
        }
        // alternate position is above
        if (m_origin.y + windowSize.cy > monitorSize.bottom)
        {
            // move the tip up
            m_origin.y = monitorSize.bottom - windowSize.cy;
        }
    }

    SetWindowPos(
            &wndTop,
            m_origin.x,
            m_origin.y,
            windowSize.cx,
            windowSize.cy,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void CInfoTip::SetOrigin(CPoint origin, CPoint alternate, bool rightAlign)
{
    m_origin = origin;
    m_alternate = alternate;
    m_bRightAlign = rightAlign;
}

void CInfoTip::Hide()
{ 
    ShowWindow(SW_HIDE);
}

void CInfoTip::OnPaint() 
{
    CPaintDC dc( this ); // device context for painting

    CRect rc;
    CBrush tooltipColourBrush;
    CBrush frameBrush;

    // Get the client rectangle
    GetClientRect(rc);

    // Create the brushes
    frameBrush.CreateSolidBrush(::GetSysColor(COLOR_INFOTEXT));
    tooltipColourBrush.CreateSolidBrush(::GetSysColor(COLOR_INFOBK));

    // Draw the frame
    dc.FillRect(rc, &tooltipColourBrush);
    dc.DrawEdge(rc, BDR_SUNKENOUTER, BF_RECT);
    dc.SetBkMode(TRANSPARENT);

    dc.SaveDC();
    dc.SelectObject(m_boldFont);
    // draw the icon
    m_image.TransparentBlt(
            dc.GetSafeHdc(),
            CRect(c_controlSpacing, c_controlSpacing, c_controlSpacing + 32, c_controlSpacing + 32),
            CRect(0, 0, 32, 32));
    // render the title in bold
    CRect rcTitle(c_controlSpacing + 32 + c_controlSpacing, c_controlSpacing, rc.right, rc.bottom);
    dc.DrawText(m_title, rcTitle, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    CSize sizeCost = dc.GetTextExtent(m_cost);
    CSize sizeRanks = dc.GetTextExtent(m_ranks);
    dc.TextOut(rc.right - c_controlSpacing - sizeCost.cx, c_controlSpacing, m_cost);
    COLORREF old = dc.SetTextColor(RGB(0,128, 0));
    dc.TextOut(rc.right - c_controlSpacing - sizeRanks.cx, c_controlSpacing + sizeCost.cy, m_ranks);
    dc.SetTextColor(old);

    dc.SelectObject(m_standardFont);
    // optional requirements list shown above description, 1 per line
    int top = c_controlSpacing + max(sizeRanks.cy * 2, 32) + c_controlSpacing;
    ASSERT(m_requirements.size() == m_bRequirementMet.size());
    for (size_t ri = 0; ri < m_requirements.size(); ++ri)
    {
        // text drawn in green or red depending on whether the requirement is met or not
        dc.SetTextColor(m_bRequirementMet[ri] ? RGB(0, 128, 0) : RGB(255, 0, 0));
        CRect rctRequirement(c_controlSpacing, top, rc.right, rc.bottom);
        dc.TextOut(c_controlSpacing, top, m_requirements[ri]);
        top += dc.GetTextExtent(m_requirements[ri]).cy;
    }
    dc.SetTextColor(RGB(0, 0, 0));    // ensure black for the rest
    // optional Effect Descriptions list shown above description, these can word wrap
    for (size_t ed = 0; ed < m_effectDescriptions.size(); ++ed)
    {
        CRect rctEd;
        // measure
        dc.DrawText(m_effectDescriptions[ed], &rctEd, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
        rctEd += CPoint(2, top);
        dc.DrawText(m_effectDescriptions[ed], &rctEd, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
        CString text;
        if (m_effectDescriptions[ed].Find(':') >= 0)
        {
            text = m_effectDescriptions[ed].Left(m_effectDescriptions[ed].Find(':'));   // get the effect name to draw in bold
        }
        else
        {
            // draw all text in bold
            text = m_effectDescriptions[ed];
        }
        rctEd += CPoint(1, 0);
        dc.DrawText(text, &rctEd, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
        top += rctEd.Height();
    }
    CRect rcDescription(c_controlSpacing, top, rc.right, rc.bottom);
    dc.DrawText(m_description, &rcDescription, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    // Clean up GDI
    dc.RestoreDC(-1);
}

BOOL CInfoTip::GetWindowSize(CDC* pDC, CSize * size)
{
    ASSERT(pDC != NULL);
    // an enhancement item looks like this:
    // +---------------------------------------------------+
    // | +----+ Enhancement name                    Cost x |
    // | |icon|                                    Ranks n |
    // | +----+                                            |
    // | [requirements list if any]                        |
    // | [EffectDescription1 (if any)                     ]|
    // | [EffectDescription2                              ]|
    // | [EffectDescription..n                            ]|
    // | +------------------------------------------------+|
    // | |Description                                     ||
    // | +------------------------------------------------+|
    // +---------------------------------------------------+
    CRect rcWnd(0, 0, 0, 0);
    // border space first
    rcWnd.InflateRect(c_controlSpacing, c_controlSpacing, c_controlSpacing, c_controlSpacing);
    pDC->SaveDC();
    pDC->SelectObject(m_boldFont);
    // Calculate the area for the tip text
    CRect rcTitle;
    pDC->DrawText(m_title, &rcTitle, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    rcWnd.bottom += max(rcTitle.Height() * 2, 32) + c_controlSpacing;      // 2 lines or icon height

    pDC->SelectObject(m_standardFont);
    CRect rcRanks;
    pDC->DrawText(m_ranks, &rcRanks, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    size_t topWidth = 32 + c_controlSpacing + rcTitle.Width() + c_controlSpacing + rcRanks.Width();

    // optional requirements list shown above description, 1 per line
    ASSERT(m_requirements.size() == m_bRequirementMet.size());
    for (size_t ri = 0; ri < m_requirements.size(); ++ri)
    {
        CRect rctRequirement;
        pDC->DrawText(m_requirements[ri], &rctRequirement, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
        topWidth = max(topWidth, (size_t)rctRequirement.Width());
        rcWnd.InflateRect(
                0,
                0,
                0,
                rctRequirement.Height());
    }
    // optional Effect Descriptions list shown above description, these can word wrap
    for (size_t ed = 0; ed < m_effectDescriptions.size(); ++ed)
    {
        CRect rctEd;
        pDC->DrawText(m_effectDescriptions[ed], &rctEd, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
        topWidth = max(topWidth, (size_t)rctEd.Width());
        rcWnd.InflateRect(
                0,
                0,
                0,
                rctEd.Height());
    }
    CRect rcDescription;
    pDC->DrawText(m_description, &rcDescription, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);

    // now work out the total window size
    size_t bottomWidth = rcDescription.Width();
    rcWnd.InflateRect(
            0,
            0,
            max(topWidth, bottomWidth),
            rcDescription.Height());
    // Set the window size
    if (size != NULL)
    {
        size->cx = rcWnd.Width();
        size->cy = rcWnd.Height();
    }
    pDC->RestoreDC(-1);

    return TRUE;
}

void CInfoTip::SetEnhancementTreeItem(
        const Character & charData,
        const EnhancementTreeItem * pItem,
        const std::string & selection,
        size_t spentInTree)
{
    m_image.Destroy();
    LoadImageFile(IT_enhancement, pItem->Icon(), &m_image);
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return or possible \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_requirements.clear();
    m_effectDescriptions.clear();
    m_bRequirementMet.clear();
    // add the required spent in tree to requirements list
    CString text;
    text.Format("Requires: %d AP spent in tree", pItem->MinSpent());
    m_requirements.push_back(text);
    m_bRequirementMet.push_back(spentInTree >= pItem->MinSpent());
    pItem->CreateRequirementStrings(
            charData,
            &m_requirements,
            &m_bRequirementMet,
            MAX_LEVEL);
    m_cost.Format("  Cost %d", pItem->Cost(selection));
    m_ranks.Format("  Ranks %d", pItem->Ranks());
}

void CInfoTip::SetEnhancementSelectionItem(
        const Character & charData,
        const EnhancementTreeItem * pItem,
        const EnhancementSelection * pSelection,
        size_t ranks)
{
    m_image.Destroy();
    LoadImageFile(IT_enhancement, pSelection->Icon(), &m_image);
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pSelection->Name().c_str();
    m_description = pSelection->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_effectDescriptions.clear();
    m_requirements.clear();
    m_bRequirementMet.clear();
    CString text;
    text.Format("Requires: %d AP spent in tree", pItem->MinSpent());
    m_requirements.push_back(text);
    m_bRequirementMet.push_back(true);
    pSelection->CreateRequirementStrings(
            charData,
            &m_requirements,
            &m_bRequirementMet,
            MAX_LEVEL);
    m_cost.Format("  Cost %d", pSelection->Cost());
    m_ranks.Format("  Ranks %d", ranks);
}

void CInfoTip::SetFeatItem(
        const Character & charData,
        const Feat * pItem,
        bool featSwapWarning,
        size_t level,
        bool grantedFeat)
{
    m_image.Destroy();
    if (S_OK != LoadImageFile(IT_feat, pItem->Icon(), &m_image, false))
    {
        LoadImageFile(IT_ui, pItem->Icon(), &m_image);
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_effectDescriptions.clear();
    m_requirements.clear();
    m_bRequirementMet.clear();
    if (!grantedFeat)
    {
        pItem->CreateRequirementStrings(charData, &m_requirements, &m_bRequirementMet, level);
        m_cost = "";
        if (pItem->MaxTimesAcquire() != 1)
        {
            m_ranks.Format("  Max Acquire %d", pItem->MaxTimesAcquire());
        }
        else
        {
            m_ranks = "";
        }
    }
    else
    {
        m_requirements.push_back("This is a granted feat");
        m_bRequirementMet.push_back(true);
    }
    if (featSwapWarning)
    {
        m_requirements.push_back("Requires a feat swap with Fred");
        m_bRequirementMet.push_back(false);
    }
    if (m_bAlternate)
    {
        m_requirements.push_back("This is an alternate feat selection.");
        m_bRequirementMet.push_back(false);
    }
    if (IsInIgnoreList(pItem->Name()))
    {
        m_requirements.push_back("This Feat is in your ignore list (Right click to restore it).");
        m_bRequirementMet.push_back(false);
    }
}

void CInfoTip::SetStanceItem(
        const Character & charData,
        const Stance * pItem,
        size_t numStacks)
{
    m_image.Destroy();
    if (S_OK != LoadImageFile(IT_enhancement, pItem->Icon(), &m_image, false))
    {
        // see if its a feat icon we need to use
        if (S_OK != LoadImageFile(IT_feat, pItem->Icon(), &m_image, false))
        {
            // finally check if its a UI (racial) icon we need to use
            if (S_OK != LoadImageFile(IT_ui, pItem->Icon(), &m_image, false))
            {
                LoadImageFile(IT_augment, pItem->Icon(), &m_image);
            }
        }
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_effectDescriptions.clear();
    m_requirements.clear();
    m_bRequirementMet.clear();
    if (pItem->HasSetBonus())
    {
        m_ranks.Format("  Stacks: %d", numStacks);
        // convert the descriptions to requirements to show which have been met
        // by the number of stacks
        std::istringstream ss((LPCTSTR)m_description);
        std::string s;
        size_t lastCount = 0;
        while (std::getline(ss, s, '\n'))
        {
            m_requirements.push_back(s.c_str());
            size_t count = static_cast<size_t>(atoi(m_requirements.back()));
            if (count > 0)
            {
                lastCount = count;
            }
            m_bRequirementMet.push_back(lastCount == 0 || lastCount <= numStacks);
        }
        m_description = "";
    }
    else
    {
        m_ranks = "";
        // list the stances which cannot be active if this one is
        std::list<std::string> incompatibleStances = pItem->IncompatibleStance();
        std::list<std::string>::const_iterator it = incompatibleStances.begin();
        while (it != incompatibleStances.end())
        {
            CString name;
            name = "Incompatible with: ";
            name += (*it).c_str();
            m_requirements.push_back(name);
            ++it;
        }
        m_bRequirementMet.resize(m_requirements.size(), true);
    }
    m_cost = "";
}

void CInfoTip::SetItem(
        const Item * pItem)
{
    m_image.Destroy();
    if (S_OK != LoadImageFile(IT_item, pItem->Icon(), &m_image, false))
    {
        // see if its a feat icon we need to use
        LoadImageFile(IT_feat, pItem->Icon(), &m_image);
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    // now do the same for any effect descriptions
    m_effectDescriptions.clear();
    const std::list<std::string> & eds = pItem->EffectDescription();
    std::list<std::string>::const_iterator it = eds.begin();
    while (it != eds.end())
    {
        CString processedDescription = (*it).c_str();
        GenerateLineBreaks(&processedDescription);
        m_effectDescriptions.push_back(processedDescription);
        ++it;
    }
    // add any set bonuses
    const std::list<std::string> & sets = pItem->SetBonus();
    std::list<std::string>::const_iterator sit = sets.begin();
    while (sit != sets.end())
    {
        const SetBonus & set = FindSetBonus((*sit));
        m_effectDescriptions.push_back(set.Description().c_str());
        ++sit;
    }

    // add the augments to the effect description list
    std::vector<ItemAugment> augments = pItem->Augments();
    for (size_t i = 0; i < augments.size(); ++i)
    {
        CString augmentText;
        augmentText += augments[i].Type().c_str();
        augmentText += ": ";
        if (augments[i].HasSelectedAugment())
        {
            // may be a configurable augment
            if (augments[i].HasValue())
            {
                CString text;
                text.Format("%+.0f ", augments[i].Value());
                augmentText += text;
            }
            augmentText += augments[i].SelectedAugment().c_str();
            m_effectDescriptions.push_back(augmentText);
            const Augment & aug = FindAugmentByName(augments[i].SelectedAugment());
            // also include any of the effect descriptions it may have
            const std::list<std::string> & eds = aug.EffectDescription();
            std::list<std::string>::const_iterator it = eds.begin();
            while (it != eds.end())
            {
                CString processedDescription = (*it).c_str();
                GenerateLineBreaks(&processedDescription);
                m_effectDescriptions.push_back(processedDescription);
                ++it;
            }
        }
        else
        {
            augmentText += "Empty augment slot";
            m_effectDescriptions.push_back(augmentText);
        }
    }
    m_requirements.clear();
    m_bRequirementMet.clear();
    if (pItem->HasDropLocation())
    {
        CString location;
        location.Format("Drop Location: %s", pItem->DropLocation().c_str());
        m_requirements.push_back(location);
        m_bRequirementMet.push_back(false);     // in red as not met to stand out
    }
    if (pItem->HasMinorArtifact())
    {
        m_requirements.push_back("---MINOR ARTIFACT---");
        m_bRequirementMet.push_back(false);     // in red as important
    }
    // optional slot restrictions
    if (pItem->HasRestrictedSlots())
    {
        for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
        {
            if (pItem->RestrictedSlots().HasSlot((InventorySlotType)i))
            {
                CString text;
                text.Format("Restricts items in the slot: %s", EnumEntryText((InventorySlotType)i, InventorySlotTypeMap));
                m_requirements.push_back(text);
                m_bRequirementMet.push_back(false);     // in red as important
            }
        }
    }
    // show the slots this item equips to
    CString slots = "Equips to slots: ";
    slots += pItem->ItemType().c_str();
    m_requirements.push_back(slots);
    m_bRequirementMet.push_back(true);     // in red as important
    m_cost = "";
}

void CInfoTip::SetAugment(
        const Augment * pAugment)
{
    m_effectDescriptions.clear();
    m_image.Destroy();
    if (S_OK != LoadImageFile(IT_augment, pAugment->HasIcon() ? pAugment->Icon() : "", &m_image, false))
    {
        // see if its a feat icon we need to use
        LoadImageFile(IT_augment, pAugment->HasIcon() ? pAugment->Icon() : "", &m_image);
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pAugment->Name().c_str();
    m_description = pAugment->Description().c_str();
    m_ranks.Format("  Min Level: %d", pAugment->MinLevel());
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    // also include any of the effect descriptions it may have
    const std::list<std::string> & eds = pAugment->EffectDescription();
    std::list<std::string>::const_iterator it = eds.begin();
    while (it != eds.end())
    {
        CString processedDescription = (*it).c_str();
        GenerateLineBreaks(&processedDescription);
        m_effectDescriptions.push_back(processedDescription);
        ++it;
    }
    m_requirements.clear();
    m_bRequirementMet.clear();
    m_cost = "";
}

void CInfoTip::SetLevelItem(
        const Character & charData,
        size_t level,
        const LevelTraining * levelData,
        ClassType expectedClass)
{
    // icon is the class level
    m_image.Destroy();
    CString icon = EnumEntryText(
            levelData->HasClass() ? levelData->Class() : Class_Unknown,
            classTypeMap);
    LoadImageFile(IT_ui, (LPCTSTR)icon, &m_image, true);
    std::vector<size_t> classLevels = charData.ClassLevels(level);
    m_image.SetTransparentColor(c_transparentColour);
    m_title.Format("Level %d: %s (%d)",
            level + 1,
            icon,
            levelData->HasClass() ? classLevels[levelData->Class()] : 0);
    m_description = "";
    // The description field is the list of feats that can/have been trained
    // along with the list of skills and skill points available
    std::vector<TrainableFeatTypes> trainable = charData.TrainableFeatTypeAtLevel(level);
    if (trainable.size() > 0)
    {
        for (size_t tft = 0; tft < trainable.size(); ++tft)
        {
            CString label = TrainableFeatTypeLabel(trainable[tft]);
            label += ": ";
            TrainedFeat tf = charData.GetTrainedFeat(
                    level,
                    trainable[tft]);
            if (tf.FeatName().empty())
            {
                label += "Empty Feat Slot\n";
            }
            else
            {
                label += tf.FeatName().c_str();
                label += "\n";
            }
            m_description += label;
        }
    }
    else
    {
        m_description += "No trainable feats at this level\n";
    }
    // now show how many skill points we have at this level unless its an epic level
    if (levelData->HasClass() && levelData->Class() != Class_Epic)
    {
        // blank line between feats and skills
        m_description += "\n";

        CString text;
        text.Format("Skill Points Available %d of %d",
                levelData->SkillPointsAvailable() - levelData->SkillPointsSpent(),
                levelData->SkillPointsAvailable());
        m_description += text;
        // show what skills (if any) points have been spent on
        // and whether they have been overspent on
        const std::list<TrainedSkill> & ts = levelData->TrainedSkills();
        std::list<TrainedSkill>::const_iterator it = ts.begin();
        std::vector<size_t> skillRanks(Skill_Count, 0);
        while (it != ts.end())
        {
            skillRanks[(*it).Skill()]++;
            ++it;
        }
        for (size_t i = 0; i < Skill_Count; ++i)
        {
            if (skillRanks[i] != 0)
            {
                // this skill has been trained, list it
                bool classSkill = IsClassSkill(levelData->Class(), (SkillType)i);
                if (classSkill)
                {
                    text.Format("\n%s: %d Ranks",
                            EnumEntryText((SkillType)i, skillTypeMap),
                            skillRanks[i]);
                }
                else
                {
                    text.Format("\n%s: %.1f Ranks (%d spent)",
                            EnumEntryText((SkillType)i, skillTypeMap),
                            skillRanks[i] * 0.5,
                            skillRanks[i]);
                }
                // has it been over trained?
                if (charData.SkillAtLevel((SkillType)i, level, false)
                        > charData.MaxSkillForLevel((SkillType)i, level))
                {
                    text  += " SKILL OVER TRAINED";
                }
                m_description += text;
            }
        }
    }
    m_requirements.clear();
    m_bRequirementMet.clear();
    m_cost = "";
    // warn if iconic race does not have expected class at level 1
    if (levelData->HasClass()
            && expectedClass != levelData->Class())
    {
        CString text("Requires a +1 Heart of Wood to switch from Iconic class of ");
        text += EnumEntryText(expectedClass, classTypeMap);
        m_requirements.push_back(text);
        m_bRequirementMet.push_back(false);
    }
}

void CInfoTip::SetSelfBuff(const std::string & name)
{
    OptionalBuff buff = FindOptionalBuff(name);
    m_effectDescriptions.clear();
    m_image.Destroy();
    if (S_OK != LoadImageFile(IT_spell, buff.HasIcon() ? buff.Icon() : "", &m_image, false))
    {
        // see if its a enhancement icon we need to use
        if (S_OK != LoadImageFile(IT_enhancement, buff.HasIcon() ? buff.Icon() : "", &m_image, false))
        {
             LoadImageFile(IT_feat, buff.HasIcon() ? buff.Icon() : "", &m_image);
        }
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_title = buff.Name().c_str();
    m_description = buff.Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_requirements.clear();
    m_bRequirementMet.clear();
    m_cost = "";
}

void CInfoTip::SetDCItem(
        const Character & charData,
        const DC * pDC)
{
    m_effectDescriptions.clear();
    m_image.Destroy();
    // load its icon
    if (S_OK != LoadImageFile(IT_enhancement, pDC->Icon(), &m_image, false))
    {
        // see if its a feat icon we need to use
        if (S_OK != LoadImageFile(IT_feat, pDC->Icon(), &m_image, false))
        {
            if (S_OK != LoadImageFile(IT_item, pDC->Icon(), &m_image, false))
            {
                LoadImageFile(IT_ui, pDC->Icon(), &m_image);
            }
        }
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pDC->Name().c_str();
    m_description = pDC->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_requirements.clear();
    m_bRequirementMet.clear();
    m_requirements.push_back(pDC->DCBreakdown(&charData).c_str());
    m_bRequirementMet.push_back(true);
    if (pDC->HasOther())
    {
        m_cost.Format(
                "DC: %s + %d",
                pDC->Other().c_str(),
                pDC->CalculateDC(&charData));
    }
    else
    {
        m_cost.Format("DC: %d", pDC->CalculateDC(&charData));
    }
}

void CInfoTip::GenerateLineBreaks(CString * text)
{
    // this can work in two ways:
    // 1: If the text already contains "\n"'s use those
    // 2: If not, then break the text up automatically to around 80 characters
    //    in length maximum per line.
    size_t count = text->Replace("\\n", "\n");
    if (count == 0)
    {
        // may have embedded "\n"'s
        count = text->Find('\n', 0);
        if (count > (size_t)text->GetLength())
        {
            // looks like we have to do it manually
            size_t length = text->GetLength();
            size_t pos = 75;        // assume 7 characters in
            while (pos < length)
            {
                // look for the first space character from pos onwards
                if (text->GetAt(pos) == ' ')
                {
                    // change the space to a "\n"
                    text->SetAt(pos, '\n');
                    pos += 75;          // move a standard line length from there
                }
                else
                {
                    ++pos;
                }
            }
        }
    }
}

