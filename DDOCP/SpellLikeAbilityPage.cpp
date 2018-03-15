// SpellLikeAbilityPage.cpp
//
#include "stdafx.h"
#include "SpellLikeAbilityPage.h"
#include "GlobalSupportFunctions.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellLikeAbilityPage property page

CSpellLikeAbilityPage::CSpellLikeAbilityPage() :
    CPropertyPage(CSpellLikeAbilityPage::IDD),
    m_pCharacter(NULL)
{
    //{{AFX_DATA_INIT(CSpellLikeAbilityPage)
    //}}AFX_DATA_INIT
}

CSpellLikeAbilityPage::~CSpellLikeAbilityPage()
{
}

void CSpellLikeAbilityPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSpellLikeAbilityPage)
    DDX_Control(pDX, IDC_SPELLS, m_slas);
    //}}AFX_DATA_MAP
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSpellLikeAbilityPage, CPropertyPage)
    //{{AFX_MSG_MAP(CSpellLikeAbilityPage)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

/////////////////////////////////////////////////////////////////////////////
// CSpellLikeAbilityPage message handlers

BOOL CSpellLikeAbilityPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpellLikeAbilityPage::OnSize(UINT nType, int cx, int cy)
{
    CPropertyPage::OnSize(nType, cx, cy);
    if (IsWindow(m_slas.GetSafeHwnd()))
    {
        // spells control takes entire client area
        m_slas.MoveWindow(0, 0, cx, cy);
    }
}

BOOL CSpellLikeAbilityPage::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_SPELLS,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CSpellLikeAbilityPage::SetCharacter(Character * pCharacter)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }
    m_pCharacter = pCharacter;
    m_slas.SetCharacter(pCharacter);
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
}

void CSpellLikeAbilityPage::UpdateFeatEffect(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    // only interested in Effect_SpellLikeAbility
    if (effect.Type() == Effect_SpellLikeAbility)
    {
        m_slas.AddSLA(effect.SpellLikeAbility(), 1);
    }
}

void CSpellLikeAbilityPage::UpdateFeatEffectRevoked(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_SpellLikeAbility)
    {
        m_slas.RevokeSLA(effect.SpellLikeAbility());
    }
}

void CSpellLikeAbilityPage::UpdateItemEffect(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_SpellLikeAbility)
    {
        m_slas.AddSLA(effect.SpellLikeAbility(), 1);
    }
}

void CSpellLikeAbilityPage::UpdateItemEffectRevoked(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_SpellLikeAbility)
    {
        m_slas.RevokeSLA(effect.SpellLikeAbility());
    }
}

void CSpellLikeAbilityPage::UpdateEnhancementEffect(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_SpellLikeAbility)
    {
        m_slas.AddSLA(effect.m_effect.SpellLikeAbility(), effect.m_tier);
    }
}

void CSpellLikeAbilityPage::UpdateEnhancementEffectRevoked(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_SpellLikeAbility)
    {
        m_slas.RevokeSLA(effect.m_effect.SpellLikeAbility());
    }
}

const CSLAControl * CSpellLikeAbilityPage::GetSLAControl()
{
    return &m_slas;
}

