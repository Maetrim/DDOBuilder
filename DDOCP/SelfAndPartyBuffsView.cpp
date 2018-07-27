// SelfAndPartyBuffsView.cpp
//
#include "stdafx.h"
#include "SelfAndPartyBuffsView.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    const int c_windowSize = 38;
}

IMPLEMENT_DYNCREATE(CSelfAndPartyBuffsView, CFormView)

CSelfAndPartyBuffsView::CSelfAndPartyBuffsView() :
    CFormView(CSelfAndPartyBuffsView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL)
{
}

CSelfAndPartyBuffsView::~CSelfAndPartyBuffsView()
{
}

void CSelfAndPartyBuffsView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_BUFFS, m_listBuffs);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSelfAndPartyBuffsView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CSelfAndPartyBuffsView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSelfAndPartyBuffsView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CSelfAndPartyBuffsView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    m_listBuffs.InsertColumn(0, "Buff Name", LVCFMT_LEFT, 250, 0);

    // set the style to allow check boxes on each item
    m_listBuffs.SetExtendedStyle(m_listBuffs.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_INFOTIP);

    // add the items to the list
    const std::list<OptionalBuff> & buffs = OptionalBuffs();
    std::list<OptionalBuff>::const_iterator it = buffs.begin();
    while (it != buffs.end())
    {
        m_listBuffs.InsertItem(m_listBuffs.GetItemCount(), (*it).Name().c_str(), 0);
        ++it;
    }
}

void CSelfAndPartyBuffsView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (IsWindow(m_listBuffs.GetSafeHwnd()))
    {
        // control uses full window size
        m_listBuffs.MoveWindow(0, 0, cx, cy);
        m_listBuffs.SetColumnWidth(0, cx
                - GetSystemMetrics(SM_CYBORDER) * 4     // control border
                - GetSystemMetrics(SM_CYHSCROLL));      // vertical scrollbar
    }
    SetScaleToFitSize(CSize(cx, cy));
}

LRESULT CSelfAndPartyBuffsView::OnNewDocument(WPARAM wParam, LPARAM lParam)
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
    if (IsWindow(m_listBuffs.GetSafeHwnd()))
    {
        m_listBuffs.EnableWindow(m_pCharacter != NULL);
    }
    return 0L;
}

BOOL CSelfAndPartyBuffsView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}



