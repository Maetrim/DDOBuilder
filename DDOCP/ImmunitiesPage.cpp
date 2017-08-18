// ImmunitiesPage.cpp
//
#include "stdafx.h"
#include "ImmunitiesPage.h"
#include "GlobalSupportFunctions.h"

/////////////////////////////////////////////////////////////////////////////
// CImmunitiesPage property page
namespace
{
    const size_t c_controlSpacing = 3;
}

CImmunitiesPage::CImmunitiesPage() :
    CPropertyPage(CImmunitiesPage::IDD),
    m_pCharacter(NULL)
{
    //{{AFX_DATA_INIT(CImmunitiesPage)
    //}}AFX_DATA_INIT
}

CImmunitiesPage::~CImmunitiesPage()
{
}

void CImmunitiesPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CImmunitiesPage)
    DDX_Control(pDX, IDC_IMMUNITY_LIST, m_listImmunity);
    //}}AFX_DATA_MAP
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CImmunitiesPage, CPropertyPage)
    //{{AFX_MSG_MAP(CImmunitiesPage)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_NOTIFY(HDN_ENDTRACK, IDC_IMMUNITY_LIST, OnEndtrackImmunityList)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, IDC_IMMUNITY_LIST, OnEndtrackImmunityList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

/////////////////////////////////////////////////////////////////////////////
// CImmunitiesPage message handlers

BOOL CImmunitiesPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // add the columns to the list control
    m_listImmunity.InsertColumn(0, "Immunity To", LVCFMT_LEFT, 100, 0);
    m_listImmunity.InsertColumn(1, "Source", LVCFMT_LEFT, 150, 0);
    m_listImmunity.SetExtendedStyle(m_listImmunity.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    LoadColumnWidthsByName(&m_listImmunity, "ImmunityList_%s");

    Populate();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CImmunitiesPage::OnSize(UINT nType, int cx, int cy)
{
    CPropertyPage::OnSize(nType, cx, cy);
    if (IsWindow(m_listImmunity.GetSafeHwnd()))
    {
        // control takes entire client area minus border
        m_listImmunity.MoveWindow(c_controlSpacing, c_controlSpacing, cx - c_controlSpacing, cy - c_controlSpacing);
    }
}

BOOL CImmunitiesPage::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_IMMUNITY_LIST,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CImmunitiesPage::OnEndtrackImmunityList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // just save the column widths to registry so restored next time we run
    UNREFERENCED_PARAMETER(pNMHDR);
    UNREFERENCED_PARAMETER(pResult);
    SaveColumnWidthsByName(&m_listImmunity, "ImmunityList_%s");
}

void CImmunitiesPage::SetCharacter(Character * pCharacter)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }
    m_pCharacter = pCharacter;
    m_immunities.clear();
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
    Populate();
}

void CImmunitiesPage::Populate()
{
    // add all items in the immunities list to the control
    if (IsWindow(m_listImmunity.GetSafeHwnd()))
    {
        m_listImmunity.LockWindowUpdate();
        m_listImmunity.DeleteAllItems();
        std::list<ImmunityItem>::iterator it = m_immunities.begin();
        while (it != m_immunities.end())
        {
            int index = m_listImmunity.InsertItem(
                    m_listImmunity.GetItemCount(),
                    (*it).Immunity().c_str(),
                    0);
            m_listImmunity.SetItemText(
                    index,
                    1,
                    (*it).Source().c_str());
            ++it;
        }
        m_listImmunity.UnlockWindowUpdate();
    }
}

void CImmunitiesPage::UpdateFeatEffect(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    // only interested in Effect_Immunity
    if (effect.Type() == Effect_Immunity)
    {
        AddImmunity(effect.Immunity(), featName);
    }
}

void CImmunitiesPage::UpdateFeatEffectRevoked(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_Immunity)
    {
        RevokeImmunity(effect.Immunity(), featName);
    }
}

void CImmunitiesPage::UpdateItemEffect(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    // only interested in Effect_Immunity
    if (effect.Type() == Effect_Immunity)
    {
        AddImmunity(effect.Immunity(), itemName);
    }
}

void CImmunitiesPage::UpdateItemEffectRevoked(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_Immunity)
    {
        RevokeImmunity(effect.Immunity(), itemName);
    }
}

void CImmunitiesPage::UpdateEnhancementEffect(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_Immunity)
    {
        AddImmunity(effect.m_effect.Immunity(), enhancementName);
    }
}

void CImmunitiesPage::UpdateEnhancementEffectRevoked(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_Immunity)
    {
        RevokeImmunity(effect.m_effect.Immunity(), enhancementName);
    }
}

void CImmunitiesPage::AddImmunity(
        const std::string & immunity,
        const std::string & source)
{
    ImmunityItem item(immunity, source);
    m_immunities.push_back(item);
    // update the list
    Populate();
}

void CImmunitiesPage::RevokeImmunity(
        const std::string & immunity,
        const std::string & source)
{
    ImmunityItem item(immunity, source);
    // find the item in the list and remove
    bool found = false;
    m_immunities.push_back(item);
    std::list<ImmunityItem>::iterator it = m_immunities.begin();
    while (it != m_immunities.end())
    {
        if ((*it) == item)
        {
            m_immunities.erase(it);
            found = true;
            break;
        }
        else
        {
            ++it;
        }
    }
    // update the list
    if (found)
    {
        Populate();
    }
}
