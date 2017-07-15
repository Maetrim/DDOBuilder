// GearSetNameDialog.cpp
//
#include "stdafx.h"
#include "GearSetNameDialog.h"

#include "Character.h"

// CGearSetNameDialog dialog

IMPLEMENT_DYNAMIC(CGearSetNameDialog, CDialog)

CGearSetNameDialog::CGearSetNameDialog(
        CWnd* pParent,
        const Character * pCharacter,
        const std::string & defaultName) :
    CDialog(CGearSetNameDialog::IDD, pParent),
    m_pCharacter(pCharacter),
    m_name(defaultName)
{
}

CGearSetNameDialog::~CGearSetNameDialog()
{
}

void CGearSetNameDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_GEAR_SET_NAME, m_editGearName);
}

BEGIN_MESSAGE_MAP(CGearSetNameDialog, CDialog)
END_MESSAGE_MAP()

// CGearSetNameDialog message handlers
BOOL CGearSetNameDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_editGearName.SetWindowText(m_name.c_str());

    return TRUE;
}

void CGearSetNameDialog::OnOK()
{
    // check that the name entered is unique and does not already exist
    CString name;
    m_editGearName.GetWindowText(name);
    const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
    bool unique = true;
    std::list<EquippedGear>::const_iterator it = setups.begin();
    while (unique && it != setups.end())
    {
        unique = ((*it).Name() != (LPCSTR)name);
        ++it;
    }
    if (!unique)
    {
        // tell user name is not unique and get them to re-enter
        AfxMessageBox("The gear name you entered already exists.\n"
                "All gear names must be unique. Please try again with a different name.",
                MB_ICONEXCLAMATION);
    }
    else
    {
        // all ok, return success
        m_name = (LPCSTR)name;
        CDialog::OnOK();
    }
}

std::string CGearSetNameDialog::Name() const
{
    return m_name;
}

