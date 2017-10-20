// SpellsPage.cpp
//
#include "stdafx.h"
#include "SpellsPage.h"
#include "GlobalSupportFunctions.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellsPage property page

CSpellsPage::CSpellsPage(ClassType ct, AbilityType ability, UINT strResourceId) :
    CPropertyPage(CSpellsPage::IDD, strResourceId),
    m_pCharacter(NULL),
    m_classType(ct),
    m_abilityType(ability)
{
    //{{AFX_DATA_INIT(CSpellsPage)
    //}}AFX_DATA_INIT
}

CSpellsPage::~CSpellsPage()
{
}

void CSpellsPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSpellsPage)
    DDX_Control(pDX, IDC_SPELLS, m_spells);
    //}}AFX_DATA_MAP
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSpellsPage, CPropertyPage)
    //{{AFX_MSG_MAP(CSpellsPage)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

/////////////////////////////////////////////////////////////////////////////
// CSpellsPage message handlers

BOOL CSpellsPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpellsPage::OnSize(UINT nType, int cx, int cy)
{
    CPropertyPage::OnSize(nType, cx, cy);
    if (IsWindow(m_spells.GetSafeHwnd()))
    {
        // spells control takes entire client area
        m_spells.MoveWindow(0, 0, cx, cy);
    }
}

BOOL CSpellsPage::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_SPELLS,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CSpellsPage::SetCharacter(Character * pCharacter)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }
    m_pCharacter = pCharacter;
    m_spells.SetCharacter(pCharacter, m_classType);
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
}

void CSpellsPage::SetTrainableSpells(const std::vector<size_t> & spellsPerLevel)
{
    m_spells.SetTrainableSpells(spellsPerLevel);
}

void CSpellsPage::UpdateSpellTrained(
        Character * charData,
        const TrainedSpell & spell)
{
    if (spell.Class() == m_classType)
    {
        // this spell is for us, update our control
        m_spells.UpdateSpells();
    }
}

void CSpellsPage::UpdateSpellRevoked(
        Character * charData,
        const TrainedSpell & spell)
{
    if (spell.Class() == m_classType)
    {
        // this spell is for us, update our control
        m_spells.UpdateSpells();
    }
}

void CSpellsPage::UpdateFeatEffect(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    // only interested in Effect_GrantSpell
    if (effect.Type() == Effect_GrantSpell
            && (effect.Class() == m_classType || effect.Class() == Class_All))
    {
        m_spells.AddFixedSpell(
                effect.Spell(),
                effect.SpellLevel());
    }
}

void CSpellsPage::UpdateFeatEffectRevoked(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_GrantSpell
            && (effect.Class() == m_classType || effect.Class() == Class_All))
    {
        m_spells.RevokeFixedSpell(
                effect.Spell(),
                effect.SpellLevel());
    }
}

void CSpellsPage::UpdateItemEffect(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    // only interested in Effect_GrantSpell
    if (effect.Type() == Effect_GrantSpell
            && (effect.Class() == m_classType || effect.Class() == Class_All))
    {
        m_spells.AddFixedSpell(
                effect.Spell(),
                effect.SpellLevel());
    }
}

void CSpellsPage::UpdateItemEffectRevoked(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_GrantSpell
            && (effect.Class() == m_classType || effect.Class() == Class_All))
    {
        m_spells.RevokeFixedSpell(
                effect.Spell(),
                effect.SpellLevel());
    }
}

void CSpellsPage::UpdateEnhancementEffect(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_GrantSpell
            && (effect.m_effect.Class() == m_classType || effect.m_effect.Class() == Class_All))
    {
        m_spells.AddFixedSpell(
                effect.m_effect.Spell(),
                effect.m_effect.SpellLevel());
    }
}

void CSpellsPage::UpdateEnhancementEffectRevoked(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_GrantSpell
            && (effect.m_effect.Class() == m_classType || effect.m_effect.Class() == Class_All))
    {
        m_spells.RevokeFixedSpell(
                effect.m_effect.Spell(),
                effect.m_effect.SpellLevel());
    }
}
