// DCView.cpp
//
#include "stdafx.h"
#include "DCView.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    const int c_windowSize = 38;
}

IMPLEMENT_DYNCREATE(CDCView, CFormView)

CDCView::CDCView() :
    CFormView(CDCView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_tipCreated(false),
    m_pTooltipItem(NULL),
    m_nextDcId(IDC_SPECIALFEAT_0)
{
}

CDCView::~CDCView()
{
}

void CDCView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CDCView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CDCView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDCView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CDCView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    m_tooltip.Create(this);
    m_tipCreated = true;
}

void CDCView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_dcButtons.size() > 0)
    {
        CRect wndClient;
        GetClientRect(&wndClient);
        CRect itemRect(
                c_controlSpacing,
                c_controlSpacing,
                c_windowSize + c_controlSpacing,
                c_windowSize + c_controlSpacing);
        // move each DC button
        for (size_t i = 0; i < m_dcButtons.size(); ++i)
        {
            m_dcButtons[i]->MoveWindow(itemRect, TRUE);
            // move rectangle across for next set of controls
            itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
            if (itemRect.right > (wndClient.right - c_controlSpacing))
            {
                // oops, not enough space in client area here
                // move down and start the next row of controls
                itemRect -= CPoint(itemRect.left, 0);
                itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
            }
        }
    }
    for (size_t i = 0; i < m_dcButtons.size(); ++i)
    {
        m_dcButtons[i]->Invalidate(TRUE);
    }
    SetScaleToFitSize(CSize(cx, cy));
}

LRESULT CDCView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
        DestroyAllDCs();
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
    return 0L;
}

BOOL CDCView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CDCView::UpdateNewDC(Character * charData, const DC & dc)
{
    AddDC(dc);
}

void CDCView::UpdateRevokeDC(Character * charData, const DC & dc)
{
    RevokeDC(dc);
}

void CDCView::AddDC(const DC & dc)
{
    // only add the stance if it is not already present
    bool found = false;
    for (size_t i = 0; i < m_dcButtons.size(); ++i)
    {
        if (m_dcButtons[i]->IsYou(dc))
        {
            found = true;
            m_dcButtons[i]->AddStack();
        }
    }
    if (!found)
    {
        // position the created windows left to right until
        // they don't fit then move them down a row and start again
        // each stance window is c_windowSize * c_windowSize pixels
        CRect wndClient;
        GetClientRect(&wndClient);
        CRect itemRect(
                c_controlSpacing,
                c_controlSpacing,
                c_windowSize + c_controlSpacing,
                c_windowSize + c_controlSpacing);

        // now create the new user DC control
        m_dcButtons.push_back(new CDCButton(m_pCharacter, dc));
        // create a parent window that is c_windowSize by c_windowSize pixels in size
        m_dcButtons.back()->Create(
                "",
                WS_CHILD | WS_VISIBLE,
                itemRect,
                this,
                m_nextDcId++);
        m_dcButtons.back()->AddStack();
        if (IsWindow(GetSafeHwnd()))
        {
            // now force an on size event to position everything
            CRect rctWnd;
            GetClientRect(&rctWnd);
            OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
        }
    }
}

void CDCView::RevokeDC(const DC & dc)
{
    // only revoke the stance if it is not already present and its the last stack of it
    for (size_t i = 0; i < m_dcButtons.size(); ++i)
    {
        if (m_dcButtons[i]->IsYou(dc))
        {
            m_dcButtons[i]->RevokeStack();
            if (m_dcButtons[i]->NumStacks() == 0)
            {
                // all instances of this stance are gone, remove the button
                m_dcButtons[i]->DestroyWindow();
                delete m_dcButtons[i];
                m_dcButtons[i] = NULL;
                // clear entries from the array
                std::vector<CDCButton *>::iterator it = m_dcButtons.begin() + i;
                m_dcButtons.erase(it);
                // now force an on size event
                CRect rctWnd;
                GetClientRect(&rctWnd);
                OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
                break;
            }
        }
    }
}

void CDCView::DestroyAllDCs()
{
    for (size_t i = 0; i < m_dcButtons.size(); ++i)
    {
        m_dcButtons[i]->DestroyWindow();
        delete m_dcButtons[i];
        m_dcButtons[i] = NULL;
    }
    m_dcButtons.clear();
}

void CDCView::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which stance the mouse may be over
    CWnd * pWnd = ChildWindowFromPoint(point);
    CDCButton * pDC = dynamic_cast<CDCButton*>(pWnd);
    if (pDC != NULL
            && pDC != m_pTooltipItem)
    {
        CRect itemRect;
        pDC->GetWindowRect(&itemRect);
        ScreenToClient(itemRect);
        // over a new item or a different item
        m_pTooltipItem = pDC;
        ShowTip(*pDC, itemRect);
    }
    else
    {
        if (m_showingTip
                && pDC != m_pTooltipItem)
        {
            // no longer over the same item
            HideTip();
        }
        // as the mouse is over the enhancement tree, ensure the status bar message prompts available actions
        GetMainFrame()->SetStatusBarPromptText("Ready.");
    }
}

LRESULT CDCView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    GetMainFrame()->SetStatusBarPromptText("Ready.");
    return 0;
}

void CDCView::ShowTip(const CDCButton & item, CRect itemRect)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    ClientToScreen(&itemRect);
    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
    CPoint tipAlternate(itemRect.left, itemRect.top - 2);
    SetTooltipText(item, tipTopLeft, tipAlternate);
    m_showingTip = true;
    // track the mouse so we know when it leaves our window
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
}

void CDCView::HideTip()
{
    // tip not shown if not over an assay
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
        m_pTooltipItem = NULL;
    }
}

void CDCView::SetTooltipText(
        const CDCButton & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetDCItem(*m_pCharacter, &item.GetDCItem());
    m_tooltip.Show();
}

const std::vector<CDCButton *> & CDCView::DCs() const
{
    return m_dcButtons;
}

