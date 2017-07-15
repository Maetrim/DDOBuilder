// ActiveStanceDialog.cpp
//

#include "stdafx.h"
#include "ActiveStanceDialog.h"

#include "GlobalSupportFunctions.h"

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CActiveStanceDialog, CDialog)
    //{{AFX_MSG_MAP(CActiveStanceDialog)
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_BUTTON_STANCE, OnButtonStance)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CActiveStanceDialog::CActiveStanceDialog(
        CWnd* pParent,
        Character * pCharacter,
        const Stance & stance) :
    CDialog(CActiveStanceDialog::IDD, pParent),
    m_stance(stance),
    m_pCharacter(pCharacter),
    m_isActive(false),
    m_stacks(1)
{
    //{{AFX_DATA_INIT(CActiveStanceDialog)
    //}}AFX_DATA_INIT
    pCharacter->AttachObserver(this);
}

bool CActiveStanceDialog::IsYou(const Stance & stance) const
{
    return (stance.Name() == m_stance.Name());
}

void CActiveStanceDialog::AddStack()
{
    ++m_stacks;
}

void CActiveStanceDialog::LoseStack()
{
    --m_stacks;
}

size_t CActiveStanceDialog::NumStacks() const
{
    return m_stacks;
}

void CActiveStanceDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CActiveStanceDialog)
    DDX_Control(pDX, IDC_BUTTON_STANCE, m_buttonStance);
    //}}AFX_DATA_MAP
}

BOOL CActiveStanceDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    if (m_tooltip.Create(this))
    {
        m_tooltipConfig.m_bBoldLabel = true;
        m_tooltipConfig.m_bDrawSeparator = true;
        m_tooltipConfig.m_nMaxDescrWidth = 150;
        m_tooltipConfig.m_bVislManagerTheme = true;
        m_tooltip.SetParams(&m_tooltipConfig);
        std::string tipText;
        tipText = "Click to active this stance: " + m_stance.Name();
        m_tooltip.AddTool(&m_buttonStance, tipText.c_str());
        m_tooltip.Activate(TRUE);
    }
    SetupControls();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CActiveStanceDialog::SetupControls()
{
    if (m_pCharacter != NULL)
    {
        m_isActive = m_pCharacter->IsStanceActive(m_stance.Name());
        m_buttonStance.SetWindowText(m_stance.Name().c_str());
        CImage image;
        // could be IT_feat also
        HRESULT result = LoadImageFile(IT_enhancement, m_stance.Icon(), &image, false);
        if (result != S_OK)
        {
            result = LoadImageFile(IT_feat, m_stance.Icon(), &image, false);
        }
        if (result != S_OK)
        {
            result = LoadImageFile(IT_spell, m_stance.Icon(), &image, true);
        }
        m_buttonStance.SetImage(image.Detach());
        m_buttonStance.SetTransparent();
        m_buttonStance.Invalidate(FALSE);    // button must redraw
    }
    else
    {
        // no document selected
        m_buttonStance.SetWindowText("ERROR!");
    }
    m_buttonStance.SetCheck(m_isActive ? BST_CHECKED : BST_UNCHECKED);
    m_buttonStance.m_bTopImage = true;
}

BOOL CActiveStanceDialog::PreTranslateMessage(MSG* pMsg)
{
    m_tooltip.RelayEvent(pMsg);
    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CActiveStanceDialog::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_BUTTON_STANCE,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

// character observer overrides
void CActiveStanceDialog::UpdateStanceActivated(
        Character * charData,
        const std::string & stanceName)
{
    // if our stance just became active, set the button state
    if (stanceName == m_stance.Name())
    {
        m_buttonStance.SetCheck(BST_CHECKED);
        m_isActive = true;
    }
}

void CActiveStanceDialog::UpdateStanceDeactivated(
        Character * charData,
        const std::string & stanceName)
{
    // if our stance just became de-active, clear the button state
    if (stanceName == m_stance.Name())
    {
        m_buttonStance.SetCheck(BST_UNCHECKED);
        m_isActive = false;
    }
}

void CActiveStanceDialog::OnButtonStance()
{
    // toggle our stance state
    m_isActive = !m_isActive;
    m_buttonStance.SetCheck(m_isActive ? BST_CHECKED : BST_UNCHECKED);
    if (m_isActive)
    {
        m_pCharacter->ActivateStance(m_stance);
    }
    else
    {
        m_pCharacter->DeactivateStance(m_stance);
    }
}

