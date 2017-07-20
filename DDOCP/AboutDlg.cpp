// AboutDlg.cpp
//
#include "stdafx.h"
#include "AboutDlg.h"
#include "BuildInfo.h"

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_VERSION, m_staticVersion);

    CString text;
    text.Format("DDOCP, Version %s", BUILDINFO_VERSION);
    m_staticVersion.SetWindowText(text);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
