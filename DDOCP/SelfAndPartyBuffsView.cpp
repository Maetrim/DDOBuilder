// SelfAndPartyBuffsView.cpp
//
#include "stdafx.h"
#include "SelfAndPartyBuffsView.h"
#include "GlobalSupportFunctions.h"
#include "MouseHook.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
}

IMPLEMENT_DYNCREATE(CSelfAndPartyBuffsView, CFormView)

CSelfAndPartyBuffsView::CSelfAndPartyBuffsView() :
    CFormView(CSelfAndPartyBuffsView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_bPopulating(false),
    m_showingTip(false),
    m_tipCreated(false),
    m_hoverItem(-1),
    m_hoverHandle(0)
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
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BUFFS, OnItemchangedListBuffs)
    ON_NOTIFY(NM_HOVER, IDC_LIST_BUFFS, OnHoverListBuffs)
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
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
    m_tooltip.Create(this);
    m_tipCreated = true;

    m_listBuffs.InsertColumn(0, "Buff Name", LVCFMT_LEFT, 250, 0);

    // set the style to allow check boxes on each item
    m_listBuffs.SetExtendedStyle(m_listBuffs.GetExtendedStyle()
            | LVS_EX_TRACKSELECT
            | LVS_EX_FULLROWSELECT
            | LVS_EX_CHECKBOXES);
            //| LVS_EX_LABELTIP); // stops hover tooltips from working

    m_bPopulating = true;
    // add the items to the list
    const std::list<OptionalBuff> & buffs = OptionalBuffs();
    std::list<OptionalBuff>::const_iterator it = buffs.begin();
    while (it != buffs.end())
    {
        m_listBuffs.InsertItem(m_listBuffs.GetItemCount(), (*it).Name().c_str(), 0);
        ++it;
    }
    m_bPopulating = false;
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
        CheckSelectedItems();
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
        IDC_LIST_BUFFS,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CSelfAndPartyBuffsView::OnItemchangedListBuffs(NMHDR* pNMHDR, LRESULT* pResult)
{
    // stop control updates done by populating the control causing changes
    if (!m_bPopulating)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        BOOL oldCheckState = ((pNMListView->uOldState & LVIS_STATEIMAGEMASK)>>12) - 1;
        BOOL newCheckState = ((pNMListView->uNewState & LVIS_STATEIMAGEMASK)>>12) - 1;

        if (oldCheckState != -1
                && newCheckState != -1
                && oldCheckState != newCheckState)
        {
            CString item = m_listBuffs.GetItemText(pNMListView->iItem, 0);
            if (newCheckState == TRUE)
            {
                m_pCharacter->EnableSelfAndPartyBuff((LPCTSTR)item);
            }
            else
            {
                m_pCharacter->DisableSelfAndPartyBuff((LPCTSTR)item);
            }
        }
    }
    *pResult = 0;
}

void CSelfAndPartyBuffsView::CheckSelectedItems()
{
    if (IsWindow(m_listBuffs.GetSafeHwnd()))
    {
        // set the check state of all the items selected for this character
        m_bPopulating = true;
        const std::list<std::string> & enabledBuffs = m_pCharacter->EnabledSelfAndPartyBuffs();
        for (size_t i = 0; i < (size_t)m_listBuffs.GetItemCount(); ++i)
        {
            CString item = m_listBuffs.GetItemText(i, 0);
            // see if its in our list of activated buffs
            bool bActive = false;
            std::list<std::string>::const_iterator it = enabledBuffs.begin();
            while (!bActive && it != enabledBuffs.end())
            {
                if ((*it) == (LPCTSTR)item)
                {
                    // found it, its active
                    bActive = true;
                }
                else
                {
                    ++it;
                }
            }
            // set / clear the check mark state of this item to match
            BOOL enabled = bActive ? TRUE : FALSE;
            m_listBuffs.SetItemState(i, INDEXTOSTATEIMAGEMASK(enabled + 1), LVIS_STATEIMAGEMASK);
        }
        m_bPopulating = false;
    }
}

void CSelfAndPartyBuffsView::OnHoverListBuffs(NMHDR* pNMHDR, LRESULT* pResult)
{
    // the user it hovering over a list control item. Identify it and display
    // the item tooltip for this item
    CPoint mousePosition;
    GetCursorPos(&mousePosition);
    m_listBuffs.ScreenToClient(&mousePosition);

    LVHITTESTINFO hitInfo;
    hitInfo.pt = mousePosition;
    if (m_listBuffs.SubItemHitTest(&hitInfo) >= 0)
    {
        if (m_hoverItem != hitInfo.iItem)
        {
            // the item under the hover has changed
            m_hoverItem = hitInfo.iItem;
            // mouse is over a valid item, get the items rectangle and
            // show the item tooltip
            CRect rect;
            m_listBuffs.GetItemRect(hitInfo.iItem, &rect, LVIR_BOUNDS);
            HideTip();
            // get the item text
            CString name = m_listBuffs.GetItemText(hitInfo.iItem, 0);
            m_listBuffs.ClientToScreen(&rect);
            ShowTip((LPCTSTR)name, rect);
        }
    }
    *pResult = 1;   // stop the hover selecting the item
}

LRESULT CSelfAndPartyBuffsView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    if (wParam == m_hoverHandle)
    {
        // no longer monitoring this location
        GetMouseHook()->DeleteRectangleToMonitor(m_hoverHandle);
        m_hoverHandle = 0;
        m_hoverItem = -1;
    }
    return 0;
}

void CSelfAndPartyBuffsView::ShowTip(const std::string & name, CRect itemRect)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
    CPoint tipAlternate(itemRect.left, itemRect.top - 2);
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetSelfBuff(name);
    m_tooltip.Show();
    m_showingTip = true;
    // make sure we don't stack multiple monitoring of the same rectangle
    if (m_hoverHandle == 0)
    {
        m_hoverHandle = GetMouseHook()->AddRectangleToMonitor(
                this->GetSafeHwnd(),
                itemRect,           // screen coordinates,
                WM_MOUSEENTER,
                WM_MOUSELEAVE,
                true);
    }
}

void CSelfAndPartyBuffsView::HideTip()
{
    // tip not shown if not over a gear slot
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
    }
}
