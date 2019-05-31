// ClassAndFeatView.cpp
//
#include "stdafx.h"
#include "ClassAndFeatView.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
}

IMPLEMENT_DYNCREATE(CClassAndFeatView, CFormView)

CClassAndFeatView::CClassAndFeatView() :
    CFormView(CClassAndFeatView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_bHadCreate(false)
{
}

CClassAndFeatView::~CClassAndFeatView()
{
}

void CClassAndFeatView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CLASS_AND_FEAT_LIST, m_featsAndClasses);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CClassAndFeatView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CClassAndFeatView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CClassAndFeatView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CClassAndFeatView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    if (!m_bHadCreate)
    {
        //m_wndFeatNavigationBar.Create(
        //        CRect(0, 0, 100, 100),
        //        this,
        //        1);
        //CRect rectDummy(0, 0, 0, 0);
        //m_listFeats[0].Create(
        //        WS_VISIBLE | WS_CHILD,
        //        rectDummy,
        //        &m_wndFeatNavigationBar,
        //        2);
        //m_wndFeatNavigationBar.AddControl(&m_listFeats[0], "Control 1", 2, TRUE, AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE);
        //m_listFeats[1].Create(WS_VISIBLE | WS_CHILD, rectDummy, &m_wndFeatNavigationBar, 3);
        //m_wndFeatNavigationBar.AddControl(&m_listFeats[1], "Control 2", 3, TRUE, AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE);
        //m_listFeats[2].Create(WS_VISIBLE | WS_CHILD, rectDummy, &m_wndFeatNavigationBar, 4);
        //m_wndFeatNavigationBar.AddControl(&m_listFeats[2], "Control 3", 4, TRUE, AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE);
        //m_listFeats[3].Create(WS_VISIBLE | WS_CHILD, rectDummy, &m_wndFeatNavigationBar, 5);
        //m_wndFeatNavigationBar.AddControl(&m_listFeats[3], "Control 4", 5, TRUE, AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE);
        //m_listFeats[4].Create(WS_VISIBLE | WS_CHILD, rectDummy, &m_wndFeatNavigationBar, 6);
        //m_wndFeatNavigationBar.AddControl(&m_listFeats[4], "Control 5", 6, TRUE, AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE);
    }
    m_bHadCreate = true;
}

void CClassAndFeatView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);
    if (IsWindow(m_featsAndClasses.GetSafeHwnd()))
    {
        CSize requiredSize = m_featsAndClasses.RequiredSize();
        CRect rctFeats;
        rctFeats.left = 0;
        rctFeats.right = max(requiredSize.cx, cx); //cx - (cx / 3) - 3;
        rctFeats.top = 0;
        rctFeats.bottom = max(requiredSize.cy, cy);
        m_featsAndClasses.MoveWindow(rctFeats, TRUE);
        //CRect rctNavBar;
        //rctNavBar.left = cx - (cx / 3);
        //rctNavBar.right = cx - 1;
        //rctNavBar.top = 0;
        //rctNavBar.bottom = cy;
        //m_wndFeatNavigationBar.MoveWindow(rctNavBar);
        SetScrollSizes(MM_TEXT, requiredSize);
    }
}

LRESULT CClassAndFeatView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }

    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
    m_featsAndClasses.SetCharacter(m_pCharacter);
    CRect rect;
    GetClientRect(&rect);
    OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
    return 0L;
}

BOOL CClassAndFeatView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_CLASS_AND_FEAT_LIST,
        //1,                  // m_wndFeatNavigationBar
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}
