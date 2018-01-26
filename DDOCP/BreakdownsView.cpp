// BreakdownsView.cpp
//
#include "stdafx.h"
#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

#include "BreakdownItemAbility.h"
#include "BreakdownItemAssassinate.h"
#include "BreakdownItemBAB.h"
#include "BreakdownItemCasterLevel.h"
#include "BreakdownItemEnergyAbsorption.h"
#include "BreakdownItemEnergyResistance.h"
#include "BreakdownItemHitpoints.h"
#include "BreakdownItemMRR.h"
#include "BreakdownItemMRRCap.h"
#include "BreakdownItemPRR.h"
#include "BreakdownItemSave.h"
#include "BreakdownItemSimple.h"
#include "BreakdownItemSkill.h"
#include "BreakdownItemSpellPoints.h"
#include "BreakdownItemSpellPower.h"
#include "BreakdownItemSpellSchool.h"
#include "BreakdownItemTactical.h"
#include "BreakdownItemTurnUndeadLevel.h"
#include "BreakdownItemTurnUndeadHitDice.h"
#include "BreakdownItemWeaponEffects.h"
#include "BreakdownItemSneakAttackDice.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    const char * f_treeSizeKey = "BreakdownsView";
    const char * f_treeSizeEntry = "TreeSize";
}

IMPLEMENT_DYNCREATE(CBreakdownsView, CFormView)

CBreakdownsView::CBreakdownsView() :
    CFormView(CBreakdownsView::IDD),
    m_pCharacter(NULL),
    m_itemBreakdownTree(true, true),
    m_bDraggingDivider(false),
    m_treeSizePercent(75),      // defaults to 75% on first run of software
    m_pWeaponEffects(NULL),
    m_hWeaponsHeader(NULL)
{

}

CBreakdownsView::~CBreakdownsView()
{
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        delete m_items[i];
        m_items[i] = NULL;
    }
    m_items.clear();
    delete m_pWeaponEffects;
    m_pWeaponEffects = NULL;
}

void CBreakdownsView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ITEM_BREAKDOWN, m_itemBreakdownList);
    DDX_Control(pDX, IDC_DIVIDER, m_divider);
    DDX_Control(pDX, IDC_BUTTON_CLIPBOARD, m_buttonClipboard);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CBreakdownsView, CFormView)
    ON_WM_SIZE()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_NOTIFY(TVN_SELCHANGED, 1, OnSelChangedBreakdownTree)
    ON_WM_SETCURSOR()
    ON_BN_CLICKED(IDC_DIVIDER, OnDividerClicked)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_NOTIFY(HDN_ENDTRACK, IDC_ITEM_BREAKDOWN, OnEndtrackBreakdownList)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, IDC_ITEM_BREAKDOWN, OnEndtrackBreakdownList)
    ON_BN_CLICKED(IDC_BUTTON_CLIPBOARD, OnButtonClipboardCopy)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CBreakdownsView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBreakdownsView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

LRESULT CBreakdownsView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }
    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    if (pCharacter != m_pCharacter)
    {
        m_pCharacter = pCharacter;
        // let all the breakdown items know about the document change
        for (size_t ii = 0; ii < m_items.size(); ++ii)
        {
            m_items[ii]->SetCharacter(pCharacter, true);    // top level items always observe
        }
        m_pWeaponEffects->SetCharacter(pCharacter, true);
        if (pCharacter != NULL)
        {
            // need to know about gear changes
            pCharacter->AttachObserver(this);
        }
    }
    return 0L;
}

void CBreakdownsView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    m_treeSizePercent = AfxGetApp()->GetProfileInt(f_treeSizeKey, f_treeSizeEntry, 75);
    m_itemBreakdownTree.CreateEx(
            WS_EX_CLIENTEDGE,
            WC_TREEVIEW,
            "BreakdownItems",
            WS_VISIBLE | WS_CHILD | WS_TABSTOP,
            CRect(0,0,10,10),
            this,
            1);
    m_itemBreakdownTree.InsertColumn(0, "Item", LVCFMT_LEFT, 250);
    m_itemBreakdownTree.InsertColumn(1, "Value", LVCFMT_LEFT, 75);

    m_itemBreakdownList.InsertColumn(0, "Breakdown source", LVCFMT_LEFT, 220);
    m_itemBreakdownList.InsertColumn(1, "Stacks", LVCFMT_LEFT, 50);
    m_itemBreakdownList.InsertColumn(2, "Value", LVCFMT_LEFT, 50);
    m_itemBreakdownList.InsertColumn(3, "Bonus Type", LVCFMT_LEFT, 50);
    m_itemBreakdownList.SetExtendedStyle(m_itemBreakdownList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    LoadColumnWidthsByName(&m_itemBreakdownList, "BreakdownList_%s");
    CreateBreakdowns();
    // Image for copy to clipboard button
    m_buttonClipboard.SetImage(IDB_BITMAP_COPYTOCLIPBOARD);
}

void CBreakdownsView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (IsWindow(m_itemBreakdownList.GetSafeHwnd()))
    {
        // +---------------------------------+  ^
        // |                                 |  |
        // |    Breakdowns Tree List         |  | m_treeSizePercent as a percentage
        // |                                 |  |
        // |                                 |  |
        // |                                 |  |
        // |                                 |  |
        // +---------------------------------+  v
        // [-- -------Drag divider--------][c]
        // +---------------------------------+
        // |   Selected Item breakdown List  |
        // |                                 |
        // |                                 |
        // +---------------------------------+
        // control area configured by the m_treeSizePercent variable which is a percentage
        CRect rctDivider;
        CRect rctButtonClipboard;
        m_divider.GetWindowRect(rctDivider);
        m_buttonClipboard.GetWindowRect(rctButtonClipboard);
        // position the divider at its percentage location
        rctDivider -= rctDivider.TopLeft();
        rctDivider.left = c_controlSpacing;
        rctDivider.right = cx - c_controlSpacing - rctButtonClipboard.Width() - c_controlSpacing;
        int verticalSpace = cy - (c_controlSpacing * 4) - rctDivider.Height();
        rctDivider += CPoint (0, c_controlSpacing * 2 + (verticalSpace * m_treeSizePercent) / 100);
        rctButtonClipboard -= rctButtonClipboard.TopLeft();
        rctButtonClipboard += CPoint(cx - rctButtonClipboard.Width() - c_controlSpacing,c_controlSpacing * 2 + (verticalSpace * m_treeSizePercent) / 100);
        // all other items placed relative to the divider
        CRect rctTree(c_controlSpacing, c_controlSpacing, cx - c_controlSpacing, rctDivider.top - c_controlSpacing);
        CRect rctList(c_controlSpacing, rctDivider.bottom + c_controlSpacing, cx - c_controlSpacing, cy - c_controlSpacing);
        m_itemBreakdownTree.MoveWindow(rctTree);
        m_divider.MoveWindow(rctDivider);
        m_buttonClipboard.MoveWindow(rctButtonClipboard);
        m_itemBreakdownList.MoveWindow(rctList);
    }
}

void CBreakdownsView::CreateBreakdowns()
{
    CreateAbilityBreakdowns();
    CreateSavesBreakdowns();
    CreateSkillBreakdowns();
    CreatePhysicalBreakdowns();
    CreateMagicalBreakdowns();
    CreateTurnUndeadBreakdowns();
    CreateEnergyResistancesBreakdowns();
    CreateHirelingBreakdowns();
    CreateWeaponBreakdowns();
}

void CBreakdownsView::CreateAbilityBreakdowns()
{
    // insert the abilities root tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Abilities", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);
    for (size_t ai = Ability_Unknown + 1; ai < Ability_Count; ++ai)
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                EnumEntryText((AbilityType)ai, abilityTypeMap),
                hParent,
                TVI_LAST);
        BreakdownItem * pAbility = new BreakdownItemAbility(
                (AbilityType)ai,
                (BreakdownType)(Breakdown_Strength + ai - Ability_Unknown - 1),
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pAbility);
        m_items.push_back(pAbility);
    }
}

void CBreakdownsView::CreateSavesBreakdowns()
{
    // insert the Saves root tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Saving Throws", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Fortitude",
                hParent,
                TVI_LAST);
        BreakdownItem * pFS = new BreakdownItemSave(
                Breakdown_SaveFortitude,
                Save_Fortitude,
                &m_itemBreakdownTree,
                hItem,
                Ability_Constitution,
                NULL);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pFS);
        m_items.push_back(pFS);
        // some sub variations of fortitude for specific sub-save types
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Poison",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pPS = new BreakdownItemSave(
                    Breakdown_SavePoison,
                    Save_Poison,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveFortitude));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pPS);
            m_items.push_back(pPS);
        }
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Disease",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pPS = new BreakdownItemSave(
                    Breakdown_SaveDisease,
                    Save_Disease,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveFortitude));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pPS);
            m_items.push_back(pPS);
        }
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Reflex",
                hParent,
                TVI_LAST);
        BreakdownItem * pRS = new BreakdownItemSave(
                Breakdown_SaveReflex,
                Save_Reflex,
                &m_itemBreakdownTree,
                hItem,
                Ability_Dexterity,
                NULL);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pRS);
        m_items.push_back(pRS);
        // some sub variations of reflex for specific sub-save types
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Traps",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pTS = new BreakdownItemSave(
                    Breakdown_SaveTraps,
                    Save_Traps,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveReflex));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pTS);
            m_items.push_back(pTS);
        }
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Spell",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pSS = new BreakdownItemSave(
                    Breakdown_SaveSpell,
                    Save_Spell,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveReflex));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pSS);
            m_items.push_back(pSS);
        }
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Magic",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pSM = new BreakdownItemSave(
                    Breakdown_SaveMagic,
                    Save_Magic,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveReflex));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pSM);
            m_items.push_back(pSM);
        }
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Will",
                hParent,
                TVI_LAST);
        BreakdownItem * pWS = new BreakdownItemSave(
                Breakdown_SaveWill,
                Save_Will,
                &m_itemBreakdownTree,
                hItem,
                Ability_Wisdom,
                NULL);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pWS);
        m_items.push_back(pWS);
        // some sub variations of Will for specific sub-save types
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Enchantment",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pES = new BreakdownItemSave(
                    Breakdown_SaveEnchantment,
                    Save_Enchantment,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                FindBreakdown(Breakdown_SaveWill));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pES);
            m_items.push_back(pES);
        }
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Illusion",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pIS = new BreakdownItemSave(
                    Breakdown_SaveIllusion,
                    Save_Illusion,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveWill));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pIS);
            m_items.push_back(pIS);
        }
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Fear",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pFS = new BreakdownItemSave(
                    Breakdown_SaveFear,
                    Save_Fear,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveWill));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pFS);
            m_items.push_back(pFS);
        }
        {
            HTREEITEM hSubItem = m_itemBreakdownTree.InsertItem(
                    "vs Curse",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pSC = new BreakdownItemSave(
                    Breakdown_SaveCurse,
                    Save_Curse,
                    &m_itemBreakdownTree,
                    hSubItem,
                    Ability_Unknown,
                    FindBreakdown(Breakdown_SaveWill));
            m_itemBreakdownTree.SetItemData(hSubItem, (DWORD)(void*)pSC);
            m_items.push_back(pSC);
        }
    }
}

void CBreakdownsView::CreateSkillBreakdowns()
{
    // insert the skills root tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Skills", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);
    for (size_t si = Skill_Unknown + 1; si < Skill_Count; ++si)
    {
        // find the correct ability breakdown for this skill
        // so it can be observed
        AbilityType at = StatFromSkill((SkillType)si);
        BreakdownItem * pAbility = NULL;
        switch (at)
        {
        case Ability_Strength:
            pAbility = FindBreakdown(Breakdown_Strength);
            break;
        case Ability_Dexterity:
            pAbility = FindBreakdown(Breakdown_Dexterity);
            break;
        case Ability_Constitution:
            pAbility = FindBreakdown(Breakdown_Constitution);
            break;
        case Ability_Intelligence:
            pAbility = FindBreakdown(Breakdown_Intelligence);
            break;
        case Ability_Wisdom:
            pAbility = FindBreakdown(Breakdown_Wisdom);
            break;
        case Ability_Charisma:
            pAbility = FindBreakdown(Breakdown_Charisma);
            break;
        }
        ASSERT(pAbility != NULL);   // should have found one
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                EnumEntryText((SkillType)si, skillTypeMap),
                hParent,
                TVI_LAST);
        BreakdownItem * pSkill = new BreakdownItemSkill(
                (SkillType)si,
                (BreakdownType)(Breakdown_SkillBalance + si - Skill_Unknown - 1),
                &m_itemBreakdownTree,
                hItem,
                pAbility);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSkill);
        m_items.push_back(pSkill);
    }
}

void CBreakdownsView::CreatePhysicalBreakdowns()
{
    // insert the physical root tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Physical Breakdowns", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hitpoints",
                hParent,
                TVI_LAST);
        BreakdownItem * pHP = new BreakdownItemHitpoints(
                &m_itemBreakdownTree,
                hItem,
                FindBreakdown(Breakdown_Constitution));
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pHP);
        m_items.push_back(pHP);
        {
            HTREEITEM hUnconsciousRange = m_itemBreakdownTree.InsertItem(
                    "Unconscious Range",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pUR = new BreakdownItemSimple(
                    Breakdown_UnconsciousRange,
                    Effect_UnconsciousRange,
                    "Unconscious Range",
                    &m_itemBreakdownTree,
                    hUnconsciousRange);
            m_itemBreakdownTree.SetItemData(hUnconsciousRange, (DWORD)(void*)pUR);
            m_items.push_back(pUR);
        }
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Movement Speed",
                hParent,
                TVI_LAST);
        BreakdownItem * pSpeed = new BreakdownItemSimple(
                Breakdown_MovementSpeed,
                Effect_MovementSpeed,
                "Movement Speed",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSpeed);
        m_items.push_back(pSpeed);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Armor Check Penalty",
                hParent,
                TVI_LAST);
        BreakdownItem * pACP = new BreakdownItemSimple(
                Breakdown_ArmorCheckPenalty,
                Effect_ArmorCheckPenalty,
                "Armor Check penalty",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pACP);
        m_items.push_back(pACP);
    }
    // defensive physical items
    // defensive items are:
    //      AC
    //      PRR
    //      MRR
    //      Fortification
    //      Dodge
    //      Dodge Cap
    //      Missile Deflection
    HTREEITEM hDefensiveParent = m_itemBreakdownTree.InsertItem(
            "Defensive Breakdowns", 
            hParent,
            TVI_LAST);
    m_itemBreakdownTree.SetItemData(hDefensiveParent, 0);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "AC",
                hDefensiveParent,
                TVI_LAST);
        BreakdownItem * pAC = new BreakdownItemSimple(
                Breakdown_AC,
                Effect_ACBonus,
                "AC",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pAC);
        m_items.push_back(pAC);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "PRR",
                hDefensiveParent,
                TVI_LAST);
        BreakdownItem * pPRR = new BreakdownItemPRR(
                Breakdown_PRR,
                "PRR",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pPRR);
        m_items.push_back(pPRR);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "MRR",
                hDefensiveParent,
                TVI_LAST);
        BreakdownItem * pMRR = new BreakdownItemMRR(
                Breakdown_MRR,
                "MRR",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pMRR);
        m_items.push_back(pMRR);
        {
            HTREEITEM hItemCap = m_itemBreakdownTree.InsertItem(
                    "MRR Cap",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pMRRCap = new BreakdownItemMRRCap(
                    Breakdown_MRRCap,
                    "MRR Cap",
                    &m_itemBreakdownTree,
                    hItemCap);
            m_itemBreakdownTree.SetItemData(hItemCap, (DWORD)(void*)pMRRCap);
            m_items.push_back(pMRRCap);
        }
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Fortification",
                hDefensiveParent,
                TVI_LAST);
        BreakdownItem * pFort= new BreakdownItemSimple(
                Breakdown_Fortification,
                Effect_Fortification,
                "Fortification",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pFort);
        m_items.push_back(pFort);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Dodge",
                hDefensiveParent,
                TVI_LAST);
        BreakdownItem * pDodge = new BreakdownItemSimple(
                Breakdown_Dodge,
                Effect_DodgeBonus,
                "Dodge",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pDodge);
        m_items.push_back(pDodge);
        {
            HTREEITEM hItemCap = m_itemBreakdownTree.InsertItem(
                    "Dodge Cap",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pDC = new BreakdownItemSimple(
                    Breakdown_DodgeCap,
                    Effect_DodgeCapBonus,
                    "Dodge Cap",
                    &m_itemBreakdownTree,
                    hItemCap);
            m_itemBreakdownTree.SetItemData(hItemCap, (DWORD)(void*)pDC);
            m_items.push_back(pDC);
        }
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Missile Deflection",
                hDefensiveParent,
                TVI_LAST);
        BreakdownItem * pMD = new BreakdownItemSimple(
                Breakdown_MissileDeflection,
                Effect_MissileDeflection,
                "Missile Deflection",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pMD);
        m_items.push_back(pMD);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Healing Amplification (Positive)",
                hParent,
                TVI_LAST);
        BreakdownItem * pHA = new BreakdownItemSimple(
                Breakdown_HealingAmplification,
                Effect_HealingAmplification,
                "Healing Amplification (Positive)",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pHA);
        m_items.push_back(pHA);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Healing Amplification (Negative)",
                hParent,
                TVI_LAST);
        BreakdownItem * pNHA = new BreakdownItemSimple(
                Breakdown_NegativeHealingAmplification,
                Effect_NegativeHealingAmplification,
                "Healing Amplification (Negative)",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pNHA);
        m_items.push_back(pNHA);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Repair Amplification",
                hParent,
                TVI_LAST);
        BreakdownItem * pRA = new BreakdownItemSimple(
                Breakdown_RepairAmplification,
                Effect_RepairAmplification,
                "Repair Amplification",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pRA);
        m_items.push_back(pRA);
    }

    // offensive breakdowns include:
    // BAB
    // threat generation
    // off hand attack chance
    // doublestrike
    // doubleshot
    // Melee Power
    // Ranged Power
    // Dodge Bypass
    // Fortification Bypass
    // Missile Deflection Bypass
    // Glancing Blows
    HTREEITEM hOffensiveParent = m_itemBreakdownTree.InsertItem(
            "Offensive Breakdowns", 
            hParent,
            TVI_LAST);
    m_itemBreakdownTree.SetItemData(hOffensiveParent, 0);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Base Attack Bonus",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pBAB = new BreakdownItemBAB(
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pBAB);
        m_items.push_back(pBAB);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Melee Threat Generation",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pThreat = new BreakdownItemSimple(
                Breakdown_ThreatMelee,
                Effect_ThreatBonusMelee,
                "Melee Threat Generation",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pThreat);
        m_items.push_back(pThreat);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Ranged Threat Generation",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pThreat = new BreakdownItemSimple(
                Breakdown_ThreatRanged,
                Effect_ThreatBonusRanged,
                "Ranged Threat Generation",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pThreat);
        m_items.push_back(pThreat);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell Threat Generation",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pThreat = new BreakdownItemSimple(
                Breakdown_ThreatSpell,
                Effect_ThreatBonusSpell,
                "Spell Threat Generation",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pThreat);
        m_items.push_back(pThreat);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Off Hand Attack Chance",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pOHA = new BreakdownItemSimple(
                Breakdown_OffHandAttackBonus,
                Effect_OffHandAttackBonus,
                "Off Hand Attack Chance",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pOHA);
        m_items.push_back(pOHA);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Doublestrike",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pDS = new BreakdownItemSimple(
                Breakdown_DoubleStrike,
                Effect_DoubleStrike,
                "Doublestrike",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pDS);
        m_items.push_back(pDS);
        {
            HTREEITEM hOHDS = m_itemBreakdownTree.InsertItem(
                    "Off Hand Doublestrike",
                    hItem,
                    TVI_LAST);
            BreakdownItem * pOHDS = new BreakdownItemSimple(
                    Breakdown_OffHandDoubleStrike,
                    Effect_OffHandDoublestrike,
                    "Off Hand Doublestrike",
                    &m_itemBreakdownTree,
                    hOHDS);
            m_itemBreakdownTree.SetItemData(hOHDS, (DWORD)(void*)pOHDS);
            m_items.push_back(pOHDS);
        }
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Sneak Attack Dice",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pSAD = new BreakdownItemSneakAttackDice(
                Breakdown_SneakAttackDice,
                Effect_SneakAttackDice,
                "Sneak Attack Dice",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSAD);
        m_items.push_back(pSAD);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Sneak Attack Damage",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pSAD = new BreakdownItemSimple(
                Breakdown_SneakAttackDamage,
                Effect_SneakAttackDamage,
                "Sneak Attack Damage",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSAD);
        m_items.push_back(pSAD);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Sneak Attack Attack",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pSAA = new BreakdownItemSimple(
                Breakdown_SneakAttackAttack,
                Effect_SneakAttackAttack,
                "Sneak Attack Attack",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSAA);
        m_items.push_back(pSAA);
    }

    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Doubleshot",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pRA = new BreakdownItemSimple(
                Breakdown_DoubleShot,
                Effect_DoubleShot,
                "Doubleshot",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pRA);
        m_items.push_back(pRA);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Melee Power",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pMP = new BreakdownItemSimple(
                Breakdown_MeleePower,
                Effect_MeleePower,
                "Melee Power",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pMP);
        m_items.push_back(pMP);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Ranged Power",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pRP = new BreakdownItemSimple(
                Breakdown_RangedPower,
                Effect_RangedPower,
                "Ranged Power",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pRP);
        m_items.push_back(pRP);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Dodge Bypass",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pDodgeBypass = new BreakdownItemSimple(
                Breakdown_DodgeBypass,
                Effect_DodgeBypass,
                "Dodge Bypass",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pDodgeBypass);
        m_items.push_back(pDodgeBypass);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Fortification Bypass",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pFortBypass = new BreakdownItemSimple(
                Breakdown_FortificationBypass,
                Effect_FortificationBypass,
                "Fortification Bypass",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pFortBypass);
        m_items.push_back(pFortBypass);
    }
    {
        HTREEITEM hMDB = m_itemBreakdownTree.InsertItem(
                "Missile Deflection Bypass",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pMDB = new BreakdownItemSimple(
                Breakdown_MissileDeflectionBypass,
                Effect_MissileDeflectionBypass,
                "Missile Deflection Bypass",
                &m_itemBreakdownTree,
                hMDB);
        m_itemBreakdownTree.SetItemData(hMDB, (DWORD)(void*)pMDB);
        m_items.push_back(pMDB);
    }
    {
        HTREEITEM hGB = m_itemBreakdownTree.InsertItem(
                "Glancing Blows Chance",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pGBC = new BreakdownItemSimple(
                Breakdown_GlancingBlowsChance,
                Effect_GlancingBlows,
                "Glancing Blows Chance",
                &m_itemBreakdownTree,
                hGB);
        m_itemBreakdownTree.SetItemData(hGB, (DWORD)(void*)pGBC);
        m_items.push_back(pGBC);
    }
    {
        HTREEITEM hGBD = m_itemBreakdownTree.InsertItem(
                "Glancing Blows Damage",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pGBD = new BreakdownItemSimple(
                Breakdown_GlancingBlowsDamage,
                Effect_GlancingBlowsDamage,
                "Glancing Blows Damage",
                &m_itemBreakdownTree,
                hGBD);
        m_itemBreakdownTree.SetItemData(hGBD, (DWORD)(void*)pGBD);
        m_items.push_back(pGBD);
    }
    {
        HTREEITEM hGBEC = m_itemBreakdownTree.InsertItem(
                "Glancing Blow Effect Chance",
                hOffensiveParent,
                TVI_LAST);
        BreakdownItem * pGBEC = new BreakdownItemSimple(
                Breakdown_GlancingBlowEffects,
                Effect_GlancingBlows,
                "Glancing Blow Effect Chance",
                &m_itemBreakdownTree,
                hGBEC);
        m_itemBreakdownTree.SetItemData(hGBEC, (DWORD)(void*)pGBEC);
        m_items.push_back(pGBEC);
    }

    HTREEITEM hTacticalParent = m_itemBreakdownTree.InsertItem(
            "Tactical Breakdowns", 
            hParent,
            TVI_LAST);
    m_itemBreakdownTree.SetItemData(hTacticalParent, 0);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Assassinate",
                hTacticalParent,
                TVI_LAST);
        BreakdownItem * pAssassinate = new BreakdownItemAssassinate(
                Breakdown_Assassinate,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pAssassinate);
        m_items.push_back(pAssassinate);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Stunning Blow",
                hTacticalParent,
                TVI_LAST);
        BreakdownItem * pSB = new BreakdownItemTactical(
                Breakdown_TacticalStunningBlow,
                Tactical_StunningBlow,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSB);
        m_items.push_back(pSB);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Stunning Fist",
                hTacticalParent,
                TVI_LAST);
        BreakdownItem * pSF = new BreakdownItemTactical(
                Breakdown_TacticalStunningFist,
                Tactical_StunningFist,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSF);
        m_items.push_back(pSF);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Sunder",
                hTacticalParent,
                TVI_LAST);
        BreakdownItem * pSunder = new BreakdownItemTactical(
                Breakdown_TacticalSunder,
                Tactical_Sunder,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSunder);
        m_items.push_back(pSunder);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Trip",
                hTacticalParent,
                TVI_LAST);
        BreakdownItem * pTrip = new BreakdownItemTactical(
                Breakdown_TacticalTrip,
                Tactical_Trip,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pTrip);
        m_items.push_back(pTrip);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Stunning Shield",
                hTacticalParent,
                TVI_LAST);
        BreakdownItem * pSS = new BreakdownItemTactical(
                Breakdown_TacticalStunningShield,
                Tactical_StunningShield,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSS);
        m_items.push_back(pSS);
    }
}

void CBreakdownsView::CreateMagicalBreakdowns()
{
    // insert the magical root tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Magical Breakdowns", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell Points",
                hParent,
                TVI_LAST);
        BreakdownItem * pSP = new BreakdownItemSpellPoints(
                Breakdown_Spellpoints,
                Effect_SpellPoints,
                "Spell Points",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSP);
        m_items.push_back(pSP);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Arcane Spell Failure (Armor)",
                hParent,
                TVI_LAST);
        BreakdownItem * pASF = new BreakdownItemSimple(
                Breakdown_ArcaneSpellfailure,
                Effect_ArcaneSpellFailure,
                "Arcane Spell Failure (Armor)",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pASF);
        m_items.push_back(pASF);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Arcane Spell Failure (Shields)",
                hParent,
                TVI_LAST);
        BreakdownItem * pASF = new BreakdownItemSimple(
                Breakdown_ArcaneSpellfailureShields,
                Effect_ArcaneSpellFailureShields,
                "Arcane Spell Failure (Shields)",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pASF);
        m_items.push_back(pASF);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell Resistance",
                hParent,
                TVI_LAST);
        BreakdownItem * pSR = new BreakdownItemSimple(
                Breakdown_SpellResistance,
                Effect_SpellResistance,
                "Spell Resistance",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSR);
        m_items.push_back(pSR);
    }
    HTREEITEM hCasterLevels = m_itemBreakdownTree.InsertItem(
            "Caster Levels", 
            hParent,
            TVI_LAST);
    m_itemBreakdownTree.SetItemData(hCasterLevels, 0);
    AddCasterLevels(hCasterLevels);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell Penetration",
                hParent,
                TVI_LAST);
        BreakdownItem * pSP = new BreakdownItemSimple(
                Breakdown_SpellPenetration,
                Effect_SpellPenetrationBonus,
                "Spell Penetration",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSP);
        m_items.push_back(pSP);
    }

    {
        // spell powers
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell Powers",
                hParent,
                TVI_LAST);
        m_itemBreakdownTree.SetItemData(hItem, 0);
        AddSpellPower(Breakdown_SpellPowerAcid, SpellPower_Acid, "Acid Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerAlignment, SpellPower_Alignment, "Alignment Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerCold, SpellPower_Cold, "Cold Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerEarth, SpellPower_Earth, "Earth Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerElectric, SpellPower_Electric, "Electric Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerFire, SpellPower_Fire, "Fire Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerForce, SpellPower_Force, "Force Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerLight, SpellPower_Light, "Light Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerNegative, SpellPower_Negative, "Negative Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerPhysical, SpellPower_Physical, "Physical Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerPositive, SpellPower_Positive, "Positive Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerRepair, SpellPower_Repair, "Repair Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerRust, SpellPower_Rust, "Rust Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerSonic, SpellPower_Sonic, "Sonic Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerUntyped, SpellPower_Untyped, "Untyped Spell power", hItem);
        AddSpellPower(Breakdown_SpellPowerWater, SpellPower_Water, "Water Spell power", hItem);
    }

    {
        // spell critical
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell Critical Chance",
                hParent,
                TVI_LAST);
        m_itemBreakdownTree.SetItemData(hItem, 0);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceAcid, SpellPower_Acid, "Acid Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceAlignment, SpellPower_Alignment, "Alignment Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceCold, SpellPower_Cold, "Cold Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceEarth, SpellPower_Earth, "Earth Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceElectric, SpellPower_Electric, "Electric Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceFire, SpellPower_Fire, "Fire Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceForce, SpellPower_Force, "Force Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceLight, SpellPower_Light, "Light Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceNegative, SpellPower_Negative, "Negative Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChancePhysical, SpellPower_Physical, "Physical Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChancePositive, SpellPower_Positive, "Positive Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceRepair, SpellPower_Repair, "Repair Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceRust, SpellPower_Rust, "Rust Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceSonic, SpellPower_Sonic, "Sonic Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceUntyped, SpellPower_Untyped, "Untyped Critical Chance", hItem);
        AddSpellCriticalChance(Breakdown_SpellCriticalChanceWater, SpellPower_Water, "Water Critical Chance", hItem);
    }

    {
        // spell critical multipliers
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell Critical Multipliers",
                hParent,
                TVI_LAST);
        m_itemBreakdownTree.SetItemData(hItem, 0);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierAcid, SpellPower_Acid, "Acid Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierAlignment, SpellPower_Alignment, "Alignment Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierCold, SpellPower_Cold, "Cold Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierEarth, SpellPower_Earth, "Earth Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierElectric, SpellPower_Electric, "Electric Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierFire, SpellPower_Fire, "Fire Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierForce, SpellPower_Force, "Force Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierLight, SpellPower_Light, "Light Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierNegative, SpellPower_Negative, "Negative Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierPhysical, SpellPower_Physical, "Physical Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierPositive, SpellPower_Positive, "Positive Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierRepair, SpellPower_Repair, "Repair Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierRust, SpellPower_Rust, "Rust Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierSonic, SpellPower_Sonic, "Sonic Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierUntyped, SpellPower_Untyped, "Untyped Critical Multiplier", hItem);
        AddSpellCriticalMultiplier(Breakdown_SpellCriticalMultiplierWater, SpellPower_Water, "Water Critical Multiplier", hItem);
    }

    {
        // spell schools
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Spell School DCs",
                hParent,
                TVI_LAST);
        m_itemBreakdownTree.SetItemData(hItem, 0);
        AddSpellSchool(Breakdown_SpellSchoolAbjuration, SpellSchool_Abjuration, "Abjuration DC", hItem);
        AddSpellSchool(Breakdown_SpellSchoolConjuration, SpellSchool_Conjuration, "Conjuration DC", hItem);
        AddSpellSchool(Breakdown_SpellSchoolDivination, SpellSchool_Divination, "Divination DC", hItem);
        AddSpellSchool(Breakdown_SpellSchoolEnchantment, SpellSchool_Enchantment, "Enchantment DC", hItem);
        AddSpellSchool(Breakdown_SpellSchoolEvocation, SpellSchool_Evocation, "Evocation DC", hItem);
        AddSpellSchool(Breakdown_SpellSchoolIllusion, SpellSchool_Illusion, "Illusion DC", hItem);
        AddSpellSchool(Breakdown_SpellSchoolNecromancy, SpellSchool_Necromancy, "Necromancy DC", hItem);
        AddSpellSchool(Breakdown_SpellSchoolTransmutation, SpellSchool_Transmutation, "Transmutation DC", hItem);
    }
}

void CBreakdownsView::CreateTurnUndeadBreakdowns()
{
    // insert the Turn Undead tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Turn Undead", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Turn Undead Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pTUL = new BreakdownItemTurnUndeadLevel(
                Breakdown_TurnUndeadLevel,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pTUL);
        m_items.push_back(pTUL);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Turn Undead Hit Dice",
                hParent,
                TVI_LAST);
        BreakdownItem * pTUHD = new BreakdownItemTurnUndeadHitDice(
                Breakdown_TurnUndeadHitDice,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pTUHD);
        m_items.push_back(pTUHD);
    }

}

void CBreakdownsView::AddCasterLevels(HTREEITEM hParent)
{
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Artificer Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pArty = new BreakdownItemCasterLevel(
                Class_Artificer,
                Breakdown_CasterLevel_Artificer,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pArty);
        m_items.push_back(pArty);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Cleric Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pCleric = new BreakdownItemCasterLevel(
                Class_Cleric,
                Breakdown_CasterLevel_Cleric,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pCleric);
        m_items.push_back(pCleric);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Druid Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pDruid = new BreakdownItemCasterLevel(
                Class_Druid,
                Breakdown_CasterLevel_Druid,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pDruid);
        m_items.push_back(pDruid);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Favored Soul Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pFS = new BreakdownItemCasterLevel(
                Class_FavoredSoul,
                Breakdown_CasterLevel_FavoredSoul,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pFS);
        m_items.push_back(pFS);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Paladin Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pPally = new BreakdownItemCasterLevel(
                Class_Paladin,
                Breakdown_CasterLevel_Paladin,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pPally);
        m_items.push_back(pPally);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Sorcerer Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pSorc = new BreakdownItemCasterLevel(
                Class_Sorcerer,
                Breakdown_CasterLevel_Sorcerer,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSorc);
        m_items.push_back(pSorc);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Ranger Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pRanger = new BreakdownItemCasterLevel(
                Class_Ranger,
                Breakdown_CasterLevel_Ranger,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pRanger);
        m_items.push_back(pRanger);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Warlock Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pWarlock = new BreakdownItemCasterLevel(
                Class_Warlock,
                Breakdown_CasterLevel_Warlock,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pWarlock);
        m_items.push_back(pWarlock);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Wizard Caster Level",
                hParent,
                TVI_LAST);
        BreakdownItem * pWizard = new BreakdownItemCasterLevel(
                Class_Wizard,
                Breakdown_CasterLevel_Wizard,
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pWizard);
        m_items.push_back(pWizard);
    }

}

void CBreakdownsView::CreateEnergyResistancesBreakdowns()
{
    // insert the Resistances root tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Energy Resistances Breakdowns", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);
    {
        // Resistances
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Resistances",
                hParent,
                TVI_LAST);
        m_itemBreakdownTree.SetItemData(hItem, 0);
        AddEnergyResistance(Breakdown_EnergyResistanceAcid, Energy_Acid, "Acid Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceCold, Energy_Cold, "Cold Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceElectric, Energy_Electric, "Electric Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceFire, Energy_Fire, "Fire Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceForce, Energy_Force, "Force Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceLight, Energy_Light, "Light Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceNegative, Energy_Negative, "Negative Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistancePoison, Energy_Poison, "Poison Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistancePositive, Energy_Positive, "Positive Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceRepair, Energy_Repair, "Repair Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceRust, Energy_Rust, "Rust Resistance", hItem);
        AddEnergyResistance(Breakdown_EnergyResistanceSonic, Energy_Sonic, "Sonic Resistance", hItem);
    }
    {
        // Absorptions
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Absorption",
                hParent,
                TVI_LAST);
        m_itemBreakdownTree.SetItemData(hItem, 0);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionAcid, Energy_Acid, "Acid Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionCold, Energy_Cold, "Cold Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionElectric, Energy_Electric, "Electric Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionFire, Energy_Fire, "Fire Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionForce, Energy_Force, "Force Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionLight, Energy_Light, "Light Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionNegative, Energy_Negative, "Negative Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionPoison, Energy_Poison, "Poison Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionPositive, Energy_Positive, "Positive Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionRepair, Energy_Repair, "Repair Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionRust, Energy_Rust, "Rust Absorption", hItem);
        AddEnergyAbsorption(Breakdown_EnergyAbsorptionSonic, Energy_Sonic, "Sonic Absorption", hItem);
    }
}

void CBreakdownsView::CreateWeaponBreakdowns()
{
    // we need to know which is this element as we dynamically add/remove
    // items from the list when the equipped items change.
    m_hWeaponsHeader = m_itemBreakdownTree.InsertItem(
            "Weapons", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(m_hWeaponsHeader, NULL);
    // this is not actually added to the list control, but does need access
    // to the tree list and the "Weapons" tree item
    m_pWeaponEffects = new BreakdownItemWeaponEffects(
            &m_itemBreakdownTree,
            m_hWeaponsHeader);
}

void CBreakdownsView::CreateHirelingBreakdowns()
{
    // insert the hirelings root tree item
    HTREEITEM hParent = m_itemBreakdownTree.InsertItem(
            "Hireling Breakdowns", 
            0,
            TVI_ROOT);
    m_itemBreakdownTree.SetItemData(hParent, 0);
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling Abilities",
                hParent,
                TVI_LAST);
        BreakdownItem * pHAB = new BreakdownItemSimple(
                Breakdown_HirelingAbilityBonus,
                Effect_HirelingAbilityBonus,
                "Hireling Abilities",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pHAB);
        m_items.push_back(pHAB);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling Hitpoints",
                hParent,
                TVI_LAST);
        BreakdownItem * pHP = new BreakdownItemSimple(
                Breakdown_HirelingHitpoints,
                Effect_HirelingHitpoints,
                "Hireling Hitpoints",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pHP);
        m_items.push_back(pHP);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling Fortification",
                hParent,
                TVI_LAST);
        BreakdownItem * pFort = new BreakdownItemSimple(
                Breakdown_HirelingFortification,
                Effect_HirelingFortification,
                "Hireling Fortification",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pFort);
        m_items.push_back(pFort);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling PRR",
                hParent,
                TVI_LAST);
        BreakdownItem * pPRR = new BreakdownItemSimple(
                Breakdown_HirelingPRR,
                Effect_HirelingPRR,
                "Hireling PRR",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pPRR);
        m_items.push_back(pPRR);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling MRR",
                hParent,
                TVI_LAST);
        BreakdownItem * pMRR = new BreakdownItemSimple(
                Breakdown_HirelingMRR,
                Effect_HirelingMRR,
                "Hireling MRR",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pMRR);
        m_items.push_back(pMRR);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling Dodge",
                hParent,
                TVI_LAST);
        BreakdownItem * pDodge = new BreakdownItemSimple(
                Breakdown_HirelingDodge,
                Effect_HirelingDodge,
                "Hireling Dodge",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pDodge);
        m_items.push_back(pDodge);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling Melee Power",
                hParent,
                TVI_LAST);
        BreakdownItem * pMelee = new BreakdownItemSimple(
                Breakdown_HirelingMeleePower,
                Effect_HirelingMeleePower,
                "Hireling Melee Power",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pMelee);
        m_items.push_back(pMelee);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling Ranged Power",
                hParent,
                TVI_LAST);
        BreakdownItem * pRanged = new BreakdownItemSimple(
                Breakdown_HirelingRangedPower,
                Effect_HirelingRangedPower,
                "Hireling Ranged Power",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pRanged);
        m_items.push_back(pRanged);
    }
    {
        HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
                "Hireling Spell Power",
                hParent,
                TVI_LAST);
        BreakdownItem * pSpell = new BreakdownItemSimple(
                Breakdown_HirelingSpellPower,
                Effect_HirelingSpellPower,
                "Hireling Spell Power",
                &m_itemBreakdownTree,
                hItem);
        m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSpell);
        m_items.push_back(pSpell);
    }
}

void CBreakdownsView::AddSpellPower(
        BreakdownType bt,
        SpellPowerType type,
        const std::string & name,
        HTREEITEM hParent)
{
    HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
            name.c_str(),
            hParent,
            TVI_LAST);
    BreakdownItem * pSPItem = new BreakdownItemSpellPower(
            bt,
            Effect_SpellPower,
            type,
            name.c_str(),
            &m_itemBreakdownTree,
            hItem);
    m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSPItem);
    m_items.push_back(pSPItem);
}

void CBreakdownsView::AddSpellCriticalChance(
        BreakdownType bt,
        SpellPowerType type,
        const std::string & name,
        HTREEITEM hParent)
{
    HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
            name.c_str(),
            hParent,
            TVI_LAST);
    BreakdownItem * pSPItem = new BreakdownItemSpellPower(
            bt,
            Effect_SpellLore,
            type,
            name.c_str(),
            &m_itemBreakdownTree,
            hItem);
    m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSPItem);
    m_items.push_back(pSPItem);
}

void CBreakdownsView::AddSpellCriticalMultiplier(
        BreakdownType bt,
        SpellPowerType type,
        const std::string & name,
        HTREEITEM hParent)
{
    HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
            name.c_str(),
            hParent,
            TVI_LAST);
    BreakdownItem * pSPItem = new BreakdownItemSpellPower(
            bt,
            Effect_SpellCriticalDamage,
            type,
            name.c_str(),
            &m_itemBreakdownTree,
            hItem);
    m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSPItem);
    m_items.push_back(pSPItem);
}

void CBreakdownsView::AddSpellSchool(
        BreakdownType bt,
        SpellSchoolType type,
        const std::string & name,
        HTREEITEM hParent)
{
    HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
            name.c_str(),
            hParent,
            TVI_LAST);
    BreakdownItem * pSSItem = new BreakdownItemSpellSchool(
            bt,
            Effect_SpellDC,
            type,
            name.c_str(),
            &m_itemBreakdownTree,
            hItem);
    m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pSSItem);
    m_items.push_back(pSSItem);
}

void CBreakdownsView::AddEnergyResistance(
        BreakdownType bt,
        EnergyType type,
        const std::string & name,
        HTREEITEM hParent)
{
    HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
            name.c_str(),
            hParent,
            TVI_LAST);
    BreakdownItem * pERItem = new BreakdownItemEnergyResistance(
            bt,
            Effect_EnergyResistance,
            type,
            name.c_str(),
            &m_itemBreakdownTree,
            hItem);
    m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pERItem);
    m_items.push_back(pERItem);
}

void CBreakdownsView::AddEnergyAbsorption(
        BreakdownType bt,
        EnergyType type,
        const std::string & name,
        HTREEITEM hParent)
{
    HTREEITEM hItem = m_itemBreakdownTree.InsertItem(
            name.c_str(),
            hParent,
            TVI_LAST);
    BreakdownItem * pERItem = new BreakdownItemEnergyAbsorption(
            bt,
            Effect_EnergyAbsorbance,
            type,
            name.c_str(),
            &m_itemBreakdownTree,
            hItem);
    m_itemBreakdownTree.SetItemData(hItem, (DWORD)(void*)pERItem);
    m_items.push_back(pERItem);
}

BreakdownItem * CBreakdownsView::FindBreakdown(BreakdownType type) const
{
    BreakdownItem * pItem = NULL;
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        if (m_items[i]->Type() == type)
        {
            pItem = m_items[i];
            break;
        }
    }
    if (type == Breakdown_WeaponEffectHolder)
    {
        pItem = m_pWeaponEffects;
    }
    return pItem;
}

void CBreakdownsView::OnSelChangedBreakdownTree(NMHDR* pNMHDR, LRESULT* pResult)
{
    HTREEITEM hItem = m_itemBreakdownTree.GetSelectedItem();
    if (hItem != NULL)
    {
        DWORD itemData = m_itemBreakdownTree.GetItemData(hItem);
        if (itemData != 0)      // headings don't have items to display
        {
            BreakdownItem * pItem = static_cast<BreakdownItem *>((void*)itemData);
            pItem->PopulateBreakdownControl(&m_itemBreakdownList);
            m_buttonClipboard.EnableWindow(TRUE);
        }
        else
        {
            // just empty the control
            m_itemBreakdownList.DeleteAllItems();
            // no specific item selected, therefore for clipboard copy
            m_buttonClipboard.EnableWindow(FALSE);
        }
    }
}

BOOL CBreakdownsView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    CPoint point;
    BOOL retVal = FALSE;

    // is the mouse over the divider control?
    GetCursorPos(&point);
    CWnd *pWnd2 = WindowFromPoint(point);
    if (pWnd2 != NULL)
    {
        if (&m_divider == pWnd2)
        {
            // it is, show a drag up/down cursor
            SetCursor(LoadCursor(NULL, IDC_SIZENS));
            retVal = TRUE;
        }
    }
    else if (m_bDraggingDivider)
    {
        // also set the cursor if a drag action is in progress
        SetCursor(LoadCursor(NULL, IDC_SIZENS));
        retVal = TRUE;
    }

    if (retVal == FALSE)
    {
        // call base class if we didn't handle special case
        retVal = CFormView::OnSetCursor(pWnd, nHitTest, message);
    }
    return retVal;
}

void CBreakdownsView::OnDividerClicked()
{
    CRect rect;
    m_bDraggingDivider = true;
    m_divider.GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_treeSizePercent = rect.top - c_controlSpacing;
    SetCapture();               // need all mouse messages while drag in progress
    SetCursor(LoadCursor(NULL, IDC_SIZENS));
}

void CBreakdownsView::OnMouseMove(UINT nFlags, CPoint point)
{
    CFormView::OnMouseMove(nFlags, point);

    if (m_bDraggingDivider)
    {
        CalculatePercent(point);
        SetCursor(LoadCursor(NULL, IDC_SIZENS));
        m_divider.Invalidate();
    }
}

void CBreakdownsView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CFormView::OnLButtonUp(nFlags, point);
    if (m_bDraggingDivider)
    {
        ReleaseCapture();
        m_bDraggingDivider = false;
        CalculatePercent(point);
        // save the new value to registry to persist it
        AfxGetApp()->WriteProfileInt(f_treeSizeKey, f_treeSizeEntry, m_treeSizePercent);
    }
}

void CBreakdownsView::CalculatePercent(CPoint point)
{
    CRect rect;
    CRect rctDivider;
    GetClientRect(&rect);
    m_divider.GetWindowRect(&rctDivider);
    int verticalSpace = rect.Height() - (c_controlSpacing * 4) - rctDivider.Height();
    m_treeSizePercent = (point.y - (c_controlSpacing * 2)) * 100 / verticalSpace;

    // ensure other window is always visible in some way
    if (m_treeSizePercent < 10)
    {
        m_treeSizePercent = 10;
    }
    if (m_treeSizePercent > 90)
    {
        m_treeSizePercent = 90;
    }

    // now force an on size event
    CRect rctWnd;
    GetClientRect(&rctWnd);
    OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
}

void CBreakdownsView::OnEndtrackBreakdownList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // just save the column widths to registry so restored next time we run
    UNREFERENCED_PARAMETER(pNMHDR);
    UNREFERENCED_PARAMETER(pResult);
    SaveColumnWidthsByName(&m_itemBreakdownList, "BreakdownList_%s");
}

void CBreakdownsView::OnButtonClipboardCopy()
{
    // build up column data so we can align the clipboard text output
    std::vector<CString> columns[CO_count];
    // title is the selected breakdown and its total
    HTREEITEM hItem = m_itemBreakdownTree.GetSelectedItem();
    if (hItem != NULL)
    {
        DWORD itemData = m_itemBreakdownTree.GetItemData(hItem);
        if (itemData != 0)      // headings don't have items to display
        {
            BreakdownItem * pItem = static_cast<BreakdownItem *>((void*)itemData);
            // first item is the breakdown item and total
            columns[CO_Source].push_back(pItem->Title());
            columns[CO_Value].push_back(pItem->Value());
            columns[CO_Stacks].push_back("");
            columns[CO_BonusType].push_back("");
            // now add headings in the list control
            CHeaderCtrl* pHeaderCtrl = m_itemBreakdownList.GetHeaderCtrl();
            int nColumnCount = pHeaderCtrl->GetItemCount();
            ASSERT(nColumnCount == CO_count);
            for (int column = 0; column < nColumnCount; ++column)
            {
                HDITEM item;
                char itemText[100];
                item.mask = HDI_TEXT;
                item.pszText = itemText;
                item.cchTextMax = 100;
                pHeaderCtrl->GetItem(column, &item);
                columns[column].push_back(item.pszText);
            }
            // now add the content
            size_t count = (size_t)m_itemBreakdownList.GetItemCount();
            for (size_t i = 0; i < count; ++i)
            {
                CString itemText;
                for (int column = 0; column < nColumnCount; ++column)
                {
                    itemText = m_itemBreakdownList.GetItemText(i, column);
                    columns[column].push_back(itemText);
                }
            }
            // now pad all items in each column to the same size
            int maxWidth[CO_count] = {0, 0, 0, 0};
            for (size_t i = 0; i < columns[0].size(); ++i)
            {
                for (int column = 0; column < nColumnCount; ++column)
                {
                    maxWidth[column] = max(maxWidth[column], columns[column][i].GetLength());
                }
            }
            for (size_t i = 0; i < columns[0].size(); ++i)
            {
                CString padded;
                for (int column = 0; column < nColumnCount; ++column)
                {
                    padded.Format("%*s", maxWidth[column] + 2, columns[column][i]); // 2 extra spaces
                    columns[column][i] = padded;
                }
            }
            // now generate the total clipboard text, ensure mono-spaced
            CString clipboardText;
            clipboardText += "[code]\r\n";
            for (size_t i = 0; i < columns[0].size(); ++i)
            {
                CString padded;
                for (int column = 0; column < nColumnCount; ++column)
                {
                    clipboardText += columns[column][i];
                }
                clipboardText += "\r\n";
            }
            clipboardText += "[/code]\r\n";
            // now place the text on the clipboard
            if (OpenClipboard())
            {
                HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, clipboardText.GetLength()+1);
                ASSERT(clipbuffer != NULL);
                char *buffer = (char*)GlobalLock(clipbuffer);
                strcpy_s(buffer, clipboardText.GetLength()+1, clipboardText);
                GlobalUnlock(clipbuffer);
                EmptyClipboard();
                SetClipboardData(CF_TEXT, clipbuffer);
                CloseClipboard();
            }
        }
    }
}

void CBreakdownsView::UpdateGearChanged(
        Character * pCharData,
        InventorySlotType slot)
{
    if (slot == Inventory_Weapon1
            || slot == Inventory_Weapon2)
    {
        // m_pWeaponEffects holds the actual breakdowns, we just tell it
        // to re-create them based on the selected items
        m_pWeaponEffects->WeaponsChanged(m_pCharacter->ActiveGearSet());
    }
}
