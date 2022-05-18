// SkillsView.cpp
//
#include "stdafx.h"
#include "SkillsView.h"
#include "GlobalSupportFunctions.h"
#include <algorithm>

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    const UINT UWM_UPDATE = ::RegisterWindowMessage(_T("UpdateView"));

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

IMPLEMENT_DYNCREATE(CSkillsView, CFormView)

CSkillsView::CSkillsView() :
    CFormView(CSkillsView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_hlightlightColumn(-1),
    m_hlightlightRow(-1),
    m_bUpdatePending(false),
    m_selectedColumn(-1)
{
    //{{AFX_DATA_INIT(CSkillsView)
    //}}AFX_DATA_INIT
    m_classIcons.Create(16, 15, ILC_COLOR24, 0, Class_Count);
    CBitmap images;
    images.LoadBitmap(IDR_MENUICONS_TOOLBAR);
    m_classIcons.Add(&images, (CBitmap*)NULL);
}

CSkillsView::~CSkillsView()
{
}

void CSkillsView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSkillsView)
    DDX_Control(pDX, IDC_SKILL_LIST, m_skillsList);
    DDX_Control(pDX, IDC_BUTTON_MAX_SKILL, m_buttonMaxThisSkill);
    DDX_Control(pDX, IDC_BUTTON_CLEAR_SKILL, m_buttonClearThisSkill);
    DDX_Control(pDX, IDC_BUTTON_CLEAR_ALL_SKILLS, m_buttonClearAllSkills);
    DDX_Control(pDX, IDC_BUTTON_AUTOSPEND, m_buttonAutoBuySkills);
    //}}AFX_DATA_MAP
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSkillsView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_REGISTERED_MESSAGE(UWM_UPDATE, OnUpdateView)
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
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CSkillsView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSkillsView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CSkillsView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    // add the columns to the list control with default sizes for all columns
    m_skillsList.InsertColumn( 0, "Skill", LVCFMT_LEFT, 92);
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
    m_skillsList.InsertColumn(22, "Tome", LVCFMT_CENTER, 40);

    // ensure the header control for the list is the same as the main control
    CHeaderCtrl* pHeaderCtrl = m_skillsList.GetHeaderCtrl();
    pHeaderCtrl->SetDlgCtrlID(m_skillsList.GetDlgCtrlID());
    // we do some custom processing of header control messages
    m_skillsHeader.SubclassWindow(m_skillsList.GetHeaderCtrl()->GetSafeHwnd());

    m_skillsList.InsertItem(0, "Skill Points", 0);     // item 0 shows the available skill points
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
            "Not Spent",
            0);
    // class icons shown via custom draw
    // double buffer the control to reduce flickering
    m_skillsList.SetExtendedStyle(
            m_skillsList.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

    PopulateItems();
}

void CSkillsView::OnSize(UINT nType, int cx, int cy)
{
    if (IsWindow(m_skillsList.GetSafeHwnd()))
    {
        CRect rctSkills;
        m_skillsList.GetWindowRect(rctSkills);
        rctSkills -= rctSkills.TopLeft();
        CRect rctButton[4];
        m_buttonMaxThisSkill.GetWindowRect(rctButton[0]);
        m_buttonClearThisSkill.GetWindowRect(rctButton[1]);
        m_buttonClearAllSkills.GetWindowRect(rctButton[2]);
        m_buttonAutoBuySkills.GetWindowRect(rctButton[3]);
        rctButton[0] -= rctButton[0].TopLeft();
        rctButton[0] += CPoint(0, rctSkills.bottom + c_controlSpacing);
        rctButton[1] -= rctButton[1].TopLeft();
        rctButton[1] += CPoint(rctButton[0].right + c_controlSpacing, rctSkills.bottom + c_controlSpacing);
        rctButton[2] -= rctButton[2].TopLeft();
        rctButton[2] += CPoint(rctButton[1].right + c_controlSpacing, rctSkills.bottom + c_controlSpacing);
        rctButton[3] -= rctButton[3].TopLeft();
        rctButton[3] += CPoint(rctButton[2].right + c_controlSpacing, rctSkills.bottom + c_controlSpacing);
        m_skillsList.MoveWindow(rctSkills);
        m_buttonMaxThisSkill.MoveWindow(rctButton[0]);
        m_buttonClearThisSkill.MoveWindow(rctButton[1]);
        m_buttonClearAllSkills.MoveWindow(rctButton[2]);
        m_buttonAutoBuySkills.MoveWindow(rctButton[3]);
        SetScrollSizes(MM_TEXT, CSize(rctSkills.right, rctButton[0].bottom));
    }
    CFormView::OnSize(nType, cx, cy);
}

LRESULT CSkillsView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }

    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
    EnableControls();
    PopulateItems();
    return 0L;
}

BOOL CSkillsView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_SKILL_LIST,
        IDC_BUTTON_MAX_SKILL,
        IDC_BUTTON_CLEAR_SKILL,
        IDC_BUTTON_CLEAR_ALL_SKILLS,
        IDC_BUTTON_AUTOSPEND,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CSkillsView::PopulateItems()
{
    m_skillsList.LockWindowUpdate();
    int sel = m_skillsList.GetSelectionMark();
    int index = 0;
    if (m_pCharacter != NULL)
    {
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
            // also show the tome for this skill
            size_t tome = m_pCharacter->SkillTomeValue((SkillType)skill);
            text.Format("%+d", tome);
            m_skillsList.SetItemText(index, 2 + MAX_CLASS_LEVELS, text);
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
    }
    else
    {
        // set all items to blank
        size_t columnCount = m_skillsList.GetHeaderCtrl()->GetItemCount();
        for (size_t skill = 0; skill < Skill_Count; ++skill)
        {
            for (size_t column = 1; column <columnCount; ++column)
            {
                m_skillsList.SetItemText(skill + 2, column, "");
            }
        }
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

void CSkillsView::OnCustomDrawSkillsList(NMHDR* pNMHDR, LRESULT* pResult)
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
                    if (m_pCharacter != NULL)
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
                    int icon = m_pCharacter != NULL
                            ? m_pCharacter->LevelData(level).HasClass()
                                    ? m_pCharacter->LevelData(level).Class()
                                    : Class_Unknown
                            : Class_Unknown;
                    m_classIcons.Draw(pDC, icon, p, ILD_NORMAL);
                }
                // and also draw the tome icon
                CRect rctItem;
                m_skillsList.GetSubItemRect(
                        pLVCD->nmcd.dwItemSpec,
                        MAX_CLASS_LEVELS + 2,
                        LVIR_BOUNDS,
                        rctItem);
                CPoint p(
                        rctItem.left + (rctItem.Width() - 16) / 2,
                        rctItem.top + (rctItem.Height() - 16) / 2);
                m_classIcons.Draw(pDC, Class_Count, p, ILD_NORMAL);
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
                if (m_pCharacter != NULL)
                {
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
            }
            *pResult = CDRF_SKIPDEFAULT;
        }
    }
}

void CSkillsView::OnLeftClickSkillsList(NMHDR*, LRESULT* pResult)
{
    if (m_pCharacter != NULL)
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
        else
        {
            if (pt.x == MAX_CLASS_LEVELS + 2)
            {
                // they have clicked on a tome item for a specific skill
                if (pt.y >= 2 && pt.y < 2 + Skill_Count)
                {
                    SkillType skill = (SkillType)(pt.y - 1);
                    // if possible increase the skill tome for this skill
                    size_t tome = m_pCharacter->SkillTomeValue(skill);
                    if (tome < MAX_SKILL_TOME)
                    {
                        ++tome;
                        m_pCharacter->SetSkillTome(skill, tome);
                    }
                }
            }
        }
    }
}

void CSkillsView::OnRightClickSkillsList(NMHDR*, LRESULT* pResult)
{
    if (m_pCharacter != NULL)
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
        else
        {
            if (pt.x == MAX_CLASS_LEVELS + 2)
            {
                // they have clicked on a tome item for a specific skill
                if (pt.y >= 2 && pt.y < 2 + Skill_Count)
                {
                    SkillType skill = (SkillType)(pt.y - 1);
                    // if possible decrease the skill tome for this skill
                    size_t tome = m_pCharacter->SkillTomeValue(skill);
                    if (tome > 0)
                    {
                        --tome;
                        m_pCharacter->SetSkillTome(skill, tome);
                    }
                }
            }
        }
    }
}

CPoint CSkillsView::IdentifyItemUnderMouse(bool wasClick)
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

void CSkillsView::OnItemchangedSkillsList(NMHDR* pNMHDR, LRESULT* pResult)
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

void CSkillsView::OnButtonMaxThisSkill()
{
    // try and get max ranks at level 20 for the selected skill
    // by spending skill points at each level if we can
    // first, what skill is selected?
    int sel = m_skillsList.GetSelectionMark();
    SkillType skill = (SkillType)(sel - 1);
    MaxThisSkill(skill, false);
}

void CSkillsView::MaxThisSkill(SkillType skill, bool suppressUpdate)
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

void CSkillsView::OnButtonClearThisSkill()
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

void CSkillsView::OnButtonClearAllSkills()
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

BOOL CSkillsView::PreTranslateMessage(MSG* pMsg) 
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
    return CFormView::PreTranslateMessage(pMsg);
}

void CSkillsView::OnButtonAutoSpendSkillPoints()
{
    if (m_pCharacter != NULL)
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
                        skills[Skill_Perform].AddWeighting(100);
                        skills[Skill_UMD].AddWeighting(100);
                        break;
                    case Class_Cleric:
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
}

void CSkillsView::EnableControls()
{
    bool enabled = (m_pCharacter != NULL);
    m_buttonMaxThisSkill.EnableWindow(enabled);
    m_buttonClearThisSkill.EnableWindow(enabled);
    m_buttonClearAllSkills.EnableWindow(enabled);
    m_buttonAutoBuySkills.EnableWindow(enabled);
}

void CSkillsView::UpdateSkillSpendChanged(
        Character * charData,
        size_t level,
        SkillType skill)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CSkillsView::UpdateSkillTomeChanged(
        Character * charData,
        SkillType skill)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CSkillsView::UpdateClassChoiceChanged(Character * charData)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CSkillsView::UpdateClassChanged(
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

void CSkillsView::UpdateAbilityValueChanged(
        Character * charData,
        AbilityType ability)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CSkillsView::UpdateAbilityTomeChanged(
        Character * charData,
        AbilityType ability)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

void CSkillsView::UpdateRaceChanged(Character * charData, RaceType race)
{
    if (!m_bUpdatePending)
    {
        PostMessage(UWM_UPDATE, 0, 0L);
        m_bUpdatePending = true;
    }
}

LRESULT CSkillsView::OnUpdateView(WPARAM wParam, LPARAM lParam)
{
    PopulateItems();
    m_bUpdatePending = false;
    return 0L;
}

void CSkillsView::OnColumnClickSkillsList(NMHDR* pNMHDR, LRESULT* pResult)
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

