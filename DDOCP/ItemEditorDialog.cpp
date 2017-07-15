// ItemEditorDialog.cpp
//

#include "stdafx.h"
#include "ItemEditorDialog.h"

// CItemEditorDialog dialog

IMPLEMENT_DYNAMIC(CItemEditorDialog, CDialog)

CItemEditorDialog::CItemEditorDialog(CWnd* pParent) :
    CDialog(CItemEditorDialog::IDD, pParent)
{

}

CItemEditorDialog::~CItemEditorDialog()
{
}

void CItemEditorDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CItemEditorDialog, CDialog)
END_MESSAGE_MAP()


// CItemEditorDialog message handlers
BOOL CItemEditorDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
