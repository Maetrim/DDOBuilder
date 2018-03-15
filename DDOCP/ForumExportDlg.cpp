// ForumExportDlg.cpp
//
#include "stdafx.h"
#include "ForumExportDlg.h"

CForumExportDlg::CForumExportDlg() : CDialogEx(CForumExportDlg::IDD)
{
}

void CForumExportDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SECTIONS, m_listConfigureExport);
    DDX_Control(pDX, IDC_EDIT_EXPORT, m_editExport);
}

BEGIN_MESSAGE_MAP(CForumExportDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CForumExportDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();



    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
