// DDOCPView.cpp
//
#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DDOCP.h"
#endif

#include "DDOCPDoc.h"
#include "DDOCPView.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"
#include "ActiveStanceDialog.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDDOCPView
namespace
{
    const int c_controlSpacing = 3;
    const size_t c_maxTomeValue = 7;    // max tome is +7 at level 27
    COLORREF f_abilityOverspendColour = RGB(0xE9, 0x96, 0x7A); // dark salmon
}

IMPLEMENT_DYNCREATE(CDDOCPView, CFormView)

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CDDOCPView, CFormView)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_BN_CLICKED(IDC_BUTTON_STR_PLUS, OnButtonStrPlus)
    ON_BN_CLICKED(IDC_BUTTON_STR_MINUS, OnButtonStrMinus)
    ON_BN_CLICKED(IDC_BUTTON_DEX_PLUS, OnButtonDexPlus)
    ON_BN_CLICKED(IDC_BUTTON_DEX_MINUS, OnButtonDexMinus)
    ON_BN_CLICKED(IDC_BUTTON_CON_PLUS, OnButtonConPlus)
    ON_BN_CLICKED(IDC_BUTTON_CON_MINUS, OnButtonConMinus)
    ON_BN_CLICKED(IDC_BUTTON_INT_PLUS, OnButtonIntPlus)
    ON_BN_CLICKED(IDC_BUTTON_INT_MINUS, OnButtonIntMinus)
    ON_BN_CLICKED(IDC_BUTTON_WIS_PLUS, OnButtonWisPlus)
    ON_BN_CLICKED(IDC_BUTTON_WIS_MINUS, OnButtonWisMinus)
    ON_BN_CLICKED(IDC_BUTTON_CHA_PLUS, OnButtonChaPlus)
    ON_BN_CLICKED(IDC_BUTTON_CHA_MINUS, OnButtonChaMinus)
    ON_CBN_SELENDOK(IDC_COMBO_RACE, &CDDOCPView::OnSelendokComboRace)
    ON_CBN_SELENDOK(IDC_COMBO_ALIGNMENT, &CDDOCPView::OnSelendokComboAlignment)
    ON_CBN_SELENDOK(IDC_COMBO_TOME_STR, OnSelendokComboTomeStr)
    ON_CBN_SELENDOK(IDC_COMBO_TOME_DEX, OnSelendokComboTomeDex)
    ON_CBN_SELENDOK(IDC_COMBO_TOME_CON, OnSelendokComboTomeCon)
    ON_CBN_SELENDOK(IDC_COMBO_TOME_INT, OnSelendokComboTomeInt)
    ON_CBN_SELENDOK(IDC_COMBO_TOME_WIS, OnSelendokComboTomeWis)
    ON_CBN_SELENDOK(IDC_COMBO_TOME_CHA, OnSelendokComboTomeCha)
    ON_EN_CHANGE(IDC_NAME, &CDDOCPView::OnChangeName)
    ON_CBN_SELENDOK(IDC_COMBO_LEVEL4_ABILITY, OnSelendokComboAbilityLevel4)
    ON_CBN_SELENDOK(IDC_COMBO_LEVEL8_ABILITY, OnSelendokComboAbilityLevel8)
    ON_CBN_SELENDOK(IDC_COMBO_LEVEL12_ABILITY, OnSelendokComboAbilityLevel12)
    ON_CBN_SELENDOK(IDC_COMBO_LEVEL16_ABILITY, OnSelendokComboAbilityLevel16)
    ON_CBN_SELENDOK(IDC_COMBO_LEVEL20_ABILITY, OnSelendokComboAbilityLevel20)
    ON_CBN_SELENDOK(IDC_COMBO_LEVEL24_ABILITY, OnSelendokComboAbilityLevel24)
    ON_CBN_SELENDOK(IDC_COMBO_LEVEL28_ABILITY, OnSelendokComboAbilityLevel28)
    ON_WM_SIZE()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_CHECK_GUILD_BUFFS, OnButtonGuildBuffs)
    ON_EN_KILLFOCUS(IDC_EDIT_GUILD_LEVEL, OnKillFocusGuildLevel)
END_MESSAGE_MAP()
#pragma warning(pop)

// CDDOCPView construction/destruction
CDDOCPView::CDDOCPView() :
    CFormView(CDDOCPView::IDD),
    m_pCharacter(NULL)
{
    // TODO: add construction code here
}

CDDOCPView::~CDDOCPView()
{
}

void CDDOCPView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_BUILD, m_staticBuildDescription);
    DDX_Control(pDX, IDC_NAME, m_editName);
    DDX_Control(pDX, IDC_COMBO_RACE, m_comboRace);
    DDX_Control(pDX, IDC_COMBO_ALIGNMENT, m_comboAlignment);
    DDX_Control(pDX, IDC_EDIT_STR, m_editStr);
    DDX_Control(pDX, IDC_EDIT_DEX, m_editDex);
    DDX_Control(pDX, IDC_EDIT_CON, m_editCon);
    DDX_Control(pDX, IDC_EDIT_INT, m_editInt);
    DDX_Control(pDX, IDC_EDIT_WIS, m_editWis);
    DDX_Control(pDX, IDC_EDIT_CHA, m_editCha);
    DDX_Control(pDX, IDC_EDIT_STR_COST, m_editCostStr);
    DDX_Control(pDX, IDC_EDIT_DEX_COST, m_editCostDex);
    DDX_Control(pDX, IDC_EDIT_CON_COST, m_editCostCon);
    DDX_Control(pDX, IDC_EDIT_INT_COST, m_editCostInt);
    DDX_Control(pDX, IDC_EDIT_WIS_COST, m_editCostWis);
    DDX_Control(pDX, IDC_EDIT_CHA_COST, m_editCostCha);
    DDX_Control(pDX, IDC_COMBO_TOME_STR, m_comboTomeStr);
    DDX_Control(pDX, IDC_COMBO_TOME_DEX, m_comboTomeDex);
    DDX_Control(pDX, IDC_COMBO_TOME_CON, m_comboTomeCon);
    DDX_Control(pDX, IDC_COMBO_TOME_INT, m_comboTomeInt);
    DDX_Control(pDX, IDC_COMBO_TOME_WIS, m_comboTomeWis);
    DDX_Control(pDX, IDC_COMBO_TOME_CHA, m_comboTomeCha);
    DDX_Control(pDX, IDC_BUTTON_STR_PLUS, m_buttonStrPlus);
    DDX_Control(pDX, IDC_BUTTON_STR_MINUS, m_buttonStrMinus);
    DDX_Control(pDX, IDC_BUTTON_DEX_PLUS, m_buttonDexPlus);
    DDX_Control(pDX, IDC_BUTTON_DEX_MINUS, m_buttonDexMinus);
    DDX_Control(pDX, IDC_BUTTON_CON_PLUS, m_buttonConPlus);
    DDX_Control(pDX, IDC_BUTTON_CON_MINUS, m_buttonConMinus);
    DDX_Control(pDX, IDC_BUTTON_INT_PLUS, m_buttonIntPlus);
    DDX_Control(pDX, IDC_BUTTON_INT_MINUS, m_buttonIntMinus);
    DDX_Control(pDX, IDC_BUTTON_WIS_PLUS, m_buttonWisPlus);
    DDX_Control(pDX, IDC_BUTTON_WIS_MINUS, m_buttonWisMinus);
    DDX_Control(pDX, IDC_BUTTON_CHA_PLUS, m_buttonChaPlus);
    DDX_Control(pDX, IDC_BUTTON_CHA_MINUS, m_buttonChaMinus);
    DDX_Control(pDX, IDC_CHECK_GUILD_BUFFS, m_buttonGuildBuffs);
    DDX_Control(pDX, IDC_EDIT_GUILD_LEVEL, m_editGuildLevel);
    DDX_Control(pDX, IDC_STATIC_AVAILABLE_POINTS, m_staticAvailableSpend);
    DDX_Control(pDX, IDC_COMBO_LEVEL4_ABILITY, m_comboAILevel4);
    DDX_Control(pDX, IDC_COMBO_LEVEL8_ABILITY, m_comboAILevel8);
    DDX_Control(pDX, IDC_COMBO_LEVEL12_ABILITY, m_comboAILevel12);
    DDX_Control(pDX, IDC_COMBO_LEVEL16_ABILITY, m_comboAILevel16);
    DDX_Control(pDX, IDC_COMBO_LEVEL20_ABILITY, m_comboAILevel20);
    DDX_Control(pDX, IDC_COMBO_LEVEL24_ABILITY, m_comboAILevel24);
    DDX_Control(pDX, IDC_COMBO_LEVEL28_ABILITY, m_comboAILevel28);
}

void CDDOCPView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    m_pCharacter = GetDocument()->GetCharacter();
    m_pCharacter->AttachObserver(this);

    PopulateComboboxes();
    RestoreControls();      // puts controls to values of loaded data
    EnableButtons();
    UpdateBuildDescription();

    GetParentFrame()->RecalcLayout();
    ResizeParentToFit();
}

void CDDOCPView::OnRButtonUp(UINT nFlags, CPoint point)
{
    UNREFERENCED_PARAMETER(nFlags);
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void CDDOCPView::OnContextMenu(CWnd * pWnd, CPoint point)
{
    UNREFERENCED_PARAMETER(pWnd);
    UNREFERENCED_PARAMETER(point);
#ifndef SHARED_HANDLERS
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CDDOCPView diagnostics

#ifdef _DEBUG
void CDDOCPView::AssertValid() const
{
    CFormView::AssertValid();
}

void CDDOCPView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}

CDDOCPDoc* CDDOCPView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDDOCPDoc)));
    return (CDDOCPDoc*)m_pDocument;
}
#endif //_DEBUG

// CDDOCPView message handlers
void CDDOCPView::PopulateComboboxes()
{
    // setup the combo-boxes to have selectable content

    // Available races
    for (size_t ri = Race_Unknown + 1; ri < Race_Count; ++ri)
    {
        CString text = EnumEntryText(
                (RaceType)ri,
                raceTypeMap);
        size_t i = m_comboRace.AddString(text);
        m_comboRace.SetItemData(i, ri);
    }

    // Available alignments
    for (size_t ai = Alignment_Unknown + 1; ai < Alignment_Count; ++ai)
    {
        CString text = EnumEntryText(
                (AlignmentType)ai,
                alignmentTypeMap);
        size_t i = m_comboAlignment.AddString(text);
        m_comboAlignment.SetItemData(i, ai);
    }

    // ability tomes
    for (size_t ti = 0; ti <= c_maxTomeValue; ++ti)
    {
        CString text;
        if (ti > 0)
        {
            text.Format("%+d Tome", ti);
        }
        else
        {
            text = "No Tome";
        }
        // add to all 6 ability tome combo-boxes
        size_t i = m_comboTomeStr.InsertString(ti, text);
        m_comboTomeStr.SetItemData(i, ti);
        i = m_comboTomeDex.AddString(text);
        m_comboTomeDex.SetItemData(i, ti);
        i = m_comboTomeCon.AddString(text);
        m_comboTomeCon.SetItemData(i, ti);
        i = m_comboTomeInt.AddString(text);
        m_comboTomeInt.SetItemData(i, ti);
        i = m_comboTomeWis.AddString(text);
        m_comboTomeWis.SetItemData(i, ti);
        i = m_comboTomeCha.AddString(text);
        m_comboTomeCha.SetItemData(i, ti);
    }

    // level up stats
    for (size_t ai = Ability_Unknown + 1; ai < Ability_Count; ++ai)
    {
        CString text = EnumEntryText(
                (AbilityType)ai,
                abilityTypeMap);
        size_t i = m_comboAILevel4.AddString(text);
        m_comboAILevel4.SetItemData(i, ai);
        i = m_comboAILevel8.AddString(text);
        m_comboAILevel8.SetItemData(i, ai);
        i = m_comboAILevel12.AddString(text);
        m_comboAILevel12.SetItemData(i, ai);
        i = m_comboAILevel16.AddString(text);
        m_comboAILevel16.SetItemData(i, ai);
        i = m_comboAILevel20.AddString(text);
        m_comboAILevel20.SetItemData(i, ai);
        i = m_comboAILevel24.AddString(text);
        m_comboAILevel24.SetItemData(i, ai);
        i = m_comboAILevel28.AddString(text);
        m_comboAILevel28.SetItemData(i, ai);
    }
}

void CDDOCPView::RestoreControls()
{
    // restore loaded values
    // name
    m_editName.SetWindowText(m_pCharacter->Name().c_str());

    // Race
    SelectComboboxEntry(m_pCharacter->Race(), &m_comboRace);

    // alignment
    SelectComboboxEntry(m_pCharacter->Alignment(), &m_comboAlignment);

    // tomes
    SelectComboboxEntry(m_pCharacter->StrTome(), &m_comboTomeStr);
    SelectComboboxEntry(m_pCharacter->DexTome(), &m_comboTomeDex);
    SelectComboboxEntry(m_pCharacter->ConTome(), &m_comboTomeCon);
    SelectComboboxEntry(m_pCharacter->IntTome(), &m_comboTomeInt);
    SelectComboboxEntry(m_pCharacter->WisTome(), &m_comboTomeWis);
    SelectComboboxEntry(m_pCharacter->ChaTome(), &m_comboTomeCha);

    DisplayAbilityValue(Ability_Strength, &m_editStr);
    DisplayAbilityValue(Ability_Dexterity, &m_editDex);
    DisplayAbilityValue(Ability_Constitution, &m_editCon);
    DisplayAbilityValue(Ability_Intelligence, &m_editInt);
    DisplayAbilityValue(Ability_Wisdom, &m_editWis);
    DisplayAbilityValue(Ability_Charisma, &m_editCha);

    DisplaySpendCost(Ability_Strength, &m_editCostStr);
    DisplaySpendCost(Ability_Dexterity, &m_editCostDex);
    DisplaySpendCost(Ability_Constitution, &m_editCostCon);
    DisplaySpendCost(Ability_Intelligence, &m_editCostInt);
    DisplaySpendCost(Ability_Wisdom, &m_editCostWis);
    DisplaySpendCost(Ability_Charisma, &m_editCostCha);
    UpdateAvailableSpend();

    // ability increase selections
    SelectComboboxEntry(m_pCharacter->Level4(), &m_comboAILevel4);
    SelectComboboxEntry(m_pCharacter->Level8(), &m_comboAILevel8);
    SelectComboboxEntry(m_pCharacter->Level12(), &m_comboAILevel12);
    SelectComboboxEntry(m_pCharacter->Level16(), &m_comboAILevel16);
    SelectComboboxEntry(m_pCharacter->Level20(), &m_comboAILevel20);
    SelectComboboxEntry(m_pCharacter->Level24(), &m_comboAILevel24);
    SelectComboboxEntry(m_pCharacter->Level28(), &m_comboAILevel28);

    // guild level
    CString level;
    level.Format("%d", m_pCharacter->GuildLevel());
    m_editGuildLevel.SetWindowText(level);
    m_buttonGuildBuffs.SetCheck(m_pCharacter->HasApplyGuildBuffs() ? BST_CHECKED : BST_UNCHECKED);
}

void CDDOCPView::EnableButtons()
{
    // depending on the selected build points, enable/disable
    // all the ability increase/decrease buttons based on current spends
    const AbilitySpend & as = m_pCharacter->BuildPoints();

    m_buttonStrPlus.EnableWindow(as.CanSpendOnAbility(Ability_Strength));
    m_buttonStrMinus.EnableWindow(as.CanRevokeSpend(Ability_Strength));
    m_buttonDexPlus.EnableWindow(as.CanSpendOnAbility(Ability_Dexterity));
    m_buttonDexMinus.EnableWindow(as.CanRevokeSpend(Ability_Dexterity));
    m_buttonConPlus.EnableWindow(as.CanSpendOnAbility(Ability_Constitution));
    m_buttonConMinus.EnableWindow(as.CanRevokeSpend(Ability_Constitution));
    m_buttonIntPlus.EnableWindow(as.CanSpendOnAbility(Ability_Intelligence));
    m_buttonIntMinus.EnableWindow(as.CanRevokeSpend(Ability_Intelligence));
    m_buttonWisPlus.EnableWindow(as.CanSpendOnAbility(Ability_Wisdom));
    m_buttonWisMinus.EnableWindow(as.CanRevokeSpend(Ability_Wisdom));
    m_buttonChaPlus.EnableWindow(as.CanSpendOnAbility(Ability_Charisma));
    m_buttonChaMinus.EnableWindow(as.CanRevokeSpend(Ability_Charisma));
}

void CDDOCPView::DisplayAbilityValue(
        AbilityType ability,
        CEdit * control)
{
    size_t value = m_pCharacter->BaseAbilityValue(ability);
    CString text;
    text.Format("%d", value);
    control->SetWindowText(text);
}

void CDDOCPView::DisplaySpendCost(
        AbilityType ability,
        CEdit * control)
{
    const AbilitySpend & as = m_pCharacter->BuildPoints();
    size_t nextCost = as.NextPointsSpentCost(ability);
    CString text;
    if (nextCost > 0)
    {
        // show how many build points the next increase will cost
        text.Format("%d", nextCost);
    }
    else
    {
        // max spend on this ability has been reached
        text = "MAX";
    }
    control->SetWindowTextA(text);
}

void CDDOCPView::UpdateAvailableSpend()
{
    // show how many build points are yet to be spent
    const AbilitySpend & as = m_pCharacter->BuildPoints();
    int availablePoints = (int)as.AvailableSpend() - (int)as.PointsSpent();
    CString text;
    text.Format("Available Build points: %d", availablePoints);
    m_staticAvailableSpend.SetWindowText(text);
}

HBRUSH CDDOCPView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
    // colour the control based on whether the user has over spent
    // the number of build points available. This can happen
    // if they adjust down the number of past lives
    bool setWarning = false;
    if (pWnd == &m_staticAvailableSpend)
    {
        if (m_pDocument != NULL)
        {
            const AbilitySpend & as = m_pCharacter->BuildPoints();
            int availablePoints = (int)as.AvailableSpend() - (int)as.PointsSpent();
            setWarning = (availablePoints < 0);
        }
    }
    if (setWarning)
    {
        pDC->SetTextColor(f_abilityOverspendColour);
    }

    return hbr;
}

void CDDOCPView::OnButtonStrPlus()
{
    m_pCharacter->SpendOnAbility(Ability_Strength);
    EnableButtons();
    DisplayAbilityValue(Ability_Strength, &m_editStr);
    DisplaySpendCost(Ability_Strength, &m_editCostStr);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonStrMinus()
{
    m_pCharacter->RevokeSpendOnAbility(Ability_Strength);
    EnableButtons();
    DisplayAbilityValue(Ability_Strength, &m_editStr);
    DisplaySpendCost(Ability_Strength, &m_editCostStr);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonDexPlus()
{
    m_pCharacter->SpendOnAbility(Ability_Dexterity);
    EnableButtons();
    DisplayAbilityValue(Ability_Dexterity, &m_editDex);
    DisplaySpendCost(Ability_Dexterity, &m_editCostDex);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonDexMinus()
{
    m_pCharacter->RevokeSpendOnAbility(Ability_Dexterity);
    EnableButtons();
    DisplayAbilityValue(Ability_Dexterity, &m_editDex);
    DisplaySpendCost(Ability_Dexterity, &m_editCostDex);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonConPlus()
{
    m_pCharacter->SpendOnAbility(Ability_Constitution);
    EnableButtons();
    DisplayAbilityValue(Ability_Constitution, &m_editCon);
    DisplaySpendCost(Ability_Constitution, &m_editCostCon);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonConMinus()
{
    m_pCharacter->RevokeSpendOnAbility(Ability_Constitution);
    EnableButtons();
    DisplayAbilityValue(Ability_Constitution, &m_editCon);
    DisplaySpendCost(Ability_Constitution, &m_editCostCon);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonIntPlus()
{
    m_pCharacter->SpendOnAbility(Ability_Intelligence);
    EnableButtons();
    DisplayAbilityValue(Ability_Intelligence, &m_editInt);
    DisplaySpendCost(Ability_Intelligence, &m_editCostInt);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonIntMinus()
{
    m_pCharacter->RevokeSpendOnAbility(Ability_Intelligence);
    EnableButtons();
    DisplayAbilityValue(Ability_Intelligence, &m_editInt);
    DisplaySpendCost(Ability_Intelligence, &m_editCostInt);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonWisPlus()
{
    m_pCharacter->SpendOnAbility(Ability_Wisdom);
    EnableButtons();
    DisplayAbilityValue(Ability_Wisdom, &m_editWis);
    DisplaySpendCost(Ability_Wisdom, &m_editCostWis);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonWisMinus()
{
    m_pCharacter->RevokeSpendOnAbility(Ability_Wisdom);
    EnableButtons();
    DisplayAbilityValue(Ability_Wisdom, &m_editWis);
    DisplaySpendCost(Ability_Wisdom, &m_editCostWis);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonChaPlus()
{
    m_pCharacter->SpendOnAbility(Ability_Charisma);
    EnableButtons();
    DisplayAbilityValue(Ability_Charisma, &m_editCha);
    DisplaySpendCost(Ability_Charisma, &m_editCostCha);
    UpdateAvailableSpend();
}

void CDDOCPView::OnButtonChaMinus()
{
    m_pCharacter->RevokeSpendOnAbility(Ability_Charisma);
    EnableButtons();
    DisplayAbilityValue(Ability_Charisma, &m_editCha);
    DisplaySpendCost(Ability_Charisma, &m_editCostCha);
    UpdateAvailableSpend();
}


void CDDOCPView::OnSelendokComboRace()
{
    // race has been chosen
    RaceType race = static_cast<RaceType>(GetComboboxSelection(&m_comboRace));
    m_pCharacter->SetRace(race);

    // race may affect any ability stat value
    DisplayAbilityValue(Ability_Strength, &m_editStr);
    DisplayAbilityValue(Ability_Dexterity, &m_editDex);
    DisplayAbilityValue(Ability_Constitution, &m_editCon);
    DisplayAbilityValue(Ability_Intelligence, &m_editInt);
    DisplayAbilityValue(Ability_Wisdom, &m_editWis);
    DisplayAbilityValue(Ability_Charisma, &m_editCha);
}

void CDDOCPView::OnSelendokComboAlignment()
{
    AlignmentType alignment = static_cast<AlignmentType>(GetComboboxSelection(&m_comboAlignment));
    m_pCharacter->SetAlignment(alignment);
    // views affected by alignment changes update automatically
}

void CDDOCPView::OnSelendokComboTomeStr()
{
    size_t value = GetComboboxSelection(&m_comboTomeStr);
    m_pCharacter->SetAbilityTome(Ability_Strength, value);
}

void CDDOCPView::OnSelendokComboTomeDex()
{
    size_t value = GetComboboxSelection(&m_comboTomeDex);
    m_pCharacter->SetAbilityTome(Ability_Dexterity, value);
}

void CDDOCPView::OnSelendokComboTomeCon()
{
    size_t value = GetComboboxSelection(&m_comboTomeCon);
    m_pCharacter->SetAbilityTome(Ability_Constitution, value);
}

void CDDOCPView::OnSelendokComboTomeInt()
{
    size_t value = GetComboboxSelection(&m_comboTomeInt);
    m_pCharacter->SetAbilityTome(Ability_Intelligence, value);
}

void CDDOCPView::OnSelendokComboTomeWis()
{
    size_t value = GetComboboxSelection(&m_comboTomeWis);
    m_pCharacter->SetAbilityTome(Ability_Wisdom, value);
}

void CDDOCPView::OnSelendokComboTomeCha()
{
    size_t value = GetComboboxSelection(&m_comboTomeCha);
    m_pCharacter->SetAbilityTome(Ability_Charisma, value);
}

void CDDOCPView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
    if (bActivate == TRUE)
    {
        // notify the main frame that we are active so that it can keep all
        // the floating windows using the correct document also
        CWnd * pWnd = AfxGetMainWnd();
        CMainFrame * pMainFrame = dynamic_cast<CMainFrame*>(pWnd);
        if (pMainFrame != NULL)
        {
            pMainFrame->SetActiveDocumentAndCharacter(GetDocument(), m_pCharacter);
        }
    }

    CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CDDOCPView::OnChangeName()
{
    CString name;
    m_editName.GetWindowText(name);
    if (name != m_pCharacter->Name().c_str())
    {
        // name has changed
        m_pCharacter->SetCharacterName(name);
    }
}

void CDDOCPView::OnSelendokComboAbilityLevel4()
{
    AbilityType value = (AbilityType)GetComboboxSelection(&m_comboAILevel4);
    m_pCharacter->SetAbilityLevelUp(4, value);
}

void CDDOCPView::OnSelendokComboAbilityLevel8()
{
    AbilityType value = (AbilityType)GetComboboxSelection(&m_comboAILevel8);
    m_pCharacter->SetAbilityLevelUp(8, value);
}

void CDDOCPView::OnSelendokComboAbilityLevel12()
{
    AbilityType value = (AbilityType)GetComboboxSelection(&m_comboAILevel12);
    m_pCharacter->SetAbilityLevelUp(12, value);
}

void CDDOCPView::OnSelendokComboAbilityLevel16()
{
    AbilityType value = (AbilityType)GetComboboxSelection(&m_comboAILevel16);
    m_pCharacter->SetAbilityLevelUp(16, value);
}

void CDDOCPView::OnSelendokComboAbilityLevel20()
{
    AbilityType value = (AbilityType)GetComboboxSelection(&m_comboAILevel20);
    m_pCharacter->SetAbilityLevelUp(20, value);
}

void CDDOCPView::OnSelendokComboAbilityLevel24()
{
    AbilityType value = (AbilityType)GetComboboxSelection(&m_comboAILevel24);
    m_pCharacter->SetAbilityLevelUp(24, value);
}

void CDDOCPView::OnSelendokComboAbilityLevel28()
{
    AbilityType value = (AbilityType)GetComboboxSelection(&m_comboAILevel28);
    m_pCharacter->SetAbilityLevelUp(28, value);
}

void CDDOCPView::UpdateBuildDescription()
{
    CString text = m_pCharacter->GetBuildDescription().c_str();
    m_staticBuildDescription.SetWindowText(text);
}

// Character overrides
void CDDOCPView::UpdateAvailableBuildPointsChanged(Character * pCharacter)
{
    // show the now available build points
    UpdateAvailableSpend();
}

void CDDOCPView::UpdateClassChanged(
        Character * pCharacter,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    // level up view changed a class selection, keep our overall build description
    // up to date.
    UpdateBuildDescription();
}

void CDDOCPView::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);
}

void CDDOCPView::OnButtonGuildBuffs()
{
    // toggle the enabled guild buffs status
    m_pCharacter->ToggleApplyGuildBuffs();
}

void CDDOCPView::OnKillFocusGuildLevel()
{
    // change the guild level
    CString text;
    m_editGuildLevel.GetWindowText(text);
    size_t level = atoi(text);      // its ES_NUMBER so guaranteed to work
    m_pCharacter->SetGuildLevel(level);
}
