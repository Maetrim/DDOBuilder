// SkillSpendDialog.cpp
//

#include "stdafx.h"
#include "SkillSpendDialog.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"
#include <algorithm>

namespace
{
    COLORREF f_classSkillColour = RGB(152,251,152);             // pale green
    COLORREF f_classSkillColourHighlighted = RGB(52,151,52);    // darker green
    COLORREF f_crossClassSkillColour = RGB(255, 255, 255);      // white
    COLORREF f_crossClassSkillColourHighlighted = RGB(180, 180, 180);      // gray
    COLORREF f_untrainableSkillColour = RGB(0xFF, 0xB6, 0xC1);  // light pink
    COLORREF f_untrainableSkillColourHighlighted = RGB(222, 160, 171);  // dark pink
    COLORREF f_selectedColour = ::GetSysColor(COLOR_HIGHLIGHT);
    COLORREF f_skillOverspendColour = RGB(0xFF, 0x00, 0x00);    // RED
    COLORREF f_black = RGB(0, 0, 0);                            // black

    class WeightedSkill
    {
    public:
        WeightedSkill(SkillType skill) :
                m_skill(skill),
                m_weight(0)
            {
            }
        ~WeightedSkill() {};

        void AddWeighting(int amount) { if (m_weight < 100) m_weight += amount; };
        SkillType Skill() const { return m_skill; };
        int Weight() const { return m_weight; };
    private:
        SkillType m_skill;
        int m_weight;
    };

    bool SortSkillDescending(const WeightedSkill & a, const WeightedSkill & b)
    {
        // sort on skill weighting
        return (a.Weight() > b.Weight());
    }
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSkillSpendDialog, CDialog)
    //{{AFX_MSG_MAP(CSkillSpendDialog)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SKILL_LIST, OnCustomDrawSkillsList)
    ON_NOTIFY(NM_CLICK, IDC_SKILL_LIST, OnLeftClickSkillsList)
    ON_NOTIFY(NM_RCLICK, IDC_SKILL_LIST, OnRightClickSkillsList)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_SKILL_LIST, OnItemchangedSkillsList)
    ON_BN_CLICKED(IDC_BUTTON_MAX_SKILL, OnButtonMaxThisSkill)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_SKILL, OnButtonClearThisSkill)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALL_SKILLS, OnButtonClearAllSkills)
    ON_BN_CLICKED(IDC_BUTTON_AUTOSPEND, OnButtonAutoSpendSkillPoints)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_SKILL_LIST, OnColumnClickSkillsList)
    ON_NOTIFY(HDN_ITEMCLICK, IDC_SKILL_LIST, OnColumnClickSkillsList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CSkillSpendDialog::CSkillSpendDialog(Character * pCharacter) :
    CDialog(CSkillSpendDialog::IDD, NULL),
    m_pCharacter(pCharacter),
    m_hlightlightColumn(-1),
    m_hlightlightRow(-1),
    m_selectedColumn(-1)
{
    //{{AFX_DATA_INIT(CSkillSpendDialog)
    //}}AFX_DATA_INIT
    m_classIcons.Create(16, 15, ILC_COLOR24, 0, Class_Count);
    CBitmap images;
    images.LoadBitmap(IDR_MENUICONS_TOOLBAR);
    m_classIcons.Add(&images, (CBitmap*)NULL);
}

void CSkillSpendDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSkillSpendDialog)
    DDX_Control(pDX, IDC_SKILL_LIST, m_skillsList);
    DDX_Control(pDX, IDC_BUTTON_MAX_SKILL, m_buttonMaxThisSkill);
    DDX_Control(pDX, IDC_BUTTON_CLEAR_SKILL, m_buttonClearThisSkill);
    DDX_Control(pDX, IDC_BUTTON_CLEAR_ALL_SKILLS, m_buttonClearAllSkills);
    //}}AFX_DATA_MAP
}

BOOL CSkillSpendDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // add the columns to the list control with default sizes for all columns
    m_skillsList.InsertColumn( 0, "Skill", LVCFMT_LEFT, 100);
    m_skillsList.InsertColumn( 1, "1", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 2, "2", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 3, "3", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 4, "4", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 5, "5", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 6, "6", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 7, "7", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 8, "8", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn( 9, "9", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(10, "10", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(11, "11", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(12, "12", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(13, "13", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(14, "14", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(15, "15", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(16, "16", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(17, "17", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(18, "18", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(19, "19", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(20, "20", LVCFMT_CENTER, 30);
    m_skillsList.InsertColumn(21, "Total", LVCFMT_LEFT, 40);

    // ensure the header control for the list is the same as the main control
    CHeaderCtrl* pHeaderCtrl = m_skillsList.GetHeaderCtrl();
    pHeaderCtrl->SetDlgCtrlID(m_skillsList.GetDlgCtrlID());
    // we do some custom processing of header control messages
    m_skillsHeader.SubclassWindow(m_skillsList.GetHeaderCtrl()->GetSafeHwnd());

    m_skillsList.InsertItem(0, "Available @ Level", 0);     // item 0 shows the available skill points
    m_skillsList.InsertItem(1, "Class", 0);         // item 1 shows the class icons
    // class icons shown via custom draw
    // items 2 to n are the skills
    for (size_t skill = Skill_Unknown + 1; skill < Skill_Count; ++skill)
    {
        CString text;
        text = EnumEntryText((SkillType)skill, skillTypeMap);
        int index = m_skillsList.InsertItem(m_skillsList.GetItemCount(), text, 0);
    }
    // repeat the class icons again at the bottom
    m_skillsList.InsertItem(m_skillsList.GetItemCount(), "Class", 0);
    // and finally a spent row
    int index = m_skillsList.InsertItem(
            m_skillsList.GetItemCount(),
            "Still to Spend",
            0);
    // class icons shown via custom draw
    // double buffer the control to reduce flickering
    m_skillsList.SetExtendedStyle(
            m_skillsList.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

    PopulateItems();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSkillSpendDialog::PopulateItems()
{
    m_skillsList.LockWindowUpdate();
    int sel = m_skillsList.GetSelectionMark();
    int index = 0;
    // populate the available skill points at each level
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
        int available = levelData.SkillPointsAvailable();
        CString text;
        text.Format("%d", available);
        m_skillsList.SetItemText(index, 1 + level, text);
    }
    ++index;
    ++index;        // skip class icons line
    // items 2 to n are the skills
    for (size_t skill = Skill_Unknown + 1; skill < Skill_Count; ++skill)
    {
        CString text;
        // add the number of skill points spent on this skill at each heroic level
        for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
        {
            const LevelTraining & levelData = m_pCharacter->LevelData(level);
            const std::list<TrainedSkill> & ts = levelData.TrainedSkills();
            std::list<TrainedSkill>::const_iterator it = ts.begin();
            std::vector<size_t> skillRanks(Skill_Count, 0);
            while (it != ts.end())
            {
                skillRanks[(*it).Skill()]++;
                ++it;
            }
            // left blank if no skill points spent at this level for this skill
            if (skillRanks[skill] > 0)
            {
                ClassType ct = m_pCharacter->LevelData(level).HasClass()
                        ? m_pCharacter->LevelData(level).Class()
                        : Class_Unknown;
                if (IsClassSkill(ct, (SkillType)skill))
                {
                    text.Format("%d", skillRanks[skill]);
                }
                else
                {
                    text = "";
                    int fullRanks = (skillRanks[skill] / 2);
                    if (fullRanks > 0)
                    {
                        text.Format("%d", fullRanks);
                    }
                    // its a cross class skill, show in multiples of ½
                    if (skillRanks[skill] % 2 != 0)
                    {
                        text += "½";
                    }
                }
                m_skillsList.SetItemText(index, 1 + level, text);
            }
            else
            {
                // blank the item out if no trained ranks
                m_skillsList.SetItemText(index, 1 + level, "");
            }
        }
        // show the total trained ranks at end of heroic levels
        double total = m_pCharacter->SkillAtLevel((SkillType)skill, MAX_CLASS_LEVELS, false);
        text.Format("%.1f", total);
        m_skillsList.SetItemText(index, 1 + MAX_CLASS_LEVELS, text);
        ++index;
    }
    ++index;        // skip class icons line
    // and finally a spent row
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
        int available = levelData.SkillPointsAvailable();
        int spent = levelData.SkillPointsSpent();
        CString text;
        text.Format("%d", available - spent);
        m_skillsList.SetItemText(index, 1 + level, text);
    }
    if (sel >= 0)
    {
        m_skillsList.SetItemState(
                sel,
                LVIS_SELECTED | LVIS_FOCUSED,
                LVIS_SELECTED | LVIS_FOCUSED);
        m_skillsList.SetSelectionMark(sel);
    }
    m_skillsList.UnlockWindowUpdate();
}

void CSkillSpendDialog::OnCustomDrawSkillsList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW *pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

    *pResult = CDRF_DODEFAULT;          // assume normal windows drawn control
    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else
    {
        int lastItem = m_skillsList.GetItemCount() - 1;
        if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
        {
            // this is the pre-paint stage
            if (pLVCD->nmcd.dwItemSpec == 1
                    || pLVCD->nmcd.dwItemSpec >= UINT(lastItem - 1))
            {
                // this is the pre-paint stage for a class line or last item
                // we need to post paint option here to add class icons
                // and highlight lines
                *pResult = CDRF_NOTIFYPOSTPAINT;
            }
            else
            {
                // for pre-paint on other items we need to draw sub items
                *pResult = CDRF_NOTIFYSUBITEMDRAW;
            }
        }
        else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
        {
            pLVCD->clrTextBk = (pLVCD->iSubItem == m_selectedColumn) ? f_crossClassSkillColourHighlighted : f_crossClassSkillColour;
            pLVCD->clrText = f_black;
            if (pLVCD->iSubItem > 0 && pLVCD->iSubItem <= 20)
            {
                int level = pLVCD->iSubItem - 1;
                SkillType skill = (SkillType)(pLVCD->nmcd.dwItemSpec - 1);  // skill 0 is unknown
                // this is a level column
                if (pLVCD->nmcd.dwItemSpec >= 2
                        && pLVCD->nmcd.dwItemSpec < 2 + Skill_Count)
                {
                    double maxSkill = m_pCharacter->MaxSkillForLevel(
                            skill,
                            level);
                    double skillValue = m_pCharacter->SkillAtLevel(
                            skill,
                            level,
                            false);     // skill tome not included
                    // set the background colour to pale green for class skills
                    ClassType ct = m_pCharacter->LevelData(level).HasClass()
                            ? m_pCharacter->LevelData(level).Class()
                            : Class_Unknown;
                    if (IsClassSkill(ct, skill))
                    {
                        // show its a class skill
                        pLVCD->clrTextBk = (pLVCD->iSubItem == m_selectedColumn) ? f_classSkillColourHighlighted : f_classSkillColour;
                        pLVCD->clrText = f_black;
                    }
                    else
                    {
                        // white to cross class skill, red for cannot train skill at all
                        if (maxSkill > 0.0)
                        {
                            pLVCD->clrTextBk = (pLVCD->iSubItem == m_selectedColumn) ? f_crossClassSkillColourHighlighted : f_crossClassSkillColour;
                            pLVCD->clrText = f_black;
                        }
                        else
                        {
                            // this skill cannot be trained at all
                            pLVCD->clrTextBk = (pLVCD->iSubItem == m_selectedColumn) ? f_untrainableSkillColourHighlighted : f_untrainableSkillColour;
                            pLVCD->clrText = f_black;
                        }
                    }
                    if (skillValue > maxSkill)
                    {
                        // this skill is overspent at this point
                        pLVCD->clrText = f_skillOverspendColour;
                    }
                }
            }
            else
            {
                pLVCD->clrTextBk = f_crossClassSkillColour;
                pLVCD->clrText = f_black;
            }
        }
        else if (CDDS_ITEMPOSTPAINT == pLVCD->nmcd.dwDrawStage)
        {
            CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
            if (pLVCD->nmcd.dwItemSpec == 1
                    || pLVCD->nmcd.dwItemSpec == lastItem - 1)
            {
                // for post-paint we do all 20 classes in one go
                for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
                {
                    // find out where we are drawing the class icon
                    CRect rctItem;
                    m_skillsList.GetSubItemRect(
                            pLVCD->nmcd.dwItemSpec,
                            level + 1,
                            LVIR_BOUNDS,
                            rctItem);
                    CPoint p(
                            rctItem.left + (rctItem.Width() - 16) / 2,
                            rctItem.top + (rctItem.Height() - 16) / 2);
                    // get the class icon to draw for this level
                    int icon = m_pCharacter->LevelData(level).HasClass()
                            ? m_pCharacter->LevelData(level).Class()
                            : Class_Unknown;
                    m_classIcons.Draw(pDC, icon, p, ILD_NORMAL);
                }
            }
            else
            {
                // draw the selected items also
                for (int item = 0; item <= lastItem; ++item)
                {
                    bool selected = (m_skillsList.GetItemState(item, LVIS_SELECTED) != 0);
                    if (selected)
                    {
                        // only re-draw for selected items
                        CRect rctItem;
                        m_skillsList.GetSubItemRect(
                                item,
                                0,  // gets rectangle of whole item
                                LVIR_BOUNDS,
                                rctItem);
                        CRect rctItem2;
                        m_skillsList.GetSubItemRect(
                                item,
                                1,
                                LVIR_BOUNDS,
                                rctItem2);
                        rctItem.left += 2;
                        rctItem.right = rctItem2.left - 1;
                        CString text;
                        text = m_skillsList.GetItemText(item, 0);
                        CBrush b;
                        b.CreateSolidBrush(f_selectedColour);
                        pDC->FillRect(rctItem, &b);
                        // now re-draw the item text
                        COLORREF old = pDC->SetTextColor(RGB(255, 255, 255));
                        pDC->TextOut(rctItem.left + 2, rctItem.top, text);
                        pDC->SetTextColor(old);
                    }
                }
                // must be last line, draw the highlight if present
                if (m_hlightlightColumn >= 0)
                {
                    CRect rctTopItem;
                    CRect rctBottomItem;
                    m_skillsList.GetSubItemRect(
                            0,
                            m_hlightlightColumn,
                            LVIR_BOUNDS,
                            rctTopItem);
                    m_skillsList.GetSubItemRect(
                            m_skillsList.GetItemCount() - 1,
                            m_hlightlightColumn,
                            LVIR_BOUNDS,
                            rctBottomItem);
                    pDC->MoveTo(rctTopItem.left, rctTopItem.top);
                    pDC->LineTo(rctTopItem.left, rctBottomItem.bottom - 1);
                    pDC->MoveTo(rctTopItem.right - 2, rctTopItem.top);
                    pDC->LineTo(rctTopItem.right - 2, rctBottomItem.bottom - 1);
                }
                if (m_hlightlightRow >= 0)
                {
                    CRect rctLeftItem;
                    CRect rctRightItem;
                    m_skillsList.GetSubItemRect(
                            m_hlightlightRow,
                            0,
                            LVIR_BOUNDS,
                            rctLeftItem);
                    m_skillsList.GetSubItemRect(
                            m_hlightlightRow,
                            m_skillsList.GetHeaderCtrl()->GetItemCount() - 1,
                            LVIR_BOUNDS,
                            rctRightItem);
                    pDC->MoveTo(rctLeftItem.left + 2, rctLeftItem.top);
                    pDC->LineTo(rctRightItem.right, rctLeftItem.top);
                    pDC->MoveTo(rctLeftItem.left + 2, rctLeftItem.bottom - 1);
                    pDC->LineTo(rctRightItem.right, rctLeftItem.bottom - 1);
                }
            }
            *pResult = CDRF_SKIPDEFAULT;
        }
    }
}

void CSkillSpendDialog::OnLeftClickSkillsList(NMHDR*, LRESULT* pResult)
{
    CPoint pt = IdentifyItemUnderMouse(true);
    if (pt.x >= 1 && pt.x < 1 + MAX_CLASS_LEVELS)
    {
        // they have left clicked on a class level item
        if (pt.y >= 2 && pt.y < 1 + Skill_Count)
        {
            SkillType skill = (SkillType)(pt.y - 1);
            int level = pt.x - 1;
            // and its a valid skill
            // add a skill rank here if:
            // its trainable and we have skill points to spend
            double maxSkill = m_pCharacter->MaxSkillForLevel(
                    skill,
                    level);
            double currentSkill = m_pCharacter->SkillAtLevel(skill, level, false);
            if (maxSkill > 0.0
                    && currentSkill < maxSkill)
            {
                // must have skill points left to spend
                const LevelTraining & levelData = m_pCharacter->LevelData(level);
                int available = levelData.SkillPointsAvailable();
                int spent = levelData.SkillPointsSpent();
                if (available > spent)
                {
                    // yes, the user can train this skill
                    m_pCharacter->SpendSkillPoint(level, skill);
                    PopulateItems();        // update display
                }
            }
        }
    }
}

void CSkillSpendDialog::OnRightClickSkillsList(NMHDR*, LRESULT* pResult)
{
    CPoint pt = IdentifyItemUnderMouse(true);
    if (pt.x >= 1 && pt.x < 1 + MAX_CLASS_LEVELS)
    {
        // they have right clicked on a class level item
        if (pt.y >= 2 && pt.y < 1 + Skill_Count)
        {
            SkillType skill = (SkillType)(pt.y - 1);
            int level = pt.x - 1;
            // revoke a rank if they have trained ranks at this level
            const LevelTraining & levelData = m_pCharacter->LevelData(level);
            const std::list<TrainedSkill> & trainedSkills = levelData.TrainedSkills();
            std::list<TrainedSkill>::const_iterator it = trainedSkills.begin();
            bool hasTrainedRanks = false;
            while (it != trainedSkills.end())
            {
                if ((*it).Skill() == skill)
                {
                    // has had a spend on this skill at this level
                    hasTrainedRanks = true;
                }
                ++it;
            }
            if (hasTrainedRanks)
            {
                // yes, the user can revoke this skill
                m_pCharacter->RevokeSkillPoint(level, skill, false);
                PopulateItems();        // update display
            }
        }
    }
}

CPoint CSkillSpendDialog::IdentifyItemUnderMouse(bool wasClick)
{
    CPoint mouse;
    GetCursorPos(&mouse);
    m_skillsList.ScreenToClient(&mouse);    // convert to control coordinates
    LVHITTESTINFO hitTestInfo;
    hitTestInfo.pt = mouse; 
    int item = m_skillsList.SubItemHitTest(&hitTestInfo);
    if (item >= 0)
    {
        if (wasClick)
        {
            // and select the row clicked on
            m_skillsList.SetItemState(
                    item,
                    LVIS_SELECTED | LVIS_FOCUSED,
                    LVIS_SELECTED | LVIS_FOCUSED);
            m_skillsList.SetSelectionMark(item);
        }
        return CPoint(hitTestInfo.iSubItem, hitTestInfo.iItem);
    }
    else
    {
        // not on the control
        return CPoint(-1, -1);
    }
}

void CSkillSpendDialog::OnItemchangedSkillsList(NMHDR* pNMHDR, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // ignore invalid notifications
    if ((pNMListView->uChanged & LVIF_STATE) 
            && (pNMListView->uNewState & LVIS_SELECTED))
    {
        // enable the Max/Clear this skill button as a skill is selected
        int sel = pNMListView->iItem;
        if (sel >= 2 && sel < 1 + Skill_Count)
        {
            m_buttonMaxThisSkill.EnableWindow(true);
            m_buttonClearThisSkill.EnableWindow(true);
        }
        else
        {
            m_buttonMaxThisSkill.EnableWindow(false);
            m_buttonClearThisSkill.EnableWindow(false);
        }
        // redraw as selection change can cause draw artifacts on screen
        m_skillsList.Invalidate(FALSE);
    }
}

void CSkillSpendDialog::OnButtonMaxThisSkill()
{
    // try and get max ranks at level 20 for the selected skill
    // by spending skill points at each level if we can
    // first, what skill is selected?
    int sel = m_skillsList.GetSelectionMark();
    SkillType skill = (SkillType)(sel - 1);
    MaxThisSkill(skill, false);
}

void CSkillSpendDialog::MaxThisSkill(SkillType skill, bool suppressUpdate)
{
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        bool spent = true;
        while (spent)
        {
            // try and spend skill points on this skill at this level
            double maxSkill = m_pCharacter->MaxSkillForLevel(
                    skill,
                    level);
            double currentSkill = m_pCharacter->SkillAtLevel(skill, level, false);
            // also ensure we do not overspend a skill across all levels
            double maxSkill20 = m_pCharacter->MaxSkillForLevel(
                    skill,
                    MAX_CLASS_LEVELS-1);
            double currentSkill20 = m_pCharacter->SkillAtLevel(skill, MAX_CLASS_LEVELS-1, false);
            if (maxSkill > 0.0
                    && currentSkill < maxSkill
                    && currentSkill20 < maxSkill20)
            {
                // special case for last class level and a non-class skill
                // which we do not want to train to 11.5 ranks
                if (MAX_CLASS_LEVELS % 2 == 0
                        && level == (MAX_CLASS_LEVELS - 1)
                        && maxSkill == (MAX_CLASS_LEVELS + 3) / 2.0
                        && currentSkill == (MAX_CLASS_LEVELS + 2) / 2.0)
                {
                    // we don't spend for this specific case
                    spent = false;
                }
                else
                {
                    // must have skill points left to spend
                    const LevelTraining & levelData = m_pCharacter->LevelData(level);
                    int available = levelData.SkillPointsAvailable();
                    int spentAtLevel = levelData.SkillPointsSpent();
                    if (available > spentAtLevel)
                    {
                        // yes, the user can train this skill
                        m_pCharacter->SpendSkillPoint(level, skill, suppressUpdate);
                    }
                    else
                    {
                        spent = false;
                    }
                }
            }
            else
            {
                spent = false;
            }
        }
    }
    if (!suppressUpdate)
    {
        PopulateItems();        // update display
    }
}

void CSkillSpendDialog::OnButtonClearThisSkill()
{
    // clear any trained ranks for this skill at all class levels
    int sel = m_skillsList.GetSelectionMark();
    SkillType skill = (SkillType)(sel - 1);
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
        bool rankRevoked = true;
        while (rankRevoked)
        {
            rankRevoked = false;
            // revoke a rank if they have trained ranks at this level
            const std::list<TrainedSkill> & trainedSkills = levelData.TrainedSkills();
            std::list<TrainedSkill>::const_iterator it = trainedSkills.begin();
            while (it != trainedSkills.end())
            {
                if ((*it).Skill() == skill)
                {
                    // has had a spend on this skill at this level
                    rankRevoked = true;
                }
                ++it;
            }
            if (rankRevoked)
            {
                // yes, the user can revoke this skill
                m_pCharacter->RevokeSkillPoint(level, skill, true);
            }
        }
    }
    m_pCharacter->SkillsUpdated();
    PopulateItems();        // update display
}

void CSkillSpendDialog::OnButtonClearAllSkills()
{
    // clear any trained ranks for all skills at all class levels
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
        bool rankRevoked = true;
        while (rankRevoked)
        {
            rankRevoked = false;
            // revoke all trained ranks for all skills at this level
            std::list<TrainedSkill> trainedSkills = levelData.TrainedSkills();
            std::list<TrainedSkill>::const_iterator it = trainedSkills.begin();
            while (it != trainedSkills.end())
            {
                // don't re-evaluate feats etc on each skill point revoke
                m_pCharacter->RevokeSkillPoint(level, (*it).Skill(), true);
                ++it;
            }
        }
    }
    m_pCharacter->SkillsUpdated();
    PopulateItems();        // update display
}

BOOL CSkillSpendDialog::PreTranslateMessage(MSG* pMsg) 
{
    // monitor any mouse moves over the dialog so we can determine
    // the highlight position to be shown in the skills list control
    if (pMsg->message == WM_MOUSEMOVE)
    {
        // process the mouse location and change highlight if required
        CPoint pt = IdentifyItemUnderMouse(false);
        if (pt.y < 2 || pt.y > Skill_Count)
        {
            // no highlight for first and last two rows
            pt.y = -1;
        }
        if (pt.x == 0 || pt.x == 21)
        {
            // no highlight for first and last columns
            pt.x = -1;
        }
        if (pt.x == -1 || pt.y == -1)
        {
            // if any point is outside, then no highlight
            pt.x = -1;
            pt.y = -1;
        }
        // only redraw on a change
        if (pt.x != m_hlightlightColumn
                || pt.y != m_hlightlightRow)
        {
            m_hlightlightColumn = pt.x;
            m_hlightlightRow = pt.y;
            m_skillsList.Invalidate(FALSE);
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CSkillSpendDialog::OnButtonAutoSpendSkillPoints()
{
    CWaitCursor wait;
    // to auto spend skill points we first have to rank all the skills
    // into a spend order. This can be tricky if the character is multi-classed
    // first we will create a weighting scheme for all the skills based on the
    // class levels trained.
    std::vector<WeightedSkill> skills;
    for (size_t skill = Skill_Unknown; skill < Skill_Count; ++skill)
    {
        skills.push_back(WeightedSkill((SkillType)skill));
    }
    std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_CLASS_LEVELS);
    for (size_t ct = Class_Unknown; ct < Class_Count; ++ct)
    {
        if (classLevels[ct] > 0)
        {
            // we have levels trained in this class
            switch (ct)
            {
                case Class_Alchemist:
                    skills[Skill_Balance].AddWeighting(100);
                    skills[Skill_Concentration].AddWeighting(100);
                    skills[Skill_Heal].AddWeighting(100);
                    skills[Skill_SpellCraft].AddWeighting(100);
                    skills[Skill_Spot].AddWeighting(100);
                    skills[Skill_UMD].AddWeighting(100);
                    break;
                case Class_Artificer:
                    skills[Skill_DisableDevice].AddWeighting(100);
                    skills[Skill_Search].AddWeighting(100);
                    skills[Skill_OpenLock].AddWeighting(100);
                    skills[Skill_Spot].AddWeighting(100);
                    skills[Skill_UMD].AddWeighting(100);
                    break;
                case Class_Rogue:
                    skills[Skill_DisableDevice].AddWeighting(100);
                    skills[Skill_Search].AddWeighting(100);
                    skills[Skill_OpenLock].AddWeighting(100);
                    skills[Skill_Spot].AddWeighting(100);
                    skills[Skill_UMD].AddWeighting(100);
                    skills[Skill_Hide].AddWeighting(100);
                    skills[Skill_MoveSilently].AddWeighting(100);
                    break;
                case Class_Barbarian:
                    skills[Skill_Intimidate].AddWeighting(100);
                    skills[Skill_Balance].AddWeighting(100);
                    break;
                case Class_Bard:
                case Class_BardStormsinger:
                    skills[Skill_Perform].AddWeighting(100);
                    skills[Skill_UMD].AddWeighting(100);
                    break;
                case Class_Cleric:
                case Class_ClericDarkApostate:
                    skills[Skill_Concentration].AddWeighting(100);
                    skills[Skill_Heal].AddWeighting(100);
                    skills[Skill_SpellCraft].AddWeighting(100);
                    break;
                case Class_Druid:
                    skills[Skill_Concentration].AddWeighting(100);
                    skills[Skill_SpellCraft].AddWeighting(100);
                    break;
                case Class_FavoredSoul:
                    skills[Skill_Concentration].AddWeighting(100);
                    skills[Skill_Heal].AddWeighting(100);
                    skills[Skill_SpellCraft].AddWeighting(100);
                    break;
                case Class_Fighter:
                    skills[Skill_Intimidate].AddWeighting(100);
                    skills[Skill_Balance].AddWeighting(100);
                    break;
                case Class_Monk:
                    skills[Skill_Concentration].AddWeighting(100);
                    break;
                case Class_Paladin:
                case Class_PaladinSacredFist:
                    skills[Skill_Intimidate].AddWeighting(100);
                    break;
                case Class_Ranger:
                    skills[Skill_Spot].AddWeighting(100);
                    skills[Skill_Search].AddWeighting(100);
                    break;
                case Class_Sorcerer:
                    skills[Skill_Concentration].AddWeighting(100);
                    skills[Skill_SpellCraft].AddWeighting(100);
                    break;
                case Class_Warlock:
                    skills[Skill_Concentration].AddWeighting(100);
                    skills[Skill_SpellCraft].AddWeighting(100);
                    break;
                case Class_Wizard:
                    skills[Skill_Concentration].AddWeighting(100);
                    skills[Skill_SpellCraft].AddWeighting(100);
                    break;
            }
        }
    }
    if (m_pCharacter->Race() == Race_Warforged
            || m_pCharacter->Race() == Race_BladeForged)
    {
        // Warforged should also train repair if possible
        skills[Skill_Repair].AddWeighting(50);
    }
    // now add the basic weights for all skills. This is the order I would
    // take them if I had spare skill points
    skills[Skill_UMD].AddWeighting(21);
    skills[Skill_DisableDevice].AddWeighting(20);
    skills[Skill_OpenLock].AddWeighting(19);
    skills[Skill_Spot].AddWeighting(18);
    skills[Skill_Search].AddWeighting(17);
    skills[Skill_Balance].AddWeighting(16);
    skills[Skill_Swim].AddWeighting(15);
    skills[Skill_Bluff].AddWeighting(14);
    skills[Skill_Diplomacy].AddWeighting(13);
    skills[Skill_Haggle].AddWeighting(12);
    skills[Skill_Concentration].AddWeighting(11);
    skills[Skill_Heal].AddWeighting(10);
    skills[Skill_Hide].AddWeighting(9);
    skills[Skill_Intimidate].AddWeighting(8);
    skills[Skill_MoveSilently].AddWeighting(7);
    skills[Skill_Perform].AddWeighting(6);
    skills[Skill_Jump].AddWeighting(5);
    skills[Skill_SpellCraft].AddWeighting(4);
    skills[Skill_Repair].AddWeighting(3);
    skills[Skill_Tumble].AddWeighting(2);
    skills[Skill_Listen].AddWeighting(1);
    // now sort into decreasing order and then spend the skill points
    std::sort(skills.begin(), skills.end(), SortSkillDescending);
    // now we have the skills sorted, try and spend on each in order
    for (size_t i = 0; i < skills.size(); ++i)
    {
        MaxThisSkill(skills[i].Skill(), true);
    }
    PopulateItems();        // update display
    m_pCharacter->SkillsUpdated();
}

void CSkillSpendDialog::OnColumnClickSkillsList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMHEADER* pHeader = (NMHEADER*)pNMHDR;
    if (m_selectedColumn == pHeader->iItem)
    {
        // clear the selection
        m_selectedColumn = -1;
    }
    else
    {
        m_selectedColumn = pHeader->iItem;
        // don;t allow non level columns to be selected
        if (m_selectedColumn == 0 || m_selectedColumn > 20)
        {
            m_selectedColumn = -1;
        }
    }
    m_skillsList.Invalidate(FALSE);
}
