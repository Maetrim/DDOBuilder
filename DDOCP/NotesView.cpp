// NotesView.cpp
//
#include "stdafx.h"
#include "NotesView.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
}

IMPLEMENT_DYNCREATE(CNotesView, CFormView)

CNotesView::CNotesView() :
    CFormView(CNotesView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL)
{
}

CNotesView::~CNotesView()
{
}

void CNotesView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NOTES, m_editNotes);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CNotesView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_EN_CHANGE(IDC_NOTES, OnNotesChanged)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CNotesView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CNotesView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CNotesView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    m_editNotes.EnableWindow(FALSE);
}

void CNotesView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (IsWindow(m_editNotes.GetSafeHwnd()))
    {
        // control uses full window size
        m_editNotes.MoveWindow(0, 0, cx, cy);
    }
    SetScaleToFitSize(CSize(cx, cy));
}

LRESULT CNotesView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        m_editNotes.SetWindowText(m_pCharacter->Notes().c_str());
    }
    else
    {
        m_editNotes.SetWindowText("");
    }
    if (IsWindow(m_editNotes.GetSafeHwnd()))
    {
        m_editNotes.EnableWindow(m_pCharacter != NULL);
    }
    return 0L;
}

BOOL CNotesView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_NOTES,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CNotesView::OnNotesChanged()
{
    // update the character with any changes to the notes
    CString text;
    m_editNotes.GetWindowText(text);
    m_pCharacter->SetNotes((LPCTSTR)text);
}

