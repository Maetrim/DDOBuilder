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
    m_pDocument(NULL)
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
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
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
}

void CClassAndFeatView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);
    if (IsWindow(m_featsAndClasses.GetSafeHwnd()))
    {
        CSize requiredSize = m_featsAndClasses.RequiredSize();
        if (requiredSize == CSize(0, 0))
        {
            // no size, make it fit the window
            requiredSize = CSize(cx, cy);
        }
        int scrollX = GetScrollPos(SB_HORZ);
        int scrollY = GetScrollPos(SB_VERT);
        CRect rctControl(0, 0, requiredSize.cx, requiredSize.cy);
        rctControl -= CPoint(scrollX, scrollY);
        m_featsAndClasses.MoveWindow(rctControl, TRUE);
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
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CClassAndFeatView::OnHScroll(UINT p1, UINT p2, CScrollBar* p3)
{
    // call base class
    CFormView::OnHScroll(p1, p2, p3);
}

void CClassAndFeatView::OnVScroll(UINT p1, UINT p2, CScrollBar* p3)
{
    // call base class
    CFormView::OnVScroll(p1, p2, p3);
}

void CClassAndFeatView::UpdateClassChoiceChanged(Character * charData)
{
    // ensure control re-sizes as required
    CRect rect;
    GetClientRect(&rect);
    OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

