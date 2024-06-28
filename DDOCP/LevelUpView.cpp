// LevelUpView.cpp
//
#include "stdafx.h"
#include "LevelUpView.h"

#include "DDOCP.h"
#include "FeatSelectionDialog.h"
#include "GlobalSupportFunctions.h"
#include "SkillSpendDialog.h"
#include <algorithm>
#include "MouseHook.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    const UINT UWM_UPDATE_COMPLETE = ::RegisterWindowMessage(_T("UpdateComplete"));
    const UINT UWM_TOGGLE_FEAT = ::RegisterWindowMessage(_T("ToggleIgnoreFeat"));

    enum SkillListColumns
    {
        SLC_name = 0,
        SLC_Spend,
        SLC_Tome,
        SLC_Ability,
        SLC_Max,
        SLC_CrossClassAtLevel,
        SLC_Total,

        SLC_Count
    };
    COLORREF f_skillOverspendColour = RGB(0xE9, 0x96, 0x7A); // dark salmon
    const int c_levelButtonSizeX = 44;
    const int c_levelButtonSizeY = 54;
}

IMPLEMENT_DYNCREATE(CLevelUpView, CFormView)

CLevelUpView::CLevelUpView() :
    CFormView(CLevelUpView::IDD),
    m_pCharacter(NULL),
    m_skillBeingEdited(Skill_Unknown),
    m_level(0),
    m_canPostMessage(true),
    m_showingTip(false),
    m_tipCreated(false),
    m_selectedSkill(-1),
    m_automaticHandle(0),
    m_hoverItem(-1),
    m_bIgnoreNextMessage(false)
{
    m_comboFeatSelect[0].SetCanRemoveItems();
    m_comboFeatSelect[1].SetCanRemoveItems();
    m_comboFeatSelect[2].SetCanRemoveItems();
    m_imagesSkills.Create(
            32,             // all icons are 32x32 pixels
            32,
            ILC_COLOR32,
            0,
            Skill_Count);
    for (size_t si = Skill_Unknown + 1; si < Skill_Count; ++si)
    {
        CImage image;
        HRESULT result = LoadImageFile(
                IT_ui,
                (LPCTSTR)EnumEntryText((SkillType)si, skillTypeMap),
                &image);
        if (result == S_OK)
        {
            CBitmap bitmap;
            bitmap.Attach(image.Detach());
            m_imagesSkills.Add(&bitmap, c_transparentColour); // standard mask color (purple)
        }
    }
}

CLevelUpView::~CLevelUpView()
{
}

void CLevelUpView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_CLASS1, m_comboClass[0]);
    DDX_Control(pDX, IDC_COMBO_CLASS2, m_comboClass[1]);
    DDX_Control(pDX, IDC_COMBO_CLASS3, m_comboClass[2]);
    DDX_Control(pDX, IDC_LIST_SKILLS, m_listSkills);
    DDX_Control(pDX, IDC_COMBO_SKILLTOME, m_comboSkillTome);
    if (!pDX->m_bSaveAndValidate)
    {
        VERIFY(m_sortHeader.SubclassWindow(m_listSkills.GetHeaderCtrl()->GetSafeHwnd()));
    }
    DDX_Control(pDX, IDC_EDIT_SKILLPOINTS, m_editSkillPoints);
    DDX_Control(pDX, IDC_BUTTON_SKILL_PLUS, m_buttonPlus);
    DDX_Control(pDX, IDC_BUTTON_SKILL_MINUS, m_buttonMinus);
    DDX_Control(pDX, IDC_FEAT_TYPE1, m_staticFeatDescription[0]);
    DDX_Control(pDX, IDC_FEAT_TYPE2, m_staticFeatDescription[1]);
    DDX_Control(pDX, IDC_FEAT_TYPE3, m_staticFeatDescription[2]);
    DDX_Control(pDX, IDC_COMBO_FEATSELECT1, m_comboFeatSelect[0]);
    DDX_Control(pDX, IDC_COMBO_FEATSELECT2, m_comboFeatSelect[1]);
    DDX_Control(pDX, IDC_COMBO_FEATSELECT3, m_comboFeatSelect[2]);
    DDX_Control(pDX, IDC_LIST_AUTOFEATS, m_listAutomaticFeats);
    DDX_Control(pDX, IDC_LIST_GRANTEDFEATS, m_listGrantedFeats);
    DDX_Control(pDX, IDC_STATIC_CLASS, m_staticClass);
    DDX_Control(pDX, IDC_STATIC_SP_AVAILABLE, m_staticAvailableSpend);
    DDX_Control(pDX, IDC_BUTTON_SKILLS, m_buttonSkillsDialog);
    DDX_Control(pDX, IDC_STATIC_STR, m_abilitiesAtLevel[0]);
    DDX_Control(pDX, IDC_STATIC_DEX, m_abilitiesAtLevel[1]);
    DDX_Control(pDX, IDC_STATIC_CON, m_abilitiesAtLevel[2]);
    DDX_Control(pDX, IDC_STATIC_INT, m_abilitiesAtLevel[3]);
    DDX_Control(pDX, IDC_STATIC_WIS, m_abilitiesAtLevel[4]);
    DDX_Control(pDX, IDC_STATIC_CHA, m_abilitiesAtLevel[5]);
    DDX_Control(pDX, IDC_STATIC_BAB, m_staticBab);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CLevelUpView, CFormView)
    ON_WM_SIZE()
    ON_WM_WINDOWPOSCHANGING()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_NOTIFY(HDN_ENDTRACK, IDC_LIST_SKILLS, OnEndtrackListSkills)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, IDC_LIST_SKILLS, OnEndtrackListSkills)
    ON_NOTIFY(HDN_ENDTRACK, IDC_LIST_AUTOFEATS, OnEndtrackListAutomaticFeats)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, IDC_LIST_AUTOFEATS, OnEndtrackListAutomaticFeats)
    ON_NOTIFY(HDN_ENDTRACK, IDC_LIST_GRANTEDFEATS, OnEndtrackListGrantedFeats)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, IDC_LIST_GRANTEDFEATS, OnEndtrackListGrantedFeats)
    ON_NOTIFY(NM_HOVER, IDC_LIST_AUTOFEATS, OnHoverAutomaticFeats)
    ON_NOTIFY(NM_HOVER, IDC_LIST_GRANTEDFEATS, OnHoverGrantedFeats)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SKILLS, OnColumnclickListSkills)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_SKILLS, OnDoubleClickListSkills)
    ON_NOTIFY(NM_CLICK, IDC_LIST_SKILLS, OnLeftClickListSkills)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_SKILLS, OnRightClickListSkills)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SKILLS, OnItemchangedListSkills)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SKILLS, OnCustomDrawSkills)
    ON_CBN_SELENDOK(IDC_COMBO_CLASS1, OnClass1Selected)
    ON_CBN_SELENDOK(IDC_COMBO_CLASS2, OnClass2Selected)
    ON_CBN_SELENDOK(IDC_COMBO_CLASS3, OnClass3Selected)
    ON_CBN_SELENDOK(IDC_COMBO_SKILLTOME, OnSkillTomeSelected)
    ON_CBN_SELENDCANCEL(IDC_COMBO_SKILLTOME, OnSkillTomeCancel)
    ON_BN_CLICKED(IDC_BUTTON_SKILL_PLUS, OnButtonSkillPlus)
    ON_BN_CLICKED(IDC_BUTTON_SKILL_MINUS, OnButtonSkillMinus)
    ON_BN_CLICKED(IDC_BUTTON_SKILLS, OnButtonSkillsDialog)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_FEATSELECT1, IDC_COMBO_FEATSELECT3, OnFeatSelection)
    ON_CONTROL_RANGE(CBN_SELENDCANCEL, IDC_COMBO_FEATSELECT1, IDC_COMBO_FEATSELECT3, OnFeatSelectionCancel)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_REGISTERED_MESSAGE(UWM_UPDATE_COMPLETE, OnUpdateComplete)
    ON_WM_CAPTURECHANGED()
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_MOUSEENTER, OnMouseEnter)
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_MESSAGE(WM_MOUSEHOVER, OnHoverComboBox)
    ON_REGISTERED_MESSAGE(UWM_TOGGLE_FEAT, OnToggleFeatIgnore)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CLevelUpView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLevelUpView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

LRESULT CLevelUpView::OnNewDocument(
        WPARAM wParam,          // document pointer
        LPARAM lParam)          // not used
{
    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    if (pCharacter != m_pCharacter)
    {
        if (m_pCharacter != NULL)
        {
            // character has changed, don't watch the old any more
            m_pCharacter->DetachObserver(this);
        }
        m_pCharacter = pCharacter;
        if (m_pCharacter != NULL)
        {
            // watch for any changes we need to update on from the document
            m_pCharacter->AttachObserver(this);
            m_level = 0;        // default to level 1 when document changed
        }
        // ensure all controls show correct state, even for no document
        PopulateControls();
        EnableBuyButtons();
    }
    if (IsWindow(GetSafeHwnd()))
    {
        // do a resize to show/hide controls as required
        // as granted feats control may now have changed
        CRect rect;
        GetClientRect(rect);
        OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
    }
    return 0L;
}

void CLevelUpView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    m_tooltip.Create(this);
    m_tipCreated = true;

    // create the level buttons
    CRect itemRect(
            c_controlSpacing,
            c_controlSpacing,
            c_levelButtonSizeX + c_controlSpacing,
            c_levelButtonSizeY + c_controlSpacing);
    for (size_t i = 0 ; i < MAX_LAMANNIA_LEVEL; ++i)
    {
        m_buttonLevels[i].SetLevel(i + 1);
        m_buttonLevels[i].Create("", WS_CHILD | WS_VISIBLE, itemRect, this, IDC_LEVEL1 + i);
        itemRect += CPoint(0, itemRect.Height() + c_controlSpacing);
        CRect rect(itemRect);
        ClientToScreen(&rect);
        m_hookLevelHandles[i] = GetMouseHook()->AddRectangleToMonitor(
                this->GetSafeHwnd(),
                rect,           // screen coordinates,
                WM_MOUSEENTER,
                WM_MOUSELEAVE,
                false);
    }

    // setup the list control columns
    for (size_t i = 0 ; i < SLC_Count; ++i)
    {
        switch (i)
        {
        case SLC_name:
            m_listSkills.InsertColumn(SLC_name, "Skill", LVCFMT_LEFT, 100);
            break;
        case SLC_Ability:
            m_listSkills.InsertColumn(SLC_Ability, "Ability", LVCFMT_LEFT, 100);
            break;
        case SLC_Tome:
            m_listSkills.InsertColumn(SLC_Tome, "Tome", LVCFMT_LEFT, 100);
            break;
        case SLC_Max:
            m_listSkills.InsertColumn(SLC_Max, "Max", LVCFMT_LEFT, 100);
            break;
        case SLC_CrossClassAtLevel:
            m_listSkills.InsertColumn(SLC_CrossClassAtLevel, "xClass", LVCFMT_LEFT, 100);
            break;
        case SLC_Spend:
            m_listSkills.InsertColumn(SLC_Spend, "Spent at Level", LVCFMT_LEFT, 100);
            break;
        case SLC_Total:
            m_listSkills.InsertColumn(SLC_Total, "Total", LVCFMT_LEFT, 100);
            break;
        }
    }
    LoadColumnWidthsByName(&m_listSkills, "LevelUpSkills_%s");
    m_listSkills.SetImageList(&m_imagesSkills, LVSIL_SMALL);
    m_listSkills.SetImageList(&m_imagesSkills, LVSIL_NORMAL);
    m_listSkills.SetExtendedStyle(
            m_listSkills.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    // add the skills to the list control
    for (size_t si = Skill_Unknown + 1; si < Skill_Count; ++si)
    {
        std::string text;
        text = (LPCTSTR)EnumEntryText(
                (SkillType)si,
                skillTypeMap);
        size_t index = m_listSkills.InsertItem(
                0,
                text.c_str(),
                si - 1);        // icon images are 0 based, our skill enum is 1 based
        m_listSkills.SetItemData(index, si);    // go by item data as list may be sorted

        AbilityType at = StatFromSkill((SkillType)si);
        text = (LPCTSTR)EnumEntryText(
                at,
                abilityTypeMap);
        text.resize(3);     // truncate to 1st 3 characters, e.g. "Strength" becomes "Str"
        m_listSkills.SetItemText(index, SLC_Ability, text.c_str());
    }
    m_sortHeader.SetSortArrow(0, TRUE);
    m_listSkills.SortItems(CLevelUpView::SortCompareFunction, (long)GetSafeHwnd());

    m_listAutomaticFeats.InsertColumn(0, "Automatic Feats", LVCFMT_LEFT, 100);
    m_listAutomaticFeats.SetExtendedStyle(
            m_listAutomaticFeats.GetExtendedStyle()
            | LVS_EX_FULLROWSELECT
            | LVS_EX_TRACKSELECT
            //| LVS_EX_LABELTIP); // stops hover tooltips from working
            );
    LoadColumnWidthsByName(&m_listAutomaticFeats, "LevelUpAutoFeats_%s");

    m_listGrantedFeats.InsertColumn(0, "Granted Feats", LVCFMT_LEFT, 100);
    m_listGrantedFeats.SetExtendedStyle(
            m_listGrantedFeats.GetExtendedStyle()
            | LVS_EX_FULLROWSELECT
            | LVS_EX_TRACKSELECT
            //| LVS_EX_LABELTIP); // stops hover tooltips from working
            );
    LoadColumnWidthsByName(&m_listGrantedFeats, "LevelUpGrantedFeats_%s");

    // put the available skill tome values in the skill tome select combo box
    size_t index = m_comboSkillTome.AddString("No Tome");
    m_comboSkillTome.SetItemData(index, 0);
    for (size_t i = 1; i <= MAX_SKILL_TOME; ++i)
    {
        CString text;
        text.Format("%+d Tome", i);
        index = m_comboSkillTome.AddString(text);
        m_comboSkillTome.SetItemData(index, 1);
    }

    PopulateControls();
    EnableBuyButtons();

    // add the mouse hook areas used for feat tooltips
    for (size_t i = 0; i < 3; ++i)
    {
        CRect rect;
        m_comboFeatSelect[i].GetWindowRect(&rect);
        rect.bottom = rect.top + 32 + GetSystemMetrics(SM_CYBORDER) * 4;
        m_hookFeatHandles[i] = GetMouseHook()->AddRectangleToMonitor(
                this->GetSafeHwnd(),
                rect,           // screen coordinates,
                WM_MOUSEENTER,
                WM_MOUSELEAVE,
                false);
    }
}

void CLevelUpView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);
    if (IsWindow(m_listSkills.GetSafeHwnd()))
    {
        // ensure space for level select buttons
        CRect rctButton;
        m_buttonLevels[0].GetWindowRect(&rctButton);
        ScreenToClient(&rctButton);
        rctButton -= rctButton.TopLeft();
        rctButton += CPoint(c_controlSpacing, c_controlSpacing);
        for (size_t bi = 0; bi < MAX_LAMANNIA_LEVEL; ++bi)
        {
            m_buttonLevels[bi].MoveWindow(rctButton);
            rctButton += CPoint(0, rctButton.Height() + c_controlSpacing);
            if (rctButton.bottom > cy - c_controlSpacing
                    && bi < MAX_LAMANNIA_LEVEL - 1)  // no need to move on if its the last one
            {
                // not enough room for this button, start another column
                rctButton -= CPoint(0, rctButton.top);
                rctButton += CPoint(rctButton.Width() + c_controlSpacing, c_controlSpacing);
            }
        }

        // position all the controls above
        // [level] [heroic classes] [combo1] [combo2] [combo3]
        // [level] [str][dex][con][int][wis][cha][bab]
        // [level] [Feat type 1] [Feat type 2] [Feat type 3]
        // [level] [Feat combo1] [Feat combo2] [Feat combo3]
        // [level] [Available][count] [+][-][Auto spend]
        // [level] +------------------------------------+ +----------------+
        // [level] | Skills list                        | | Automatic feats|
        // [level] |                                    | +----------------+
        // [level] |                                    | +----------------+
        // [level] |                                    | | Granted feats  |
        // [level] +------------------------------------+ +----------------+
        // Note that granted feats are only shown if there are granted feats
        CRect rctClassLabel;
        m_staticClass.GetWindowRect(&rctClassLabel);
        rctClassLabel -= rctClassLabel.TopLeft();
        rctClassLabel += CPoint(rctButton.right + c_controlSpacing, c_controlSpacing);
        // now the 3 combo boxes
        CRect rctClassCombo[3];
        m_comboClass[0].GetWindowRect(&rctClassCombo[0]);
        m_comboClass[1].GetWindowRect(&rctClassCombo[1]);
        m_comboClass[2].GetWindowRect(&rctClassCombo[2]);
        rctClassCombo[0] -= rctClassCombo[0].TopLeft();
        rctClassCombo[1] -= rctClassCombo[1].TopLeft();
        rctClassCombo[2] -= rctClassCombo[2].TopLeft();
        rctClassCombo[0] += CPoint(rctClassLabel.right + c_controlSpacing, c_controlSpacing);
        rctClassCombo[1] += CPoint(rctClassCombo[0].right + c_controlSpacing, c_controlSpacing);
        rctClassCombo[2] += CPoint(rctClassCombo[1].right + c_controlSpacing, c_controlSpacing);
        // current abilities
        CRect rectAbilities[6];
        for (size_t i = 0; i < 6; ++i)
        {
            m_abilitiesAtLevel[i].GetWindowRect(&rectAbilities[i]);
            rectAbilities[i] -= rectAbilities[i].TopLeft();
            if (i == 0)
            {
                rectAbilities[i] += CPoint(rctButton.right + c_controlSpacing, rctClassLabel.bottom + c_controlSpacing);
            }
            else
            {
                rectAbilities[i] += CPoint(rectAbilities[i-1].right + c_controlSpacing, rctClassLabel.bottom + c_controlSpacing);
            }
        }
        // BAB shown after abilities
        CRect rctBab;
        m_staticBab.GetWindowRect(&rctBab);
        rctBab -= rctBab.TopLeft();
        rctBab += CPoint(rectAbilities[5].right + c_controlSpacing, rctClassLabel.bottom + c_controlSpacing);

        // feat type labels
        CRect rctFeatType[3];
        m_staticFeatDescription[0].GetWindowRect(&rctFeatType[0]);
        m_staticFeatDescription[1].GetWindowRect(&rctFeatType[1]);
        m_staticFeatDescription[2].GetWindowRect(&rctFeatType[2]);
        rctFeatType[0] -= rctFeatType[0].TopLeft();
        rctFeatType[1] -= rctFeatType[1].TopLeft();
        rctFeatType[2] -= rctFeatType[2].TopLeft();
        rctFeatType[0] += CPoint(rctButton.right + c_controlSpacing, rectAbilities[0].bottom + c_controlSpacing);
        rctFeatType[1] += CPoint(rctFeatType[0].right + c_controlSpacing, rectAbilities[0].bottom + c_controlSpacing);
        rctFeatType[2] += CPoint(rctFeatType[1].right + c_controlSpacing, rectAbilities[0].bottom + c_controlSpacing);
        // feat drop list combos
        CRect rctFeatCombo[3];
        m_comboFeatSelect[0].GetWindowRect(&rctFeatCombo[0]);
        m_comboFeatSelect[1].GetWindowRect(&rctFeatCombo[1]);
        m_comboFeatSelect[2].GetWindowRect(&rctFeatCombo[2]);
        rctFeatCombo[0] -= rctFeatCombo[0].TopLeft();
        rctFeatCombo[1] -= rctFeatCombo[1].TopLeft();
        rctFeatCombo[2] -= rctFeatCombo[2].TopLeft();
        rctFeatCombo[0] += CPoint(rctButton.right + c_controlSpacing, rctFeatType[0].bottom + c_controlSpacing);
        rctFeatCombo[1] += CPoint(rctFeatCombo[0].right + c_controlSpacing, rctFeatType[0].bottom + c_controlSpacing);
        rctFeatCombo[2] += CPoint(rctFeatCombo[1].right + c_controlSpacing, rctFeatType[0].bottom + c_controlSpacing);
        // skill controls
        CRect rctSkillControls[5];
        m_staticAvailableSpend.GetWindowRect(&rctSkillControls[0]);
        m_editSkillPoints.GetWindowRect(&rctSkillControls[1]);
        m_buttonPlus.GetWindowRect(&rctSkillControls[2]);
        m_buttonMinus.GetWindowRect(&rctSkillControls[3]);
        m_buttonSkillsDialog.GetWindowRect(&rctSkillControls[4]);
        ScreenToClient(rctSkillControls[0]);
        int top = rctSkillControls[0].top;
        rctSkillControls[0] -= rctSkillControls[0].TopLeft();
        rctSkillControls[1] -= rctSkillControls[1].TopLeft();
        rctSkillControls[2] -= rctSkillControls[2].TopLeft();
        rctSkillControls[3] -= rctSkillControls[3].TopLeft();
        rctSkillControls[4] -= rctSkillControls[4].TopLeft();
        rctSkillControls[0] += CPoint(rctButton.right + c_controlSpacing, top);
        rctSkillControls[1] += CPoint(rctSkillControls[0].right + c_controlSpacing, top);
        rctSkillControls[2] += CPoint(rctSkillControls[1].right + c_controlSpacing, top);
        rctSkillControls[3] += CPoint(rctSkillControls[2].right + c_controlSpacing, top);
        rctSkillControls[4] += CPoint(rctSkillControls[3].right + c_controlSpacing, top);

        // measure how wide the skills list control needs to be
        CSize skillSize = RequiredControlSizeForContent(
                &m_listSkills,
                Skill_Count);       // max #items

        int leftSpace = cx - (rctButton.right + c_controlSpacing);

        // at least 1/3rd of width to auto feats
        if (skillSize.cx > (leftSpace * 2) / 3)
        {
            skillSize.cx = (leftSpace * 2) / 3;
        }

        CRect rctSkills(
                rctButton.right + c_controlSpacing,
                rctSkillControls[0].bottom + c_controlSpacing,
                rctButton.right + c_controlSpacing + skillSize.cx,
                cy - c_controlSpacing);

        CRect rctAutoFeats;
        m_listAutomaticFeats.GetWindowRect(rctAutoFeats);
        rctAutoFeats -= rctAutoFeats.TopLeft();
        rctAutoFeats += CPoint(
                rctSkills.right + c_controlSpacing,
                rctSkills.top);
        rctAutoFeats.bottom = cy - c_controlSpacing;
        rctAutoFeats.right = cx - c_controlSpacing;

        // are we showing granted feats?
        if (m_pCharacter != NULL
                && m_pCharacter->HasGrantedFeats())
        {
            // give them 1/3rd of the height of the automatic feats
            rctAutoFeats.bottom -= rctAutoFeats.Height() / 3;
            // same for the granted feats list control
            CRect rctGrantedFeats(rctAutoFeats);
            rctGrantedFeats.top = rctAutoFeats.bottom + c_controlSpacing;
            rctGrantedFeats.bottom = cy - c_controlSpacing;
            m_listGrantedFeats.MoveWindow(rctGrantedFeats);
            m_listGrantedFeats.ShowWindow(SW_SHOW);
        }
        else
        {
            // granted feat controls not shown
            m_listGrantedFeats.ShowWindow(SW_HIDE);
        }

        // now we have all the rectangle positions, move all the controls
        m_staticClass.MoveWindow(rctClassLabel);
        m_comboClass[0].MoveWindow(rctClassCombo[0]);
        m_comboClass[1].MoveWindow(rctClassCombo[1]);
        m_comboClass[2].MoveWindow(rctClassCombo[2]);
        for (size_t i = 0; i < 6; ++i)
        {
            m_abilitiesAtLevel[i].MoveWindow(rectAbilities[i]);
        }
        m_staticBab.MoveWindow(rctBab);
        m_staticFeatDescription[0].MoveWindow(rctFeatType[0]);
        m_staticFeatDescription[1].MoveWindow(rctFeatType[1]);
        m_staticFeatDescription[2].MoveWindow(rctFeatType[2]);
        m_comboFeatSelect[0].MoveWindow(rctFeatCombo[0]);
        m_comboFeatSelect[1].MoveWindow(rctFeatCombo[1]);
        m_comboFeatSelect[2].MoveWindow(rctFeatCombo[2]);
        m_staticAvailableSpend.MoveWindow(rctSkillControls[0]);
        m_editSkillPoints.MoveWindow(rctSkillControls[1]);
        m_buttonPlus.MoveWindow(rctSkillControls[2]);
        m_buttonMinus.MoveWindow(rctSkillControls[3]);
        m_buttonSkillsDialog.MoveWindow(rctSkillControls[4]);
        m_listSkills.MoveWindow(rctSkills, TRUE);
        m_listAutomaticFeats.MoveWindow(rctAutoFeats, TRUE);

        UpdateHookRectangles();
        SetScaleToFitSize(CSize(cx, cy));
    }
}

void CLevelUpView::UpdateHookRectangles()
{
    if (IsWindow(m_listSkills.GetSafeHwnd()))
    {
        // update the mouse hook handles for tooltips
        for (size_t i = 0; i < 3; ++i)
        {
            CRect rect;
            m_comboFeatSelect[i].GetWindowRect(&rect);
            // gives the wrong rectangle, ensure large enough
            rect.bottom = rect.top + 32 + GetSystemMetrics(SM_CYBORDER) * 4;
            GetMouseHook()->UpdateRectangle(
                    m_hookFeatHandles[i],
                    rect);          // screen coordinates
        }
        for (size_t i = 0; i < MAX_LAMANNIA_LEVEL; ++i)
        {
            CRect rect;
            m_buttonLevels[i].GetWindowRect(&rect);
            GetMouseHook()->UpdateRectangle(
                    m_hookLevelHandles[i],
                    rect);          // screen coordinates
        }
    }
}

void CLevelUpView::OnWindowPosChanging(WINDOWPOS * pos)
{
    // ensure tooltip locations are correct on window move
    CFormView::OnWindowPosChanging(pos);
    UpdateHookRectangles();
}

// DocumentObserver overrides
void CLevelUpView::UpdateAlignmentChanged(
        Character * pCharacter,
        AlignmentType alignment)
{
    // alignment can affect available classes
    PopulateControls();
}

void CLevelUpView::UpdateSkillSpendChanged(
        Character * pCharacter,
        size_t level,
        SkillType skill)
{
    // skills have been affected
    DetermineTrainableFeats();
    PopulateSkills();
    EnableControls();
}

void CLevelUpView::UpdateSkillTomeChanged(
        Character * pCharacter,
        SkillType skill)
{
    // skills have been affected
    DetermineTrainableFeats();
    PopulateSkills();
}

void CLevelUpView::UpdateClassChoiceChanged(Character * pCharacter)
{
    SetAvailableClasses();
}

void CLevelUpView::UpdateClassChanged(
        Character * pCharacter,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    PopulateControls();
}

void CLevelUpView::UpdateAbilityValueChanged(
        Character * pCharacter,
        AbilityType ability)
{
    // all stats can affect trainable feats
    DetermineTrainableFeats();
    SetAbilitiesAtLevel();
    if (ability == Ability_Intelligence)
    {
        // only intelligence affects skill point spend
        EnableControls();
    }
}

void CLevelUpView::UpdateAbilityTomeChanged(
        Character * pCharacter,
        AbilityType ability)
{
    // all stats can affect trainable feats
    DetermineTrainableFeats();
    SetAbilitiesAtLevel();
    if (ability == Ability_Intelligence)
    {
        // only intelligence affects skill point spend
        PopulateControls();
    }
}

void CLevelUpView::UpdateRaceChanged(
        Character * pCharacter,
        RaceType race)
{
    // affects feat selections at level 1 and automatic feats
    // at 1st level only
    SetAbilitiesAtLevel();
    if (m_level == 0)
    {
        PopulateControls();
    }
}

void CLevelUpView::UpdateFeatTrained(
        Character * pCharacter,
        const std::string & featName)
{
    // handle the view updates once only for a large number of feat changes
    if (m_canPostMessage)
    {
        PostMessage(UWM_UPDATE_COMPLETE, 0, 0L);
        m_canPostMessage = false;
    }
}

void CLevelUpView::UpdateFeatRevoked(
        Character * pCharacter,
        const std::string & featName)
{
    // handle the view updates once only for a large number of feat changes
    if (m_canPostMessage)
    {
        PostMessage(UWM_UPDATE_COMPLETE, 0, 0L);
        m_canPostMessage = false;
    }
}

void CLevelUpView::UpdateGrantedFeatsChanged(Character * charData)
{
    PopulateGrantedFeats();
    // do a resize to show/hide controls as required
    CRect rect;
    GetClientRect(rect);
    OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

void CLevelUpView::PopulateControls()
{
    if (m_pCharacter != NULL)
    {
        CString text;
        text.Format("Level Up - Level %d", m_level + 1);
        GetParent()->SetWindowText(text);
    }
    else
    {
        GetParent()->SetWindowText("Level Up");
    }

    SetLevelButtonStates();
    SetAbilitiesAtLevel();
    PopulateSkills();
    SetAvailableClasses();

    DetermineTrainableFeats();
    EnableControls();
    PopulateAutomaticFeats();
    PopulateGrantedFeats();

    ShowBab();
}

void CLevelUpView::SetLevelButtonStates()
{
    for (size_t i = 0; i < MAX_LAMANNIA_LEVEL; ++i)
    {
        m_buttonLevels[i].SetSelected(m_level == i);
        ClassType type = Class_Unknown;
        if (m_pCharacter != NULL && i < m_pCharacter->MaxLevel())
        {
            const LevelTraining & ld = m_pCharacter->LevelData(i);
            type = ld.HasClass() ? ld.Class() : Class_Unknown;
            m_buttonLevels[i].EnableWindow(i <= m_pCharacter->MaxLevel());
            m_buttonLevels[i].ShowWindow(SW_SHOW);
            // there are issues with this level if not all feats have been
            // trained
            std::vector<TrainableFeatTypes> trainable =
                    m_pCharacter->TrainableFeatTypeAtLevel(i);
            bool hasIssue = false;
            for (size_t tft = 0; tft < trainable.size(); ++tft)
            {
                TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                        i,
                        trainable[tft]);
                if (tf.FeatName().empty())
                {
                    hasIssue = true;
                }
                // can have an issue if the level skills are over spent
                // this can happen if you train skill levels in the wrong order
                // e.g. at level 2 you put 5 points into a class skill, then
                // at level 1 you put additional skill points into that skill
                // taking the total past what is allowed at level 2.
                for (size_t si = Skill_Unknown + 1; si < Skill_Count; ++si)
                {
                    if (m_pCharacter->SkillAtLevel((SkillType)si, i, false)
                            > m_pCharacter->MaxSkillForLevel((SkillType)si, i))
                    {
                        hasIssue = true;
                    }
                }
            }
            m_buttonLevels[i].SetIssueState(hasIssue);
        }
        else
        {
            // no character, buttons disabled
            m_buttonLevels[i].EnableWindow(FALSE);
            m_buttonLevels[i].SetIssueState(false);
            if (m_pCharacter != NULL && i >= m_pCharacter->MaxLevel())
            {
                m_buttonLevels[i].ShowWindow(SW_HIDE);
            }
        }
        m_buttonLevels[i].SetClass(type);
    }
}

void CLevelUpView::PopulateSkills()
{
    // the list control keeps its content, except for column values
    // SLC_Tome, SLC_Max and SLC_Spend
    size_t count = m_listSkills.GetItemCount();
    for (size_t ii = 0; ii < count; ++ii)
    {
        if (m_pDocument != NULL)
        {
            const LevelTraining & levelData = m_pCharacter->LevelData(m_level);
            CString text;
            SkillType skill = (SkillType)m_listSkills.GetItemData(ii);
            size_t tome = m_pCharacter->SkillTomeValue(skill);
            if (tome > 0)
            {
                text.Format("%+d Tome", tome);
                m_listSkills.SetItemText(ii, SLC_Tome, text);
            }
            else
            {
                m_listSkills.SetItemText(ii, SLC_Tome, "No Tome");
            }
            // work out how high the skill can go for this level
            double maxSkill = m_pCharacter->MaxSkillForLevel(
                    skill,
                    m_level);
            text.Format("%.1f", maxSkill);
            text.Replace(".0", "");
            text.Replace(".5", "�");
            m_listSkills.SetItemText(ii, SLC_Max, text);

            size_t spentOnSkill = m_pCharacter->SpentAtLevel(
                    skill,
                    m_level);
            // show whether the it is a cross class skill at this level
            if (IsClassSkill(levelData.HasClass() ? levelData.Class() : Class_Unknown, skill))
            {
                m_listSkills.SetItemText(ii, SLC_CrossClassAtLevel, "Class Skill");
                text.Format("%d", spentOnSkill);
            }
            else
            {
                m_listSkills.SetItemText(ii, SLC_CrossClassAtLevel, "Cross Class");
                text = "";
                int fullRanks = (spentOnSkill / 2);
                if (fullRanks > 0)
                {
                    text.Format("%d", fullRanks);
                }
                // its a cross class skill, show in multiples of �
                if (spentOnSkill % 2 != 0)
                {
                    text += "�";
                }
            }
            // how many skill points have been spent on this skill at this level so far?
            m_listSkills.SetItemText(ii, SLC_Spend, text);

            // current skill value after all spends at this and previous levels counted
            double skillValue = m_pCharacter->SkillAtLevel(
                    skill,
                    m_level,
                    false);     // skill tome not included
            // maxSkill = 0 for skills that must be a class skill to train
            // non zero means its a class skill or a cross class skill
            if (maxSkill != 0)
            {
                text.Format("%.1f", skillValue);
                text.Replace(".0", "");
                text.Replace(".5", "�");
            }
            else
            {
                // this is a skill that must be a class skill to be able to train
                // e.g. Disable Device, Perform, Open Lock
                // can have trained ranks if the user subsequently switch classes
                if (skillValue > 0)
                {
                    text.Format("N/A (%.1f)", skillValue);
                    text.Replace(".0", "");
                    text.Replace(".5", "�");
                }
                else
                {
                    text = "N/A";
                }
            }
            m_listSkills.SetItemText(ii, SLC_Total, text);
        }
        else
        {
            // no document open at the moment, no data to display
            m_listSkills.SetItemText(ii, SLC_Tome, "-");
            m_listSkills.SetItemText(ii, SLC_Max, "-");
            m_listSkills.SetItemText(ii, SLC_CrossClassAtLevel, "-");
            m_listSkills.SetItemText(ii, SLC_Spend, "-");
        }
    }
    m_listSkills.SortItems(CLevelUpView::SortCompareFunction, (long)GetSafeHwnd());
}

void CLevelUpView::SetAvailableClasses()
{
    // Available classes
    m_comboClass[0].ResetContent();
    m_comboClass[1].ResetContent();
    m_comboClass[2].ResetContent();
    if (m_pDocument != NULL)
    {
        // select the 3 possible classes
        ClassType type1 = m_pCharacter->Class1();
        ClassType type2 = m_pCharacter->Class2();
        ClassType type3 = m_pCharacter->Class3();
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            if ((m_pCharacter->IsClassAvailable((ClassType)ci)
                    && ci != type2
                    && ci != type3
                    || ci == Class_Unknown)
                && (!m_pCharacter->IsClassRestricted((ClassType)ci)
                    || ci == type1
                    || SameArchetype((ClassType)ci, type1)))
            {
                // this class is selectable
                CString text = EnumEntryText(
                        (ClassType)ci,
                        classTypeMap);
                size_t i = m_comboClass[0].AddString(text);
                m_comboClass[0].SetItemData(i, ci);
            }
        }
        SelectComboboxEntry(type1, &m_comboClass[0]);
        m_comboClass[0].EnableWindow(TRUE);

        // if we have a class1, then enable the control selection for class 2
        if (type1 != Class_Unknown)
        {
            for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
            {
                if ((m_pCharacter->IsClassAvailable((ClassType)ci)
                        && ci != type1
                        && ci != type3
                    || ci == Class_Unknown)
                    && (!m_pCharacter->IsClassRestricted((ClassType)ci)
                        || ci == type2
                        || SameArchetype((ClassType)ci, type2)))
                {
                    // this class is selectable at this level
                    CString text = EnumEntryText(
                            (ClassType)ci,
                            classTypeMap);
                    size_t i = m_comboClass[1].AddString(text);
                    m_comboClass[1].SetItemData(i, ci);
                }
            }
            SelectComboboxEntry(type2, &m_comboClass[1]);
            m_comboClass[1].EnableWindow(TRUE);

            // if we have a class2, then enable control selection for class 3
            if (type2 != Class_Unknown)
            {
                for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
                {
                    if ((m_pCharacter->IsClassAvailable((ClassType)ci)
                            && ci != type1
                            && ci != type2
                            || ci == Class_Unknown)
                        && (!m_pCharacter->IsClassRestricted((ClassType)ci)
                            || ci == type3
                            || SameArchetype((ClassType)ci, type3)))
                    {
                        // this class is selectable at this level
                        CString text = EnumEntryText(
                                (ClassType)ci,
                                classTypeMap);
                        size_t i = m_comboClass[2].AddString(text);
                        m_comboClass[2].SetItemData(i, ci);
                    }
                }
                SelectComboboxEntry(type3, &m_comboClass[2]);
                m_comboClass[2].EnableWindow(TRUE);
            }
            else
            {
                // classes 3 not available until class2/1 selected
                m_comboClass[2].EnableWindow(FALSE);
            }
        }
        else
        {
            // classes 2/3 not available until class 1 selected
            m_comboClass[1].EnableWindow(FALSE);
            m_comboClass[2].EnableWindow(FALSE);
        }
    }
    else
    {
        m_comboClass[0].EnableWindow(FALSE);
        m_comboClass[1].EnableWindow(FALSE);
        m_comboClass[2].EnableWindow(FALSE);
    }
}

void CLevelUpView::EnableControls()
{
    // Control enable state depends on whether the base class to level up in
    // is selected or not. All controls are disabled until a class has been
    // selected.
    // Heroic levels 0-19 are selectable, levels 20+ automatically select class type "Epic"
    // Note that the available class list may be restricted due to alignment
    // and previous level class selections.
    if (m_pDocument != NULL)
    {
        const LevelTraining & level = m_pCharacter->LevelData(m_level);
        ClassType type = (level.HasClass() ? level.Class() : Class_Unknown);
        if (m_level < MAX_CLASS_LEVELS
                && type != Class_Unknown)
        {
            // skill point assign controls can be enabled
            m_listSkills.EnableWindow(TRUE);
            int available = level.SkillPointsAvailable();
            int spent = level.SkillPointsSpent();
            CString text;
            text.Format("%d of %d", (available - spent), available);
            m_editSkillPoints.SetWindowText(text);
        }
        else
        {
            // without a class selected or for epic levels, the user can not spend skill points
            m_listSkills.EnableWindow(FALSE);
            m_editSkillPoints.SetWindowText("-");
        }
        EnableBuyButtons();
    }
    else
    {
        m_comboClass[0].EnableWindow(FALSE);
        m_comboClass[1].EnableWindow(FALSE);
        m_comboClass[2].EnableWindow(FALSE);
        for (size_t i = 0; i < MAX_CLASS_LEVELS; ++i)
        {
            m_buttonLevels[i].EnableWindow(FALSE);
        }
    }
}

void CLevelUpView::EnableBuyButtons()
{
    // enable / disable the +/- skill point spend buttons
    // based on which skill is selected
    int sel = -1;
    POSITION pos = m_listSkills.GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        sel = m_listSkills.GetNextSelectedItem(pos);
    }
    if (sel >= 0 && m_pCharacter != NULL)
    {
        // we have a skill selection
        SkillType skill = (SkillType)m_listSkills.GetItemData(sel);
        bool canBuy = CanBuySkill(skill);
        bool canRevoke = CanRevokeSkill(skill);
        // set the button states
        m_buttonPlus.EnableWindow(canBuy);
        m_buttonMinus.EnableWindow(canRevoke);
    }
    else
    {
        m_buttonPlus.EnableWindow(false);
        m_buttonMinus.EnableWindow(false);
    }
    m_buttonSkillsDialog.EnableWindow(m_pCharacter != NULL);
}

void CLevelUpView::OnEndtrackListSkills(NMHDR* pNMHDR, LRESULT* pResult)
{
    // just save the column widths to registry so restored next time we run
    UNREFERENCED_PARAMETER(pNMHDR);
    UNREFERENCED_PARAMETER(pResult);
    SaveColumnWidthsByName(&m_listSkills, "LevelUpSkills_%s");
}

void CLevelUpView::OnEndtrackListAutomaticFeats(NMHDR* pNMHDR, LRESULT* pResult)
{
    // just save the column widths to registry so restored next time we run
    UNREFERENCED_PARAMETER(pNMHDR);
    UNREFERENCED_PARAMETER(pResult);
    SaveColumnWidthsByName(&m_listAutomaticFeats, "LevelUpAutoFeats_%s");
}

void CLevelUpView::OnEndtrackListGrantedFeats(NMHDR* pNMHDR, LRESULT* pResult)
{
    // just save the column widths to registry so restored next time we run
    UNREFERENCED_PARAMETER(pNMHDR);
    UNREFERENCED_PARAMETER(pResult);
    SaveColumnWidthsByName(&m_listGrantedFeats, "LevelUpGrantedFeats_%s");
}

void CLevelUpView::OnHoverAutomaticFeats(NMHDR* pNMHDR, LRESULT* pResult)
{
    // the user it hovering over a list control item. Identify it and display
    // the feat tooltip for this item
    CPoint mousePosition;
    GetCursorPos(&mousePosition);
    m_listAutomaticFeats.ScreenToClient(&mousePosition);

    LVHITTESTINFO hitInfo;
    hitInfo.pt = mousePosition;
    if (m_listAutomaticFeats.SubItemHitTest(&hitInfo) >= 0)
    {
        if (m_hoverItem != hitInfo.iItem)
        {
            // the item under the hover has changed
            m_hoverItem = hitInfo.iItem;
            // mouse is over a valid automatic feat, get the items rectangle and
            // show the feat tooltip
            CRect rect;
            m_listAutomaticFeats.GetItemRect(hitInfo.iItem, &rect, LVIR_BOUNDS);
            HideTip();
            // get the feat items text
            CString featName;
            featName = m_listAutomaticFeats.GetItemText(hitInfo.iItem, 0);
            m_listAutomaticFeats.ClientToScreen(&rect);
            CPoint tipTopLeft(rect.left, rect.bottom);
            CPoint tipAlternate(rect.left, rect.top);
            SetFeatTooltipText(featName, tipTopLeft, tipAlternate, false, TFT_Automatic, true); // left align
            m_showingTip = true;
            // make sure we don't stack multiple monitoring of the same rectangle
            if (m_automaticHandle == 0)
            {
                m_automaticHandle = GetMouseHook()->AddRectangleToMonitor(
                        this->GetSafeHwnd(),
                        rect,           // screen coordinates,
                        WM_MOUSEENTER,
                        WM_MOUSELEAVE,
                        true);
            }
        }
    }
}

void CLevelUpView::OnHoverGrantedFeats(NMHDR* pNMHDR, LRESULT* pResult)
{
    // the user it hovering over a list control item. Identify it and display
    // the feat tooltip for this item
    CPoint mousePosition;
    GetCursorPos(&mousePosition);
    m_listGrantedFeats.ScreenToClient(&mousePosition);

    LVHITTESTINFO hitInfo;
    hitInfo.pt = mousePosition;
    if (m_listGrantedFeats.SubItemHitTest(&hitInfo) >= 0)
    {
        if (m_hoverItem != hitInfo.iItem)
        {
            // the item under the hover has changed
            m_hoverItem = hitInfo.iItem;
            // mouse is over a valid granted feat, get the items rectangle and
            // show the feat tooltip
            CRect rect;
            m_listGrantedFeats.GetItemRect(hitInfo.iItem, &rect, LVIR_BOUNDS);
            HideTip();
            // get the feat items text
            CString featName;
            featName = m_listGrantedFeats.GetItemText(hitInfo.iItem, 0);
            m_listGrantedFeats.ClientToScreen(&rect);
            CPoint tipTopLeft(rect.left, rect.bottom);
            CPoint tipAlternate(rect.left, rect.top);
            SetFeatTooltipText(featName, tipTopLeft, tipAlternate, false, TFT_GrantedFeat, true); // left align
            m_showingTip = true;
            // make sure we don't stack multiple monitoring of the same rectangle
            if (m_automaticHandle == 0)
            {
                m_automaticHandle = GetMouseHook()->AddRectangleToMonitor(
                        this->GetSafeHwnd(),
                        rect,           // screen coordinates,
                        WM_MOUSEENTER,
                        WM_MOUSELEAVE,
                        true);
            }
        }
    }
}

void CLevelUpView::OnColumnclickListSkills(NMHDR* pNMHDR, LRESULT* pResult)
{
    // allow the user to sort the skills list based on the selected column
    // skill selected is identified by the item data which maps to the SkillType enum
    // when the control was populated.
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    size_t columnToSort = pNMListView->iSubItem;
    int column;
    bool bAscending;
    m_sortHeader.GetSortArrow(&column, &bAscending);
    if (column == columnToSort)
    {
        // just change sort direction
        bAscending = !bAscending;
    }
    else
    {
        // just change the column
        column = columnToSort;
    }
    m_sortHeader.SetSortArrow(column, bAscending);
    m_listSkills.SortItems(CLevelUpView::SortCompareFunction, (long)GetSafeHwnd());

    *pResult = 0;
}

int CLevelUpView::SortCompareFunction(
        LPARAM lParam1,
        LPARAM lParam2,
        LPARAM lParamSort)
{
    // this is a static function so we need to make our own this pointer
    CWnd * pWnd = CWnd::FromHandle((HWND)lParamSort);
    CLevelUpView * pThis = static_cast<CLevelUpView*>(pWnd);

    int sortResult = 0;
    size_t index1 = lParam1; // item data index, in our case the SkillType value
    size_t index2 = lParam2; // item data index, in our case the SkillType value

    // need to find the actual current entry in the list control to compare the
    // text content
    index1 = FindItemIndexByItemData(&pThis->m_listSkills, index1);
    index2 = FindItemIndexByItemData(&pThis->m_listSkills, index2);

    // get the current sort settings
    int column;
    bool bAscending;
    pThis->m_sortHeader.GetSortArrow(&column, &bAscending);

    // get the control text entries for the column being sorted
    CString index1Text = pThis->m_listSkills.GetItemText(index1, column);
    CString index2Text = pThis->m_listSkills.GetItemText(index2, column);

    // some columns are converted to numeric to do the sort
    // while others are compared as ASCII
    switch (column)
    {
    case SLC_Ability:
    case SLC_CrossClassAtLevel:
        // ASCII sorts
        sortResult = (index1Text < index2Text) ? -1 : 1;
        if (index1Text != index2Text)
        {
            break;
        }
        // if its the same text, sort by skill name instead
        index1Text = pThis->m_listSkills.GetItemText(index1, SLC_name);
        index2Text = pThis->m_listSkills.GetItemText(index2, SLC_name);
        // fall through
    case SLC_name:
        sortResult = (index1Text < index2Text) ? -1 : 1;
        break;
    case SLC_Tome:
    case SLC_Max:
    case SLC_Spend:
    case SLC_Total:
        {
            // numeric sorts
            index1Text.Replace("�", ".5");
            index2Text.Replace("�", ".5");
            double val1 = atof(index1Text);
            double val2 = atof(index2Text);
            if (val1 == val2)
            {
                // if numeric match, sort by skill name instead
                index1Text = pThis->m_listSkills.GetItemText(index1, SLC_name);
                index2Text = pThis->m_listSkills.GetItemText(index2, SLC_name);
                sortResult = (index1Text < index2Text) ? -1 : 1;
            }
            else
            {
                sortResult = (val1 < val2) ? -1 : 1;
            }
        }
        break;
    }
    if (!bAscending)
    {
        // switch sort direction result
        sortResult = -sortResult;
    }
    return sortResult;
}

void CLevelUpView::OnDoubleClickListSkills(NMHDR*, LRESULT* pResult)
{
    // determine whether the user has double clicked on a column tome selection
    CPoint mousePosition;
    GetCursorPos(&mousePosition);
    m_listSkills.ScreenToClient(&mousePosition);
    
    LVHITTESTINFO hitInfo;
    hitInfo.pt = mousePosition;
    if (m_listSkills.SubItemHitTest(&hitInfo) >= 0)
    {
        // the user has clicked on a valid item, check to see whether its tome entry column
        if (hitInfo.iSubItem == SLC_Tome)    // 1st column is labels
        {
            // it is the tome entry column
            // find which skill they wish to set the tome for
            m_skillBeingEdited = (SkillType)m_listSkills.GetItemData(hitInfo.iItem);
            // start an edit using the configured drop list combo which contains possible tome values (0..+5)
            CRect rctItem;
            m_listSkills.GetSubItemRect(hitInfo.iItem, hitInfo.iSubItem, LVIR_BOUNDS, rctItem);
            m_listSkills.ClientToScreen(&rctItem);
            ScreenToClient(&rctItem);
            rctItem.bottom += 300;  // ensure drop list is visible when displayed
            m_comboSkillTome.MoveWindow(rctItem);

            // make sure it has the correct item selected
            size_t value = m_pCharacter->SkillTomeValue(m_skillBeingEdited);
            m_comboSkillTome.SetCurSel(value);      // show previous selection for this skill
            m_comboSkillTome.ShowWindow(SW_SHOW);
            m_comboSkillTome.SetFocus();
            m_comboSkillTome.ShowDropDown(TRUE);    // allow new selection to go ahead
        }
    }
    *pResult = 0;
}

void CLevelUpView::OnLeftClickListSkills(NMHDR*, LRESULT* pResult)
{
    // this doesn't currently work properly
    //// this is a short cut to a skill point spend on the clicked skill
    //if (m_selectedSkill == m_listSkills.GetSelectionMark())
    //{
    //    // only buy if they clicked on the selected item
    //    OnButtonSkillPlus();
    //}
}

void CLevelUpView::OnRightClickListSkills(NMHDR*, LRESULT* pResult)
{
    // this doesn't currently work properly
    //// this is a short cut to a skill point revoke on the clicked skill
    //if (m_selectedSkill == m_listSkills.GetSelectionMark())
    //{
    //    // only revoke if they clicked on the selected item
    //    OnButtonSkillMinus();
    //}
}

void CLevelUpView::OnItemchangedListSkills(NMHDR* pNMHDR, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // ignore invalid notifications
    if (pNMListView->uNewState & LVIS_SELECTED)
    {
        // the selected skill has changed, update the buy buttons
        EnableBuyButtons();
        m_selectedSkill = pNMListView->iItem;
    }
}

void CLevelUpView::OnSkillTomeSelected()
{
    // a drop list selection for skill tome has completed
    int sel = m_comboSkillTome.GetCurSel();
    m_pCharacter->SetSkillTome(m_skillBeingEdited, sel);
    m_comboSkillTome.ShowWindow(SW_HIDE); // edit complete, hide the control
}

void CLevelUpView::OnSkillTomeCancel()
{
    // skill tome selection cancel
    m_comboSkillTome.ShowWindow(SW_HIDE); // edit complete, hide the control
}

void CLevelUpView::OnClass1Selected()
{
    // class selection complete
    ClassType type = (ClassType)GetComboboxSelection(&m_comboClass[0]);
    m_pCharacter->SetClass1(type);
}

void CLevelUpView::OnClass2Selected()
{
    // class selection complete
    ClassType type = (ClassType)GetComboboxSelection(&m_comboClass[1]);
    m_pCharacter->SetClass2(type);
}

void CLevelUpView::OnClass3Selected()
{
    // class selection complete
    ClassType type = (ClassType)GetComboboxSelection(&m_comboClass[2]);
    m_pCharacter->SetClass3(type);
}

bool CLevelUpView::CanBuySkill(SkillType skill) const
{
    bool canBuy = false;
    // more complicated as to whether we can buy.
    // depends on previous level buys and this level buys also
    const LevelTraining & levelData = m_pCharacter->LevelData(m_level);
    // is it a class skill for this level?
    bool isClassSkill = IsClassSkill(levelData.HasClass() ? levelData.Class() : Class_Unknown, skill);
    // get the maximum value the skill can be trained to at this level
    double maxSkill = m_pCharacter->MaxSkillForLevel(
            skill,
            m_level);
    // get the current value the skill has been trained to
    double currentSkill = m_pCharacter->SkillAtLevel(
            skill,
            m_level,
            false);     // skill tome not included
    // see how many skill points have yet to be spent at this level
    int available = SkillPoints(
            levelData.HasClass() ? levelData.Class() : Class_Unknown,
            m_pCharacter->Race(),
            m_pCharacter->AbilityAtLevel(Ability_Intelligence, m_level, (m_level != 0)),
            m_level);
    int spent = levelData.SkillPointsSpent();
    // also ensure we do not overspend a skill across all levels
    double maxSkill20 = m_pCharacter->MaxSkillForLevel(
            skill,
            MAX_CLASS_LEVELS-1);
    double currentSkill20 = m_pCharacter->SkillAtLevel(skill, MAX_CLASS_LEVELS-1, false);
    canBuy = ((maxSkill - currentSkill) > 0.1)      // covers rounding errors due to subtraction of doubles
            && ((available - spent) > 0)            // can go negative if intelligence adjusted down after spend
            && (currentSkill20 < maxSkill20);
    return canBuy;
}

bool CLevelUpView::CanRevokeSkill(SkillType skill) const
{
    bool canSell = false;
    // can be revoked if it has a previous spend at this level
    const LevelTraining & levelData = m_pCharacter->LevelData(m_level);
    const std::list<TrainedSkill> & trainedSkills = levelData.TrainedSkills();
    std::list<TrainedSkill>::const_iterator it = trainedSkills.begin();
    while (it != trainedSkills.end())
    {
        if ((*it).Skill() == skill)
        {
            // has had a spend on this skill at this level
            canSell = true;
        }
        ++it;
    }
    return canSell;
}

void CLevelUpView::OnButtonSkillPlus()
{
    // buy button is disabled for skills which cannot be spent on
    if (m_buttonPlus.IsWindowEnabled())
    {
        // let them spend a skill point on this skill
        int sel = -1;
        POSITION pos = m_listSkills.GetFirstSelectedItemPosition();
        if (pos != NULL)
        {
            // there is a selected skill
            sel = m_listSkills.GetNextSelectedItem(pos);
        }
        if (sel >= 0)
        {
            SkillType skill = (SkillType)m_listSkills.GetItemData(sel);
            m_pCharacter->SpendSkillPoint(m_level, skill);
        }
    }
}

void CLevelUpView::OnButtonSkillMinus()
{
    // revoke button is disabled for skills which cannot be revoked on
    if (m_buttonMinus.IsWindowEnabled())
    {
        // let them revoke a skill point on this skill
        int sel = -1;
        POSITION pos = m_listSkills.GetFirstSelectedItemPosition();
        if (pos != NULL)
        {
            // there is a selected skill
            sel = m_listSkills.GetNextSelectedItem(pos);
        }
        if (sel >= 0)
        {
            SkillType skill = (SkillType)m_listSkills.GetItemData(sel);
            m_pCharacter->RevokeSkillPoint(m_level, skill, false);
        }
    }
}

void CLevelUpView::OnButtonSkillsDialog()
{
    // no tooltips while a dialog is displayed
    GetMouseHook()->SaveState();
    CSkillSpendDialog dlg(m_pCharacter);
    dlg.DoModal();
    GetMouseHook()->RestoreState();
}

void CLevelUpView::DetermineTrainableFeats()
{
    // determine the list of available feat types that can be trained at this class/level
    // there can be a maximum of 3 choices (typically at 1st level)
    if (m_pDocument != NULL)
    {
        m_trainable = m_pCharacter->TrainableFeatTypeAtLevel(m_level);
    }
    else
    {
        // no document, so no feats, controls hidden and disabled
        m_trainable.clear();
    }

    // first show/hide the controls (max of 3 feats can be trained at any level)
    for (size_t fi = 0; fi < 3; ++fi)
    {
        if (fi < m_trainable.size())
        {
            // we have a label that shows what type of feat selection the combo is
            CString text = TrainableFeatTypeLabel(m_trainable[fi]);
            // show the trainable feat type description
            m_staticFeatDescription[fi].SetWindowText(text);
            m_staticFeatDescription[fi].EnableWindow(TRUE);
            m_staticFeatDescription[fi].ShowWindow(SW_SHOW);
            // and the combo box for feat selection
            m_comboFeatSelect[fi].EnableWindow(TRUE);
            m_comboFeatSelect[fi].ShowWindow(SW_SHOW);
            TrainedFeat tf = m_pCharacter->GetTrainedFeat(
                    m_level,
                    m_trainable[fi]);
            PopulateFeatCombobox(fi, tf.FeatName());
        }
        else
        {
            // no feat to train in this slot, disable and hide the controls
            m_staticFeatDescription[fi].EnableWindow(FALSE);
            m_staticFeatDescription[fi].ShowWindow(SW_HIDE);
            m_comboFeatSelect[fi].EnableWindow(FALSE);
            m_comboFeatSelect[fi].ShowWindow(SW_HIDE);
        }
    }
}

void CLevelUpView::PopulateFeatCombobox(
        size_t comboIndex,
        const std::string & selection)
{
    // its a valid selection
    // get a list of the available feats at current level that the user can train
    std::vector<Feat> availableFeats = m_pCharacter->TrainableFeats(
            m_trainable[comboIndex],
            m_level,
            selection);
    // include a "No Selection" feat, if > 1 feat in list
    // (i.e. user cannot un-train single feat selections)
    if (availableFeats.size() > 1
            && selection != "")         // must already have a selection also
    {
        Feat feat;
        feat.Set_Name(" No Selection");     // space causes to appear at top of list
        feat.Set_Description("Clears the current feat selection");
        feat.Set_Icon("NoSelection");
        availableFeats.push_back(feat);
    }
    std::sort(availableFeats.begin(), availableFeats.end());

    // lock the window to stop flickering while updates are done
    m_comboFeatSelect[comboIndex].LockWindowUpdate();
    m_comboFeatSelect[comboIndex].SetImageList(NULL);
    m_comboFeatSelect[comboIndex].ResetContent();

    m_imagesFeats[comboIndex].DeleteImageList();
    m_imagesFeats[comboIndex].Create(
            32,             // all icons are 32x32 pixels
            32,
            ILC_COLOR32,
            0,
            availableFeats.size());
    std::vector<Feat>::const_iterator it = availableFeats.begin();
    while (it != availableFeats.end())
    {
        (*it).AddImage(&m_imagesFeats[comboIndex]);
        ++it;
    }
    m_comboFeatSelect[comboIndex].SetImageList(&m_imagesFeats[comboIndex]);

    // now add the feat names to the combo control
    int sel = CB_ERR;
    it = availableFeats.begin();
    size_t featIndex = 0;
    while (it != availableFeats.end())
    {
        char buffer[_MAX_PATH];
        strcpy_s(buffer, (*it).Name().c_str());
        size_t pos = m_comboFeatSelect[comboIndex].AddString((*it).Name().c_str());
        m_comboFeatSelect[comboIndex].SetItemData(pos, featIndex);
        if (selection == (*it).Name())
        {
            sel = pos;
        }
        ++featIndex;
        ++it;
    }
    m_comboFeatSelect[comboIndex].SetCurSel(sel);
    m_comboFeatSelect[comboIndex].SetDroppedWidth(350); // wider to show extra text
    m_comboFeatSelect[comboIndex].UnlockWindowUpdate();
}

void CLevelUpView::PopulateAutomaticFeats()
{
    m_listAutomaticFeats.LockWindowUpdate();
    m_listAutomaticFeats.DeleteAllItems();
    if (m_pDocument != NULL)
    {
        // get the list of automatic feats gained at this level
        const LevelTraining & levelData = m_pCharacter->LevelData(m_level);
        std::list<TrainedFeat> automaticFeats = levelData.AutomaticFeats().Feats();
        m_imagesAutomaticFeats.DeleteImageList();
        m_imagesAutomaticFeats.Create(32, 32, ILC_COLOR32, 0, automaticFeats.size());

        // build an image list with all the feat icons
        std::list<TrainedFeat>::const_iterator it = automaticFeats.begin();
        while (it != automaticFeats.end())
        {
            // we only have the feat name, find the main feat and use it to get the icon
            const std::string & featName = (*it).FeatName();
            const Feat & feat = FindFeat(featName);
            feat.AddImage(&m_imagesAutomaticFeats);
            ++it;
        }
        m_listAutomaticFeats.SetImageList(&m_imagesAutomaticFeats, LVSIL_SMALL);
        m_listAutomaticFeats.SetImageList(&m_imagesAutomaticFeats, LVSIL_NORMAL);

        // now populate the list control
        size_t imageIndex = 0;
        it = automaticFeats.begin();
        while (it != automaticFeats.end())
        {
            const std::string & featName = (*it).FeatName();
            m_listAutomaticFeats.InsertItem(imageIndex, featName.c_str(), imageIndex);
            ++imageIndex;
            ++it;
        }
        if (imageIndex > 0)
        {
            // start with an item selected in the list so that tooltip work
            m_listAutomaticFeats.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            m_listAutomaticFeats.SetSelectionMark(0);
        }
    }
    m_listAutomaticFeats.UnlockWindowUpdate();
}

void CLevelUpView::PopulateGrantedFeats()
{
    m_listGrantedFeats.LockWindowUpdate();
    m_listGrantedFeats.DeleteAllItems();
    if (m_pDocument != NULL)
    {
        // get the list of granted feats
        std::list<TrainedFeat> grantedFeats = m_pCharacter->GrantedFeats();
        grantedFeats.sort();
        m_imagesGrantedFeats.DeleteImageList();
        m_imagesGrantedFeats.Create(32, 32, ILC_COLOR32, 0, grantedFeats.size());

        // build an image list with all the feat icons
        std::list<TrainedFeat>::const_iterator it = grantedFeats.begin();
        while (it != grantedFeats.end())
        {
            // we only have the feat name, find the main feat and use it to get the icon
            const std::string & featName = (*it).FeatName();
            const Feat & feat = FindFeat(featName);
            feat.AddImage(&m_imagesGrantedFeats);
            ++it;
        }
        m_listGrantedFeats.SetImageList(&m_imagesGrantedFeats, LVSIL_SMALL);
        m_listGrantedFeats.SetImageList(&m_imagesGrantedFeats, LVSIL_NORMAL);

        // now populate the list control
        size_t imageIndex = 0;
        it = grantedFeats.begin();
        while (it != grantedFeats.end())
        {
            const std::string & featName = (*it).FeatName();
            m_listGrantedFeats.InsertItem(imageIndex, featName.c_str(), imageIndex);
            ++imageIndex;
            ++it;
        }
        if (imageIndex > 0)
        {
            // start with an item selected in the list so that tooltip work
            m_listGrantedFeats.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            m_listGrantedFeats.SetSelectionMark(0);
        }
    }
    m_listGrantedFeats.UnlockWindowUpdate();
}

void CLevelUpView::ShowBab()
{
    CString babText("BAB 0");
    if (m_pCharacter != NULL)
    {
        size_t bab = m_pCharacter->BaseAttackBonus(m_level);
        babText.Format("BAB %d", bab);
    }
    m_staticBab.SetWindowText(babText);
}

void CLevelUpView::OnFeatSelection(UINT nID)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    // the user has selected a feat in one of the three feat combo boxes
    size_t typeIndex = nID - IDC_COMBO_FEATSELECT1;
    int sel = m_comboFeatSelect[typeIndex].GetCurSel();
    if (sel != CB_ERR)
    {
        // user has selected a feat, train it!
        CString featName;
        m_comboFeatSelect[typeIndex].GetLBText(sel, featName);
        Feat feat = FindFeat((LPCTSTR)featName);
        if (m_pCharacter->ShowUnavailable())
        {
            // need to evaluate the feat properly
            m_pCharacter->ToggleShowUnavailable();
            if (m_pCharacter->IsFeatTrainable(m_level, m_trainable[typeIndex], feat, true))
            {
                m_pCharacter->TrainFeat(
                        (LPCTSTR)featName,
                        m_trainable[typeIndex],
                        m_level);
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
                    m_trainable[typeIndex],
                    m_level);
        }
    }
}

void CLevelUpView::OnFeatSelectionCancel(UINT nID)
{
    // hide any tooltip being shown
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    m_bIgnoreNextMessage = true;
}

void CLevelUpView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_pCharacter != NULL)
    {
        // determine which level button (if any) has been clicked on.
        CWnd * pWnd = ChildWindowFromPoint(point);
        CLevelButton * pLevelButton = dynamic_cast<CLevelButton*>(pWnd);
        if (pLevelButton != NULL)
        {
            // yes, its a level button
            if (pLevelButton->IsWindowEnabled())
            {
                // and it is enabled
                OnButtonLevel(pLevelButton->GetDlgCtrlID());
            }
        }
    }
}

void CLevelUpView::OnButtonLevel(UINT nID)
{
    size_t level = nID - IDC_LEVEL1;
    if (level != m_level)
    {
        // user switching which level they are viewing
        m_level = level;
        PopulateControls();
    }
    else if (level < MAX_CLASS_LEVELS) // can only set for heroic levels
    {
        // if they click the same button again, show a pop up menu
        // to allow them to select a class to train
        CMenu cMenu;
        cMenu.CreatePopupMenu();
        ClassType type1 = m_pCharacter->Class1();
        ClassType type2 = m_pCharacter->Class2();
        ClassType type3 = m_pCharacter->Class3();
        // can always select unknown
        // ID_CLASS_UNKNOWN must have Class_Count reserved entries after it
        // To get icons, a toolbar that maps these to class images must exist
        // the icons must match the order of the ClassType enum
        CString title;
        title.Format("Select Class for Level %d", level + 1);
        cMenu.AppendMenu(
                MF_STRING | MF_GRAYED,
                0,
                title);
        cMenu.AppendMenu(MF_SEPARATOR);
        cMenu.AppendMenu(
                MF_STRING,
                ID_CLASS_UNKNOWN + Class_Unknown,
                (LPCTSTR)EnumEntryText(Class_Unknown, classTypeMap));
        if (type1 != Class_Unknown)
        {
            cMenu.AppendMenu(
                    MF_STRING,
                    ID_CLASS_UNKNOWN + type1,
                    (LPCTSTR)EnumEntryText(type1, classTypeMap));
        }
        if (type2 != Class_Unknown)
        {
            cMenu.AppendMenu(
                    MF_STRING,
                    ID_CLASS_UNKNOWN + type2,
                    (LPCTSTR)EnumEntryText(type2, classTypeMap));
        }
        if (type3 != Class_Unknown)
        {
            cMenu.AppendMenu(
                    MF_STRING,
                    ID_CLASS_UNKNOWN + type3,
                    (LPCTSTR)EnumEntryText(type3, classTypeMap));
        }
        CPoint p;
        GetCursorPos(&p);
        CWinAppEx * pApp = dynamic_cast<CWinAppEx*>(AfxGetApp());
        UINT sel = pApp->GetContextMenuManager()->TrackPopupMenu(
                cMenu.GetSafeHmenu(),
                p.x,
                p.y,
                this);
        if (sel >= ID_CLASS_UNKNOWN && sel <= ID_CLASS_UNKNOWN + Class_Count)
        {
            // a valid selection has been made, convert back class ClassType enum
            ClassType type = (ClassType)(sel - ID_CLASS_UNKNOWN);
            m_pCharacter->SetClass(m_level, type);
        }
        cMenu.DestroyMenu();
    }
}

BOOL CLevelUpView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_LEVEL1,
        IDC_LEVEL2,
        IDC_LEVEL3,
        IDC_LEVEL4,
        IDC_LEVEL5,
        IDC_LEVEL6,
        IDC_LEVEL7,
        IDC_LEVEL8,
        IDC_LEVEL9,
        IDC_LEVEL10,
        IDC_LEVEL11,
        IDC_LEVEL12,
        IDC_LEVEL13,
        IDC_LEVEL14,
        IDC_LEVEL15,
        IDC_LEVEL16,
        IDC_LEVEL17,
        IDC_LEVEL18,
        IDC_LEVEL19,
        IDC_LEVEL20,
        IDC_LEVEL21,
        IDC_LEVEL22,
        IDC_LEVEL23,
        IDC_LEVEL24,
        IDC_LEVEL25,
        IDC_LEVEL26,
        IDC_LEVEL27,
        IDC_LEVEL28,
        IDC_LEVEL29,
        IDC_LEVEL30,
        IDC_COMBO_CLASS1,
        IDC_COMBO_CLASS2,
        IDC_COMBO_CLASS3,
        IDC_LIST_SKILLS,
        IDC_COMBO_SKILLTOME,
        IDC_EDIT_SKILLPOINTS,
        IDC_BUTTON_SKILL_PLUS,
        IDC_BUTTON_SKILL_MINUS,
        IDC_FEAT_TYPE1,
        IDC_FEAT_TYPE2,
        IDC_FEAT_TYPE3,
        IDC_COMBO_FEATSELECT1,
        IDC_COMBO_FEATSELECT2,
        IDC_COMBO_FEATSELECT3,
        IDC_LIST_AUTOFEATS,
        IDC_LIST_GRANTEDFEATS,
        IDC_STATIC_CLASS,
        IDC_STATIC_SP_AVAILABLE,
        IDC_BUTTON_SKILLS,
        IDC_STATIC_BAB,
        IDC_STATIC_STR,
        IDC_STATIC_DEX,
        IDC_STATIC_CON,
        IDC_STATIC_INT,
        IDC_STATIC_WIS,
        IDC_STATIC_CHA,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CLevelUpView::OnCustomDrawSkills(NMHDR* pNMHDR, LRESULT* pResult)
{
    // colour the skill total column red if it has an overspend present
    NMLVCUSTOMDRAW *pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;          // assume normal windows drawn control
    if (m_pDocument != NULL)
    {
        if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
        {
            // we need notifies for each item
            *pResult = CDRF_NOTIFYITEMDRAW;
        }
        else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
        {
            // determine if a skill overspend is present
            int item = pLVCD->nmcd.dwItemSpec;
            SkillType skill = (SkillType)m_listSkills.GetItemData(item);
            double skillValue = m_pCharacter->SkillAtLevel(
                    skill,
                    m_level,
                    false);     // skill tome not included
            // work out how high the skill can go for this level
            double maxSkill = m_pCharacter->MaxSkillForLevel(
                    skill,
                    m_level);
            if (skillValue > maxSkill)
            {
                // overspend on skill, change the background colour
                pLVCD->clrTextBk = f_skillOverspendColour;
            }
            else
            {
                // no skill overspend present
                // draw it as normal
                *pResult = CDRF_DODEFAULT; // assume normal windows drawn control
            }
        }
    }
}

HBRUSH CLevelUpView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
    // colour the control based on whether the user has over spent
    // the number of skill points available at this level. This can happen
    // if they adjust the number of points spent in intelligence/tome value or change
    // classes or race.
    bool setWarning = false;
    if (pWnd == &m_editSkillPoints)
    {
        if (m_pDocument != NULL)
        {
            const LevelTraining & level = m_pCharacter->LevelData(m_level);
            int available = level.SkillPointsAvailable();
            int spent = level.SkillPointsSpent();
            setWarning = (spent > available);
        }
    }
    if (setWarning)
    {
        pDC->SetTextColor(f_skillOverspendColour);
    }

    return hbr;
}

LRESULT CLevelUpView::OnUpdateComplete(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    PopulateControls();
    m_canPostMessage = true;
    return 0L;
}

LRESULT CLevelUpView::OnMouseEnter(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        // is it an enter over a feat combo box?
        bool done = false;
        for (size_t i = 0; i < 3; ++i)
        {
            if (wParam == m_hookFeatHandles[i]
                    && m_staticFeatDescription[i].IsWindowVisible())
            {
                CRect itemRect;
                m_comboFeatSelect[i].GetWindowRect(&itemRect);
                ShowFeatTip(i, itemRect);
                done = true;
                break;
            }
        }
        if (!done)
        {
            for (size_t i = 0; i < m_pCharacter->MaxLevel(); ++i)
            {
                if (wParam == m_hookLevelHandles[i]
                        && m_buttonLevels[i].IsWindowVisible())
                {
                    CRect itemRect;
                    m_buttonLevels[i].GetWindowRect(&itemRect);
                    ShowLevelTip(i, itemRect);
                    done = true;
                    break;
                }
            }
        }
    }
    return 0;
}

LRESULT CLevelUpView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    if (wParam == m_automaticHandle)
    {
        // no longer monitoring this location
        GetMouseHook()->DeleteRectangleToMonitor(m_automaticHandle);
        m_automaticHandle = 0;
        m_hoverItem = -1;
    }
    return 0;
}

LRESULT CLevelUpView::OnHoverComboBox(WPARAM wParam, LPARAM lParam)
{
    if (!m_bIgnoreNextMessage)
    {
        // wParam = selected index
        // lParam = control ID
        UINT id = lParam - IDC_COMBO_FEATSELECT1;   // 0..1..2 now
        if (m_showingTip)
        {
            m_tooltip.Hide();
        }
        if (wParam >= 0)
        {
            // we have a selection, get the feats name
            CString featName;
            m_comboFeatSelect[id].GetLBText(wParam, featName);
            if (!featName.IsEmpty())
            {
                CRect rctWindow;
                m_comboFeatSelect[id].GetWindowRect(&rctWindow);
                rctWindow.right = rctWindow.left + m_comboFeatSelect[id].GetDroppedWidth();
                // tip is shown to the left or the right of the combo box
                CPoint tipTopLeft(rctWindow.left, rctWindow.top);
                CPoint tipAlternate(rctWindow.right, rctWindow.top);
                SetFeatTooltipText(
                        featName,
                        tipTopLeft,
                        tipAlternate,
                        true,               // right align
                        m_trainable[id],
                        m_pCharacter->IsFeatTrained((LPCTSTR)featName));
                m_showingTip = true;
            }
        }
    }
    m_bIgnoreNextMessage = false;
    return 0;
}

void CLevelUpView::ShowFeatTip(size_t featIndex, CRect itemRect)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    int sel = m_comboFeatSelect[featIndex].GetCurSel();
    if (sel != CB_ERR)
    {
        // we have a selection, get the feats name
        CString featName;
        m_comboFeatSelect[featIndex].GetLBText(sel, featName);
        if (!featName.IsEmpty())
        {
            CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
            CPoint tipAlternate(itemRect.left, itemRect.top - 2);
            SetFeatTooltipText(
                    featName,
                    tipTopLeft,
                    tipAlternate,
                    false,              // left align
                    m_trainable[featIndex],
                    true);              // is already trained
            m_showingTip = true;
        }
    }
}

void CLevelUpView::ShowLevelTip(size_t level, CRect itemRect)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    if (m_pCharacter != NULL)
    {
        CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
        CPoint tipAlternate(itemRect.left, itemRect.top - 2);
        SetLevelTooltipText(level, tipTopLeft, tipAlternate);
        m_showingTip = true;
    }
}

void CLevelUpView::HideTip()
{
    // tip not shown if not over an assay
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
    }
}

void CLevelUpView::SetFeatTooltipText(
        const CString & featName,
        CPoint tipTopLeft,
        CPoint tipAlternate,
        bool rightAlign,
        TrainableFeatTypes type,
        bool alreadyTrained)
{
    // look up the selected feat for this control
    const Feat & feat = FindFeat((LPCTSTR)featName);
    // determine whether there is a feat swap warning if training at level 1
    bool warn = false;
    if (type != TFT_Automatic
            && type != TFT_GrantedFeat
            && featName != " No Selection")
    {
        warn = !m_pCharacter->IsFeatTrainable(m_level, type, feat, (m_level != 0), alreadyTrained)
                || (featName == "Completionist" && (m_level == 0));
    }
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, rightAlign);
    m_tooltip.SetFeatItem(*m_pCharacter, &feat, warn, m_level, type == TFT_GrantedFeat);
    m_tooltip.Show();
}

void CLevelUpView::SetLevelTooltipText(
        size_t level,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    // look up the selected class
    const LevelTraining & levelData = m_pCharacter->LevelData(level);
    ClassType expectedClass = levelData.HasClass() ? levelData.Class() : Class_Unknown;
    if (level == 0)
    {
        switch (m_pCharacter->Race())
        {
        case Race_AasimarScourge: expectedClass = Class_Ranger; break;
        case Race_BladeForged: expectedClass = Class_Paladin; break;
        case Race_DeepGnome: expectedClass = Class_Wizard; break;
        case Race_EladrinChaosmancer: expectedClass = Class_SorcererWildMage; break;
        case Race_KoboldShamen: expectedClass = Class_Sorcerer; break;
        case Race_Morninglord: expectedClass = Class_Cleric; break;
        case Race_PurpleDragonKnight: expectedClass = Class_Fighter; break;
        case Race_RazorclawShifter: expectedClass = Class_Barbarian; break;
        case Race_ShadarKai: expectedClass = Class_Rogue; break;
        case Race_TabaxiTrailblazer: expectedClass = Class_Monk; break;
        case Race_TieflingScoundrel: expectedClass = Class_Bard; break;
        }
    }
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetLevelItem(*m_pCharacter, level, &levelData, expectedClass);
    m_tooltip.Show();
}

void CLevelUpView::SetAbilitiesAtLevel()
{
    if (m_pCharacter != NULL)
    {
        int str = m_pCharacter->AbilityAtLevel(Ability_Strength, m_level, true);
        int dex = m_pCharacter->AbilityAtLevel(Ability_Dexterity, m_level, true);
        int con = m_pCharacter->AbilityAtLevel(Ability_Constitution, m_level, true);
        int intelligence = m_pCharacter->AbilityAtLevel(Ability_Intelligence, m_level, true);
        int wis = m_pCharacter->AbilityAtLevel(Ability_Wisdom, m_level, true);
        int cha = m_pCharacter->AbilityAtLevel(Ability_Charisma, m_level, true);
        CString text;
        text.Format("STR %2d", str);
        m_abilitiesAtLevel[0].SetWindowText(text);
        text.Format("DEX %2d", dex);
        m_abilitiesAtLevel[1].SetWindowText(text);
        text.Format("CON %2d", con);
        m_abilitiesAtLevel[2].SetWindowText(text);
        text.Format("INT %2d", intelligence);
        m_abilitiesAtLevel[3].SetWindowText(text);
        text.Format("WIS %2d", wis);
        m_abilitiesAtLevel[4].SetWindowText(text);
        text.Format("CHA %2d", cha);
        m_abilitiesAtLevel[5].SetWindowText(text);
    }
    else
    {
        m_abilitiesAtLevel[0].SetWindowText("STR");
        m_abilitiesAtLevel[1].SetWindowText("DEX");
        m_abilitiesAtLevel[2].SetWindowText("CON");
        m_abilitiesAtLevel[3].SetWindowText("INT");
        m_abilitiesAtLevel[4].SetWindowText("WIS");
        m_abilitiesAtLevel[5].SetWindowText("CHA");
    }
}

LRESULT CLevelUpView::OnToggleFeatIgnore(WPARAM wParam, LPARAM lParam)
{
    // wParam = index of clicked item
    // lParam = (CString*)name of feat
    int selection = static_cast<int>(wParam);
    CString* pFeatName = static_cast<CString*>((void*)lParam);
    std::string featName = (LPCTSTR)(*pFeatName);
    if (!m_pCharacter->ShowIgnoredItems())
    {
        CWnd * pWnd = GetFocus();
        if (pWnd == &m_comboFeatSelect[0])
        {
            m_comboFeatSelect[0].DeleteString(wParam);
        }
        if (pWnd == &m_comboFeatSelect[1])
        {
            m_comboFeatSelect[1].DeleteString(wParam);
        }
        if (pWnd == &m_comboFeatSelect[2])
        {
            m_comboFeatSelect[2].DeleteString(wParam);
        }
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
