// ReaperEnhancementsView.cpp
//
#include "stdafx.h"
#include "ReaperEnhancementsView.h"

#include "DDOCP.h"
#include "EnhancementtreeDialog.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    // enhancement window size
    const size_t c_sizeX = 299;
    const size_t c_sizeY = 466;
}

IMPLEMENT_DYNCREATE(CReaperEnhancementsView, CFormView)

CReaperEnhancementsView::CReaperEnhancementsView() :
    CFormView(CReaperEnhancementsView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_scrollOffset(0)
{
}

CReaperEnhancementsView::~CReaperEnhancementsView()
{
    for (size_t vi = 0; vi < m_treeViews.size(); ++vi)
    {
        delete m_treeViews[vi];
    }
}

void CReaperEnhancementsView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_LEFT, m_buttonLeft);
    DDX_Control(pDX, IDC_BUTTON_RIGHT, m_buttonRight);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CReaperEnhancementsView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_BN_CLICKED(IDC_BUTTON_LEFT, OnButtonLeft)
    ON_BN_CLICKED(IDC_BUTTON_RIGHT, OnButtonRight)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CReaperEnhancementsView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CReaperEnhancementsView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CReaperEnhancementsView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    UpdateWindowTitle();
}

void CReaperEnhancementsView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_treeViews.size() > 0
            && IsWindow(m_treeViews[0]->GetSafeHwnd()))
    {
        // we can position and show all the visible enhancement windows
        // note that as these windows are big we may have to show buttons to allow
        // the visible windows to be scrolled left/right
        // calculate whether the scroll buttons are needed
        int requiredWidth = (MST_Number * c_sizeX)      // windows
                + (c_controlSpacing * (MST_Number + 1));   // spacers
        bool showScrollButtons = (requiredWidth > cx);      // true if buttons shown
        if (!showScrollButtons)
        {
            m_scrollOffset = 0;     // all can be fitted in, remove scroll if present
        }

        // default location of first enhancement tree if no scroll buttons present
        CRect itemRect(
                c_controlSpacing,
                c_controlSpacing,
                c_sizeX + c_controlSpacing,
                c_sizeY + c_controlSpacing);

        // handle the scroll left button. Only shown if the window is currently scrolled
        CRect rctButton;
        m_buttonLeft.GetWindowRect(&rctButton);
        rctButton -= rctButton.TopLeft();
        rctButton += CPoint(c_controlSpacing, (cy - rctButton.Height()) / 2);
        m_buttonLeft.MoveWindow(rctButton, FALSE);
        if (m_scrollOffset > 0)
        {
            // button needs to be seen
            m_buttonLeft.ShowWindow(SW_SHOW);
            m_buttonLeft.EnableWindow(TRUE);
            // also bump the first enhancement window right by width of the button
            itemRect += CPoint(rctButton.Width() + c_controlSpacing * 2, 0);
        }
        else
        {
            m_buttonLeft.ShowWindow(SW_HIDE);
            m_buttonLeft.EnableWindow(FALSE);
        }
        m_buttonRight.GetWindowRect(&rctButton);
        rctButton -= rctButton.TopLeft();
        rctButton += CPoint(
                    cx - c_controlSpacing - rctButton.Width(),
                    (cy - rctButton.Height()) / 2);
        m_buttonRight.MoveWindow(rctButton, FALSE);

        ASSERT(m_treeViews.size() == MST_Number);
        std::vector<bool> isShown(MST_Number, false);  // gets set to true when displayed
        for (size_t ti = 0; ti < m_visibleTrees.size(); ++ti)
        {
            if (showScrollButtons)
            {
                // determine whether we can fit any more tree windows
                if (itemRect.right >= rctButton.left - c_controlSpacing)
                {
                    // no more trees fit, break out and the remaining windows get hidden
                    break;
                }
            }
            if (ti >= m_scrollOffset)
            {
                size_t index = m_visibleTrees[ti];
                ASSERT(index >= 0 && index < MST_Number);
                // move the window to the correct location
                m_treeViews[index]->MoveWindow(itemRect, false);
                m_treeViews[index]->ShowWindow(SW_SHOW);        // ensure visible
                isShown[index] = true;
                // now move the rectangle to the next tree location
                itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
                if (ti == m_visibleTrees.size() - 1)
                {
                    // right scroll button no need to be visible
                    showScrollButtons = false;
                }
            }
            else
            {
                // although this item is visible, its scrolled off the left hand side
                // its isShown[] will remain false and window is auto hidden later
            }
        }
        // ensure all the hidden enhancement trees are not visible
        for (size_t ti = 0; ti < MST_Number; ++ti)
        {
            if (!isShown[ti])
            {
                // this tree is not visible
                m_treeViews[ti]->ShowWindow(SW_HIDE);
            }
        }
        // right scroll button may need to be visible
        if (showScrollButtons)
        {
            // position and show the scroll right button
            m_buttonRight.ShowWindow(SW_SHOW);
            m_buttonRight.EnableWindow(TRUE);
        }
        else
        {
            // no need to show the scroll right button
            m_buttonRight.ShowWindow(SW_HIDE);
            m_buttonRight.EnableWindow(FALSE);
        }
    }
}

LRESULT CReaperEnhancementsView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        // trees definitely change if the character has changed
        m_availableTrees = DetermineTrees();
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
    }
    else
    {
        DestroyEnhancementWindows();
    }
    UpdateWindowTitle();
    return 0L;
}

BOOL CReaperEnhancementsView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        0 // end marker
    };

    // exclude any visible enhancement view windows
    for (size_t i = 0; i < m_treeViews.size(); ++i)
    {
        if (::IsWindow(m_treeViews[i]->GetSafeHwnd())
                && ::IsWindowVisible(m_treeViews[i]->GetSafeHwnd()))
        {
            CRect rctWnd;
            m_treeViews[i]->GetWindowRect(&rctWnd);
            ScreenToClient(&rctWnd);
            pDC->ExcludeClipRect(&rctWnd);
        }
    }

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

std::vector<EnhancementTree> CReaperEnhancementsView::DetermineTrees()
{
    // the three standard reaper trees are always the ones available
    std::vector<EnhancementTree> trees;
    const std::list<EnhancementTree> & allTrees = EnhancementTrees();
    std::list<EnhancementTree>::const_iterator it = allTrees.begin();
    while (it != allTrees.end())
    {
        // get all the trees that are compatible with the race/class setup
        if ((*it).HasIsReaperTree())
        {
            // yes this is one of our tree's add it
            trees.push_back((*it));
        }
        ++it;
    }
    return trees;
}

void CReaperEnhancementsView::CreateEnhancementWindows()
{
    LockWindowUpdate();
    // all enhancement windows are a set size and max of MST_Number available to the user
    CRect itemRect(
            c_controlSpacing,
            c_controlSpacing,
            c_sizeX + c_controlSpacing,
            c_sizeY + c_controlSpacing);
    for (size_t i = 0; i < MST_Number; ++i)
    {
        // create the tree dialog
        // show an enhancement dialog
        CEnhancementTreeDialog * dlg = new CEnhancementTreeDialog(
                this,
                m_pCharacter,
                GetEnhancementTree(m_availableTrees[i].Name()),
                TT_reaper);
        dlg->Create(CEnhancementTreeDialog::IDD, this);
        dlg->MoveWindow(&itemRect);
        dlg->ShowWindow(SW_SHOW);
        m_treeViews.push_back(dlg);
        // TBD this is just so they all get shown for now
        m_visibleTrees.push_back(m_visibleTrees.size());
    }
    UnlockWindowUpdate();
    // reposition and show the windows (handled in OnSize)
    CRect rctWnd;
    GetClientRect(&rctWnd);
    OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
}

void CReaperEnhancementsView::DestroyEnhancementWindows()
{
    LockWindowUpdate();
    // the user has changed either a race or class and the available enhancement trees
    // has changed
    for (size_t i = 0; i < m_treeViews.size(); ++i)
    {
        m_treeViews[i]->DestroyWindow();
        delete m_treeViews[i];
        m_treeViews[i] = NULL;
    }
    m_treeViews.clear();
    m_visibleTrees.clear();
    // left and right scrolls buttons no longer needed, hide them
    m_buttonLeft.ShowWindow(SW_HIDE);
    m_buttonLeft.EnableWindow(FALSE);
    m_buttonRight.ShowWindow(SW_HIDE);
    m_buttonRight.EnableWindow(FALSE);
    UnlockWindowUpdate();
}

void CReaperEnhancementsView::OnButtonLeft()
{
    if (m_scrollOffset > 0)
    {
        --m_scrollOffset;
        // reposition and show the windows (handled in OnSize)
        CRect rctWnd;
        GetClientRect(&rctWnd);
        OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
        Invalidate();
    }
}

void CReaperEnhancementsView::OnButtonRight()
{
    ++m_scrollOffset;
    // reposition and show the windows (handled in OnSize)
    CRect rctWnd;
    GetClientRect(&rctWnd);
    OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
    Invalidate();
}

void CReaperEnhancementsView::UpdateWindowTitle()
{
    CString text;
    text.Format("Reaper Enhancements");
    GetParent()->SetWindowText(text);
}

