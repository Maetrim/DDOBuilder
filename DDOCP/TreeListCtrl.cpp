// TreeListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListCtrl.h"
#include "MemoryDC.h"

using MfcControls::CTreeListCtrl;

namespace
{
    const int c_headerControlId = 1;
    const int c_treeControlId = 2;
    const int f_noWidthSetup = -1;
    const char * f_registrySection = "BreakdownsView";
}
/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl

const UINT UWM_AUTO_RESIZE_COL = ::RegisterWindowMessage(UWM_AUTO_RESIZE_COLUMN);

CTreeListCtrl::CTreeListCtrl(bool hasButtons, bool linesAtRoot) :
    m_hasButtons(hasButtons),
    m_linesAtRoot(linesAtRoot),
    m_showHorzScrollBar(false)
{
}

CTreeListCtrl::~CTreeListCtrl()
{
}

BEGIN_MESSAGE_MAP(CTreeListCtrl, CWnd)
    //{{AFX_MSG_MAP(CTreeListCtrl)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_HSCROLL()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_WM_SYSCOLORCHANGE()
    //}}AFX_MSG_MAP
    ON_NOTIFY(HDN_ENDTRACK, c_headerControlId, OnEndtrackHeader)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, c_headerControlId, OnEndtrackHeader)
    ON_REGISTERED_MESSAGE(UWM_AUTO_RESIZE_COL, OnAutoResizeCol)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl message handlers

BOOL CTreeListCtrl::LockWindowUpdate()
{
    return m_Tree.LockWindowUpdate();
}

void CTreeListCtrl::UnlockWindowUpdate()
{
    m_Tree.UnlockWindowUpdate();
}

BOOL CTreeListCtrl::Create(
        LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName,
        DWORD dwStyle,
        const RECT& rect,
        CWnd* pParentWnd,
        UINT nID,
        CCreateContext* pContext)
{
    BOOL ret = CWnd::Create(
            NULL,
            lpszWindowName,
            dwStyle | WS_HSCROLL | SBS_SIZEBOX,
            rect,
            pParentWnd,
            nID,
            pContext);
    if (ret)
    {
        // create the child windows
        m_Header.Create(
                WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ,
                CRect(0, 0, 10, 10),
                this,
                c_headerControlId);
        DWORD style = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_SHOWSELALWAYS;
        if (m_hasButtons)
        {
            style |= TVS_HASBUTTONS;
        }
        if (m_linesAtRoot)
        {
            style |= TVS_LINESATROOT;
        }
        m_Tree.Create(
                style,
                CRect(0, 0, 10, 10),
                this,
                c_treeControlId);
        m_Tree.SetHeaderCtrl(&m_Header);
        m_Header.SetDlgCtrlID(c_headerControlId);
        SetScrollRange(SB_HORZ, 0, 100);
        SetScrollPos(SB_HORZ, 0);
    }
    Initialise();              // should be implemented in base classes
    return ret;
}

BOOL CTreeListCtrl::CreateEx(
        DWORD dwExStyle,
        LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName,
        DWORD dwStyle,
        const RECT& rect,
        CWnd* pParentWnd,
        UINT nID,
        CCreateContext* pContext)
{
    BOOL ret = CWnd::CreateEx(
            dwExStyle,
            lpszClassName,
            lpszWindowName,
            dwStyle | WS_HSCROLL | SBS_SIZEBOX,
            rect,
            pParentWnd,
            nID,
            pContext);
    if (ret)
    {
        // create the child windows
        m_Header.Create(
                WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ,
                CRect(0, 0, 10, 10),
                this,
                c_headerControlId);
        DWORD style = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_SHOWSELALWAYS;
        if (m_hasButtons)
        {
            style |= TVS_HASBUTTONS;
        }
        if (m_linesAtRoot)
        {
            style |= TVS_LINESATROOT;
        }
        m_Tree.Create(
                style,
                CRect(0, 0, 10, 10),
                this,
                c_treeControlId);
        m_Tree.SetHeaderCtrl(&m_Header);
        m_Header.SetDlgCtrlID(c_headerControlId);
        SetScrollRange(SB_HORZ, 0, 100);
        SetScrollPos(SB_HORZ, 0);
    }
    Initialise();              // should be implemented in base classes
    return ret;
}

BOOL CTreeListCtrl::EnableWindow(BOOL bEnable)
{
    m_Header.EnableWindow(bEnable);
    m_Tree.EnableWindow(bEnable);
    m_Tree.SetBkColor(GetSysColor(bEnable ? COLOR_WINDOW: COLOR_3DFACE)); // enabled / disabled colour
    return CWnd::EnableWindow(bEnable);
}

void CTreeListCtrl::Initialise()
{
    // Don't need to do anything, override for implementation.
}

void CTreeListCtrl::OnDestroy()
{
    m_Tree.DestroyWindow();
    m_Header.DestroyWindow();
    CWnd::OnDestroy();
}

void CTreeListCtrl::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    ResizeControls(cx, cy);
}

void CTreeListCtrl::OnSetFocus(CWnd* pOldWnd)
{
    m_Tree.SetFocus();
}

void CTreeListCtrl::OnEndtrackHeader(NMHDR* hdr, LRESULT* pResult)
{
    // reflect this down to the tree control
    m_Tree.HeaderTracked();
    // re-size ourselves such that scroll bars / etc will appear / disappear as required
    CRect rect;
    GetWindowRect(&rect);

    NMHDR header;
    header.hwndFrom = GetSafeHwnd();
    header.idFrom = GetDlgCtrlID();
    header.code = HDN_ENDTRACK;

    GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&header);

    PostMessage(WM_SIZE, SIZE_RESTORED, MAKELONG(rect.Width(), rect.Height()));
    *pResult = 0;
}

void CTreeListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar*)
{
    CRect m_treeRect;

    m_Tree.GetClientRect(&m_treeRect);

    // if horizontal scroll bar
    int nCurPos = GetScrollPos(SB_HORZ);
    int nPrevPos = nCurPos;
    // decide what to do for each different scroll event
    switch(nSBCode)
    {
        case SB_LEFT:
            nCurPos = 0;
            break;
        case SB_RIGHT:
            nCurPos = GetScrollLimit(SB_HORZ)-1;
            break;
        case SB_LINELEFT:
            nCurPos = max(nCurPos-6, 0);
            break;
        case SB_LINERIGHT:
            nCurPos = min(nCurPos+6, GetScrollLimit(SB_HORZ)-1);
            break;
        case SB_PAGELEFT:
            nCurPos = max(nCurPos-m_treeRect.Width(), 0);
            break;
        case SB_PAGERIGHT:
            nCurPos = min(nCurPos+m_treeRect.Width(), GetScrollLimit(SB_HORZ)-1);
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            if (nPos == 0)
            {
                nCurPos = 0;
            }
            else
            {
                nCurPos = min(StretchWidth(nPos, 6), GetScrollLimit(SB_HORZ)-1);
            }
            break;
    }

    SetScrollPos(SB_HORZ, nCurPos);
    m_Tree.m_nOffset = -nCurPos;

    // smoothly scroll the tree control
    CRect m_scrollRect;
    m_Tree.GetClientRect(&m_scrollRect);
    m_Tree.ScrollWindow(nPrevPos - nCurPos, 0, &m_scrollRect, &m_scrollRect);
    // move the header control as well
    CRect rctHeader;
    m_Header.GetClientRect(&rctHeader);
    rctHeader += CPoint(0, 1);     // correct position
    rctHeader.left = -nCurPos;
    m_Header.MoveWindow(rctHeader);
}

int CTreeListCtrl::StretchWidth(int m_nWidth, int m_nMeasure)
{
    return ((m_nWidth/m_nMeasure)+1)*m_nMeasure;//put the fixed for +1 in brackets f/0 error
}

int CTreeListCtrl::GetColumnsNum()
{
    return m_Tree.GetColumnsNum();
}

int CTreeListCtrl::GetColumnsWidth()
{
    return m_Tree.GetColumnsWidth();
}

void CTreeListCtrl::SetColumnWidth(int column, int width)
{
    m_Header.SetColumnWidth(column, width);
}

void CTreeListCtrl::AutoResizeColumn(int nCol)
{
    m_Tree.AutoResizeColumn(nCol);
}

int CTreeListCtrl::GetItemCount()
{
    return m_Tree.GetItemCount();
}

void CTreeListCtrl::RecalcColumnsWidth()
{
    m_Tree.RecalcColumnsWidth();
}

HTREEITEM CTreeListCtrl::GetTreeItem(int nItem)
{
    return m_Tree.GetTreeItem(nItem);
}

int CTreeListCtrl::GetListItem(HTREEITEM hItem)
{
    return m_Tree.GetListItem(hItem);
}

int CTreeListCtrl::InsertColumn(
        int nCol,
        LPCTSTR lpszColumnHeading,
        int nFormat,
        int nWidth,
        int nSubItem)
{
    return m_Tree.InsertColumn(
            nCol,
            lpszColumnHeading,
            nFormat,
            nWidth,
            nSubItem);
}

int CTreeListCtrl::GetColumnWidth(int nCol)
{
    return m_Tree.GetColumnWidth(nCol);
}

int CTreeListCtrl::GetColumnAlign(int nCol)
{
    return m_Tree.GetColumnAlign(nCol);
}

BOOL CTreeListCtrl::SetItemData(HTREEITEM hItem, DWORD dwData)
{
    return m_Tree.SetItemData(hItem, dwData);
}

DWORD CTreeListCtrl::GetItemData(HTREEITEM hItem) const
{
    return m_Tree.GetItemData(hItem);
}

CString CTreeListCtrl::GetItemText(HTREEITEM hItem, int nSubItem)
{
    return m_Tree.GetItemText(hItem, nSubItem);
}

HTREEITEM CTreeListCtrl::InsertItem(
        LPCTSTR lpszItem,
        int nImage,
        int nSelectedImage,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter)
{
    return m_Tree.InsertItem(
            lpszItem,
            nImage,
            nSelectedImage,
            hParent,
            hInsertAfter);
}

HTREEITEM CTreeListCtrl::InsertItem(
        LPCTSTR lpszItem,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter)
{
    return m_Tree.InsertItem(lpszItem, hParent, hInsertAfter);
}

HTREEITEM CTreeListCtrl::InsertItem(
        UINT nMask,
        LPCTSTR lpszItem,
        int nImage,
        int nSelectedImage,
        UINT nState,
        UINT nStateMask,
        LPARAM lParam,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter)
{
    return m_Tree.InsertItem(
            nMask,
            lpszItem,
            nImage,
            nSelectedImage,
            nState,
            nStateMask,
            lParam,
            hParent,
            hInsertAfter);
}

HTREEITEM CTreeListCtrl::CopyItem(
        HTREEITEM hItem,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter)
{
    return m_Tree.CopyItem(
            hItem,
            hParent,
            hInsertAfter);
}

HTREEITEM CTreeListCtrl::MoveItem(
        HTREEITEM hItem,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter)
{
    return m_Tree.MoveItem(hItem, hParent, hInsertAfter);
}

BOOL CTreeListCtrl::DeleteSubItems(HTREEITEM hItem)
{
    return m_Tree.DeleteSubItems(hItem);
}

BOOL CTreeListCtrl::DeleteItem(HTREEITEM hItem)
{
    return m_Tree.DeleteItem(hItem);
}

BOOL CTreeListCtrl::DeleteItem(int nItem)
{
    return m_Tree.DeleteItem(nItem);
}

void CTreeListCtrl::MemDeleteAllItems(HTREEITEM hParent)
{
    m_Tree.MemDeleteAllItems(hParent);
}

BOOL CTreeListCtrl::DeleteAllItems()
{
    return m_Tree.DeleteAllItems();
}

BOOL CTreeListCtrl::SetItemText(HTREEITEM hItem, int nCol ,LPCTSTR lpszItem)
{
    return m_Tree.SetItemText(hItem, nCol, lpszItem);
}

BOOL CTreeListCtrl::SetItemColor(
        HTREEITEM hItem,
        COLORREF m_newColor,
        BOOL m_bInvalidate)
{
    return m_Tree.SetItemColor(hItem, m_newColor, m_bInvalidate);
}

BOOL CTreeListCtrl::SetItemBold(HTREEITEM hItem, BOOL m_Bold, BOOL m_bInvalidate)
{
    return m_Tree.SetItemBold(hItem, m_Bold, m_bInvalidate);
}

BOOL CTreeListCtrl::SortItems(int nCol, BOOL bAscending, HTREEITEM low)
{
    return m_Tree.SortItems(nCol, bAscending, low);
}

COLORREF CTreeListCtrl::SetBkColor(COLORREF clr)
{
    COLORREF col = m_Tree.GetBkColor();
    m_Tree.SetBkColor(clr);
    return col;

}

void CTreeListCtrl::GetTreeHierarchy(
        HTREEITEM hTItem,
        CString &cstrTreeHierarchy,
        const BOOL bTopToBottom)
{
    m_Tree.GetTreeHierarchy(hTItem, cstrTreeHierarchy, bTopToBottom);
}

HTREEITEM CTreeListCtrl::GetChildPositionAt(HTREEITEM HTParentItem, int iChildItem)
{
    return m_Tree.GetChildPositionAt(HTParentItem, iChildItem);
}

HTREEITEM CTreeListCtrl::GetRootItem()
{
    return m_Tree.GetRootItem();
}

BOOL CTreeListCtrl::Expand(HTREEITEM hItem, UINT nCode)
{
    return m_Tree.Expand(hItem, nCode);
}

CString CTreeListCtrl::GetNextLine(CString &message)
{
    return m_Tree.GetNextLine(message);
}

BOOL CTreeListCtrl::GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly) const
{
    BOOL ret = m_Tree.GetItemRect(hItem, lpRect, bTextOnly);
    // take any horizontal scrolling into account
    OffsetRect(lpRect, m_Tree.m_nOffset, 0);
    return ret;
}

BOOL CTreeListCtrl::GetSubItemRect(HTREEITEM hItem, int columnIndex, LPRECT lpRect) const
{
    CRect rect;
    BOOL ret = m_Tree.GetItemRect(hItem, &rect, FALSE);
    if (ret)
    {
        // item exists, now cut down total rectangle to item rectangle
        if (m_Header.GetItemCount() > columnIndex)
        {
            CRect headerRect;
            m_Header.GetItemRect(0, &headerRect);
            m_Header.GetItemRect(columnIndex, &headerRect);
            lpRect->left = headerRect.left;
            lpRect->top = rect.top;
            lpRect->right = headerRect.right;
            lpRect->bottom = rect.bottom;
            // take any horizontal scrolling into account
            OffsetRect(lpRect, m_Tree.m_nOffset, 0);
        }
        else
        {
            ret = FALSE;    // column doesn't exist, although the item does
        }
    }
    return ret;
}

UINT CTreeListCtrl::GetCount() const
{
    return m_Tree.GetCount();
}

UINT CTreeListCtrl::GetIndent() const
{
    return m_Tree.GetIndent();
}

void CTreeListCtrl::SetIndent(UINT nIndent)
{
    m_Tree.SetIndent(nIndent);
}

CImageList* CTreeListCtrl::GetImageList(UINT nImageList) const
{
    return m_Tree.GetImageList(nImageList);
}

CImageList* CTreeListCtrl::SetImageList(CImageList* pImageList, int nImageListType)
{
    return m_Tree.SetImageList(pImageList, nImageListType);
}

HTREEITEM CTreeListCtrl::GetNextItem(HTREEITEM hItem, UINT nCode) const
{
    return m_Tree.GetNextItem(hItem, nCode);
}

HTREEITEM CTreeListCtrl::GetChildItem(HTREEITEM hItem) const
{
    return m_Tree.GetChildItem(hItem);
}

HTREEITEM CTreeListCtrl::GetNextSiblingItem(HTREEITEM hItem) const
{
    return m_Tree.GetNextSiblingItem(hItem);
}

HTREEITEM CTreeListCtrl::GetPrevSiblingItem(HTREEITEM hItem) const
{
    return m_Tree.GetPrevSiblingItem(hItem);
}

HTREEITEM CTreeListCtrl::GetParentItem(HTREEITEM hItem) const
{
    return m_Tree.GetParentItem(hItem);
}

HTREEITEM CTreeListCtrl::GetFirstVisibleItem() const
{
    return m_Tree.GetFirstVisibleItem();
}

HTREEITEM CTreeListCtrl::GetNextVisibleItem(HTREEITEM hItem) const
{
    return m_Tree.GetNextVisibleItem(hItem);
}

HTREEITEM CTreeListCtrl::GetPrevVisibleItem(HTREEITEM hItem) const
{
    return m_Tree.GetPrevVisibleItem(hItem);
}

HTREEITEM CTreeListCtrl::GetSelectedItem() const
{
    return m_Tree.GetSelectedItem();
}

HTREEITEM CTreeListCtrl::GetDropHilightItem() const
{
    return m_Tree.GetDropHilightItem();
}

BOOL CTreeListCtrl::GetItem(TVITEM* pItem) const
{
    return m_Tree.GetItem(pItem);
}

BOOL CTreeListCtrl::GetItemImage(HTREEITEM hItem, int& nImage, int& nSelectedImage) const
{
    return m_Tree.GetItemImage(hItem, nImage, nSelectedImage);
}

UINT CTreeListCtrl::GetItemState(HTREEITEM hItem, UINT nStateMask) const
{
    return m_Tree.GetItemState(hItem, nStateMask);
}

BOOL CTreeListCtrl::SetItem(TVITEM* pItem)
{
    return m_Tree.SetItem(pItem);
}

BOOL CTreeListCtrl::SetItem(
        HTREEITEM hItem,
        UINT nMask,
        LPCTSTR lpszItem,
        int nImage,
        int nSelectedImage,
        UINT nState,
        UINT nStateMask,
        LPARAM lParam)
{
    return m_Tree.SetItem(
            hItem,
            nMask,
            lpszItem,
            nImage,
            nSelectedImage,
            nState,
            nStateMask,
            lParam);
}

BOOL CTreeListCtrl::SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage)
{
    return m_Tree.SetItemImage(hItem, nImage, nSelectedImage);
}

BOOL CTreeListCtrl::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask)
{
    return m_Tree.SetItemState(hItem, nState, nStateMask);
}

BOOL CTreeListCtrl::ItemHasChildren(HTREEITEM hItem) const
{
    return m_Tree.ItemHasChildren(hItem);
}

CEdit* CTreeListCtrl::GetEditControl() const
{
    return m_Tree.GetEditControl();
}

UINT CTreeListCtrl::GetVisibleCount() const
{
    return m_Tree.GetVisibleCount();
}

CToolTipCtrl* CTreeListCtrl::GetToolTips() const
{
    return m_Tree.GetToolTips();
}

CToolTipCtrl* CTreeListCtrl::SetToolTips(CToolTipCtrl* pWndTip)
{
    return m_Tree.SetToolTips(pWndTip);
}

COLORREF CTreeListCtrl::GetBkColor() const
{
    return m_Tree.GetBkColor();
}

SHORT CTreeListCtrl::GetItemHeight() const
{
    return m_Tree.GetItemHeight();
}

SHORT CTreeListCtrl::SetItemHeight(SHORT cyHeight)
{
    return m_Tree.SetItemHeight(cyHeight);
}

COLORREF CTreeListCtrl::GetTextColor() const
{
    return m_Tree.GetTextColor();
}

COLORREF CTreeListCtrl::SetTextColor(COLORREF clr)
{
    return m_Tree.SetTextColor(clr);
}

BOOL CTreeListCtrl::SetInsertMark(HTREEITEM hItem, BOOL fAfter)
{
    return m_Tree.SetInsertMark(hItem, fAfter);
}

BOOL CTreeListCtrl::GetCheck(HTREEITEM hItem) const
{
    return m_Tree.GetCheck(hItem);
}

BOOL CTreeListCtrl::SetCheck(HTREEITEM hItem, BOOL fCheck)
{
    return m_Tree.SetCheck(hItem, fCheck);
}

COLORREF CTreeListCtrl::GetInsertMarkColor() const
{
    return m_Tree.GetInsertMarkColor();
}

COLORREF CTreeListCtrl::SetInsertMarkColor(COLORREF clrNew)
{
    return m_Tree.SetInsertMarkColor(clrNew);
}

BOOL CTreeListCtrl::Select(HTREEITEM hItem, UINT nCode)
{
    return m_Tree.Select(hItem, nCode);
}

BOOL CTreeListCtrl::SelectItem(HTREEITEM hItem)
{
    BOOL ret = m_Tree.SelectItem(hItem);
    NMHDR nmHdr;
    nmHdr.hwndFrom = GetSafeHwnd();
    nmHdr.idFrom = GetDlgCtrlID();
    nmHdr.code = TVN_SELCHANGED;
    WPARAM wParam = GetDlgCtrlID();
    LPARAM lParam = (LPARAM)(void*)&nmHdr;
    GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
    return ret;
}

BOOL CTreeListCtrl::SelectDropTarget(HTREEITEM hItem)
{
    return m_Tree.SelectDropTarget(hItem);
}

BOOL CTreeListCtrl::SelectSetFirstVisible(HTREEITEM hItem)
{
    return m_Tree.SelectSetFirstVisible(hItem);
}

CEdit* CTreeListCtrl::EditLabel(HTREEITEM hItem)
{
    return m_Tree.EditLabel(hItem);
}

HTREEITEM CTreeListCtrl::HitTest(CPoint pt, UINT* pFlags) const
{
    return m_Tree.HitTest(pt, pFlags);
}

HTREEITEM CTreeListCtrl::HitTest(TVHITTESTINFO* pHitTestInfo) const
{
    return m_Tree.HitTest(pHitTestInfo);
}

CImageList* CTreeListCtrl::CreateDragImage(HTREEITEM hItem)
{
    return m_Tree.CreateDragImage(hItem);
}

BOOL CTreeListCtrl::SortChildren(HTREEITEM hItem)
{
    return m_Tree.SortChildren(hItem);
}

BOOL CTreeListCtrl::EnsureVisible(HTREEITEM hItem)
{
    return m_Tree.EnsureVisible(hItem);
}

BOOL CTreeListCtrl::SortChildrenCB(LPTVSORTCB pSort)
{
    return m_Tree.SortChildrenCB(pSort);
}

BOOL CTreeListCtrl::OnEraseBkgnd(CDC *  pDC)
{
    pDC->SaveDC();
    CRect controlRect;
    if (IsWindow(m_Tree.GetSafeHwnd()))
    {
        m_Tree.GetWindowRect(&controlRect);
        ScreenToClient(&controlRect);
        pDC->ExcludeClipRect(controlRect);
    }

    if (IsWindow(m_Header.GetSafeHwnd()))
    {
        m_Header.GetWindowRect(&controlRect);
        ScreenToClient(&controlRect);
        pDC->ExcludeClipRect(controlRect);
    }

    CRect clip;
    pDC->GetClipBox(&clip);
    pDC->FillSolidRect(clip, GetSysColor(COLOR_WINDOW));

    pDC->RestoreDC(-1);

    return TRUE;
}

LRESULT CTreeListCtrl::OnAutoResizeCol(WPARAM wParam, LPARAM lParam)
{
    return m_Tree.SendMessage(UWM_AUTO_RESIZE_COL, wParam, lParam);
}

BOOL CTreeListCtrl::IsPtInBitmap(CPoint pt)
{
    return (m_Tree.IsPointInBitmap(pt));
}

void CTreeListCtrl::DeleteAllChildren(HTREEITEM hItem)
{
    m_Tree.DeleteAllChildren(hItem);
}

void CTreeListCtrl::OnExpandAll()
{
    ExpandAll();
}

void CTreeListCtrl::OnUpdateExpandAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetRootItem() != NULL);
}

void CTreeListCtrl::OnExpandBranch()
{
    ExpandBranch();
}

void CTreeListCtrl::OnUpdateExpandBranch(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetRootItem() != NULL);
}

void CTreeListCtrl::OnCollapseAll()
{
    CollapseAll();
}

void CTreeListCtrl::OnUpdateCollapseAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetRootItem() != NULL);
}

void CTreeListCtrl::OnCollapseBranch()
{
    CollapseBranch();
}

void CTreeListCtrl::OnUpdateCollapseBranch(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetRootItem() != NULL);
}

void CTreeListCtrl::ExpandAll()
{
    HTREEITEM hItem = GetRootItem();
    while (hItem)
    {
        m_Tree.ExpandBranch(hItem);
        hItem = GetNextSiblingItem(hItem);
    }
}

void CTreeListCtrl::CollapseAll()
{
    HTREEITEM hItem = GetRootItem();
    while (hItem)
    {
        Expand(hItem, TVE_COLLAPSE);
        hItem = GetNextSiblingItem(hItem);
    }
}

void CTreeListCtrl::ExpandBranch()
{
    m_Tree.ExpandBranch(GetSelectedItem());
}

void CTreeListCtrl::CollapseBranch()
{
    Expand(GetSelectedItem(), TVE_COLLAPSE);
}

// Resizes the controls based on the given width and the given height
void CTreeListCtrl::ResizeControls(int cx, int cy)
{
    if (m_Header.m_hWnd == NULL)
        return;

    // position all the controls in our client area
    CRect rctHeader(0, 0, 0, 0);
    CRect rctTree(0, 0, 0, 0);
    int nHeaderHeight = 0;

    if (m_Header.GetStyle() & WS_VISIBLE)
    {
        // get the rectangle of the header control
        CDC *pDC = m_Header.GetDC();
        CSize textSize;
        textSize = pDC->GetTextExtent("A");
        m_Header.ReleaseDC(pDC);
        nHeaderHeight = textSize.cy + 2;

        rctHeader.left = m_Tree.m_nOffset; // keep scrolling correct
        rctHeader.right = cx;
        rctHeader.top = 0;
        rctHeader.bottom = nHeaderHeight + 1;
    }

    // position the tree control
    rctTree.left = 0;
    rctTree.right = cx;
    rctTree.top = rctHeader.bottom + 1;
    rctTree.bottom = cy;

    // is the horizontal scroll bar visible?
    int nWidth = 0;
    int count = m_Header.GetItemCount();
    CRect rect(0, 0, 0, 0);
    for (int i = 0; i < count; i++)
    {
        m_Header.GetItemRect(i, &rect);
        nWidth += rect.Width();
    }
    if (nWidth > cx)
    {
        // the scroll bar needs to be displayed
        m_showHorzScrollBar = true;

        int pos = GetScrollPos(SB_HORZ);
        if (pos > 0)
        {
            m_Tree.m_nOffset = cx - nWidth;
            rctHeader.left = m_Tree.m_nOffset;  // keep scrolling correct
        }
    }
    else
    {
        rctHeader.left = 0;
        m_Tree.m_nOffset = 0;
        m_showHorzScrollBar = false;
    }
    // now move all the windows
    m_Header.MoveWindow(rctHeader);
    m_Tree.MoveWindow(rctTree);

    if (m_showHorzScrollBar)
    {
        SCROLLINFO  si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_PAGE | SIF_RANGE;
        CRect clientRect;
        m_Tree.GetClientRect(&clientRect);
        int pageSize = clientRect.Width();
        si.nPage = pageSize;
        si.nMax = nWidth;
        si.nMin = 0;
        SetScrollInfo(SB_HORZ, &si);
    }
    else
    {
        SetScrollPos(SB_HORZ, 0);
    }
    ShowScrollBar(SB_HORZ, m_showHorzScrollBar);
    EnableScrollBarCtrl(SB_HORZ, m_showHorzScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
// CTLItem

using MfcControls::CTLItem;

CTLItem::CTLItem()
{
    m_cEnding = '¶';
    m_itemString = "";
    m_Bold = FALSE;
    m_Color = ::GetSysColor(COLOR_WINDOWTEXT);
}

CTLItem::CTLItem(CTLItem &copyItem)
{
    m_cEnding = copyItem.m_cEnding;
    m_itemString = copyItem.GetItemString();
    m_Bold = copyItem.m_Bold;
    m_Color = copyItem.m_Color;
    m_itemData = copyItem.m_itemData;
}

CString CTLItem::GetSubstring(int m_nSub)
{
    CString m_tmpStr("");
    int i=0;
    int nHits=0;
    int length = m_itemString.GetLength();

    while((i<length) && (nHits<=m_nSub))
    {
        if(m_itemString[i]==m_cEnding)
        {
            nHits++;
        }
        else
        if(nHits==m_nSub)
            m_tmpStr+=m_itemString[i];

        i++;
    }

    if((i>=length) && (nHits<m_nSub))
        return "";
    else
        return m_tmpStr;
}

void CTLItem::SetSubstring(int m_nSub, CString m_sText)
{
    CString m_tmpStr("");
    int i=0, nHits=0, first=0;
    int length = m_itemString.GetLength();

    while((i<length) && (nHits<=m_nSub))
    {
        if(m_itemString[i]==m_cEnding)
        {
            if(nHits!=m_nSub)
                first = i;
            nHits++;
        }

        i++;
    }

    CString m_newStr("");
    if((nHits>m_nSub) || ((nHits==m_nSub) && (i>=length)))
    {
        // insert in the middle
        if(first!=0)
        {
            m_newStr = m_itemString.Left(first);
            m_newStr += m_cEnding;
        }
        m_newStr += m_sText;
        if(i<length)
        {
            m_newStr += m_cEnding;
            m_newStr += m_itemString.Right(m_itemString.GetLength()-i);
        }

        m_itemString=m_newStr;
    }
    else
    {
        // insert at the end
        for(i=nHits;i<m_nSub;i++)
            m_itemString+=m_cEnding;
        m_itemString+=m_sText;
    }
}

void CTLItem::SetColour(COLORREF colour)
{
    m_Color = colour;
}

COLORREF CTLItem::Colour() const
{
    return m_Color;
}

void CTLItem::SetBold(bool bold)
{
    m_Bold = (bold ? TRUE : FALSE);
}

bool CTLItem::Bold() const
{
    return m_Bold == TRUE;
}

void CTLItem::SetItemData(DWORD data)
{
    m_itemData = data;
}

DWORD CTLItem::ItemData() const
{
    return m_itemData;
}

/////////////////////////////////////////////////////////////////////////////
// CTlcTree

using MfcControls::CTlcTree;

IMPLEMENT_DYNAMIC(CTlcTree , CTreeCtrl);

CTlcTree::CTlcTree()
{
    m_nColumns = m_nColumnsWidth = 0;
    m_nOffset = 0;
    m_selectedItem=NULL;
    m_wndColor = GetSysColor(COLOR_WINDOW);
    m_pHeader = NULL;
    m_nItems = 0;
    m_nHoverTimerID = 0;
}

CTlcTree::~CTlcTree()
{
}

BEGIN_MESSAGE_MAP(CTlcTree, CTreeCtrl)
    //{{AFX_MSG_MAP(CTlcTree)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    //}}AFX_MSG_MAP
    ON_REGISTERED_MESSAGE(UWM_AUTO_RESIZE_COL, OnAutoResizeCol)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTlcTree message handlers

BOOL CTlcTree::OnEraseBkgnd(CDC *  pDC)
{
    pDC->SaveDC();

    CRect clip;
    pDC->GetClipBox(&clip);
    pDC->FillSolidRect(clip, GetSysColor(COLOR_WINDOW));

    pDC->RestoreDC(-1);

    return TRUE;
}

int CTlcTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

HTREEITEM CTlcTree::GetTreeItem(int nItem)
{
    HTREEITEM hItem = GetRootItem();
    int nCount = 0;

    while (hItem != NULL && nCount < nItem)
    {
        nCount++;
        hItem = GetNextTreeItem(hItem);
    }

    return hItem;
}

HTREEITEM CTlcTree::GetNextTreeItem(HTREEITEM hItem) const
{
    HTREEITEM hNext = NULL;
    // we return the next HTEEITEM for a tree such as:
    // Root (1)
    //      Child1 (2)
    //          GrandChild (3)
    //              Great-Grandchild(4)
    //          GrandChild (5)
    //      Child2 (6)
    // Item (7)

    // has this item got any children
    if (ItemHasChildren(hItem))
    {
        // next item is its first child
        hNext = GetNextItem(hItem, TVGN_CHILD);
    }
    else if (GetNextItem(hItem, TVGN_NEXT) != NULL)
    {
        // the next item at this level
        hNext = GetNextItem(hItem, TVGN_NEXT);
    }
    else
    {
        // return the next item after our parent (or our parents parent etc)
        while (hItem != NULL)
        {
            hItem = GetParentItem(hItem);
            if (GetNextItem(hItem, TVGN_NEXT) != NULL)
            {
                hNext = GetNextItem(hItem, TVGN_NEXT);
                break;
            }
        }
    }
    return hNext;
}

int CTlcTree::GetListItem(HTREEITEM hItem)
{
    HTREEITEM m_ParentItem = GetRootItem();
    int m_nCount = 0;

    while((m_ParentItem!=NULL) && (m_ParentItem!=hItem))
    {
        m_nCount ++;
        m_ParentItem = GetNextSiblingItem(m_ParentItem);
    }

    return m_nCount;
}

int CTlcTree::InsertColumn(
        int nCol,
        LPCTSTR lpszColumnHeading,
        int nFormat,
        int nWidth,
        int)
{
    HD_ITEM hdi;
    hdi.mask = HDI_TEXT | HDI_FORMAT;
    if(nWidth!=-1)
    {
        hdi.mask |= HDI_WIDTH;
        hdi.cxy = nWidth;
    }

    hdi.pszText = (LPTSTR)lpszColumnHeading;
    hdi.fmt = HDF_OWNERDRAW;

    if(nFormat == LVCFMT_RIGHT)
        hdi.fmt |= HDF_RIGHT;
    else
    if(nFormat == LVCFMT_CENTER)
        hdi.fmt |= HDF_CENTER;
    else
        hdi.fmt |= HDF_LEFT;

    m_nColumns ++;

    int m_nReturn = m_pHeader->InsertItem(nCol, &hdi);

    if(m_nColumns==1)
    {
        m_pHeader->SetItemImage(m_nReturn, 0);
    }

    RecalcColumnsWidth();
    SetBkColor(m_wndColor);
    return m_nReturn;
}

int CTlcTree::GetColumnWidth(int nCol)
{
    HD_ITEM hItem;
    hItem.mask = HDI_WIDTH;
    if(!m_pHeader->GetItem(nCol, &hItem))
        return 0;

    return hItem.cxy;
}

int CTlcTree::GetColumnAlign(int nCol)
{
    HD_ITEM hItem;
    hItem.mask = HDI_FORMAT;
    if(!m_pHeader->GetItem(nCol, &hItem))
        return LVCFMT_LEFT;

    if(hItem.fmt & HDF_RIGHT)
        return LVCFMT_RIGHT;
    else
    if(hItem.fmt & HDF_CENTER)
        return LVCFMT_CENTER;
    else
        return LVCFMT_LEFT;
}

void CTlcTree::RecalcColumnsWidth()
{
    m_nColumnsWidth = 0;
    for(int i=0;i<m_nColumns;i++)
        m_nColumnsWidth += GetColumnWidth(i);
}

void CTlcTree::DrawItemText(
        CDC* pDC,
        HTREEITEM hItem,
        size_t column,
        bool selected,
        CRect rect,
        int nWidth,
        int nFormat)
{
    // allow parent to do pre/post custom draw of items
    NMCUSTOMDRAW drawStage;

    drawStage.hdr.hwndFrom = GetSafeHwnd();
    drawStage.hdr.idFrom = GetDlgCtrlID();
    drawStage.hdr.code = NM_CUSTOMDRAW;
    drawStage.dwDrawStage = CDDS_PREPAINT;
    drawStage.hdc = pDC->m_hDC;
    drawStage.rc = rect;
    drawStage.dwItemSpec = (unsigned long)hItem;
    drawStage.uItemState = selected;
    drawStage.lItemlParam = column;

    LRESULT result = GetParent()->SendMessage(
            WM_NOTIFY,
            WPARAM(GetDlgCtrlID()),
            LPARAM(&drawStage));
    if (result == 0
            || result & CDRF_DODEFAULT)    // parent did not override drawing code
    {
        rect = drawStage.rc;    // parent may have modified
        UINT nStyle = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
        if (nFormat == LVCFMT_LEFT)
        {
            nStyle |= DT_LEFT;
        }
        else if (nFormat == LVCFMT_CENTER)
        {
            nStyle |= DT_CENTER;
        }
        else
        {
            nStyle |= DT_RIGHT;
        }

        CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
        CString text = GetItemText(hItem, column);
        pDC->DrawText(text, rect, nStyle);

        drawStage.dwDrawStage = CDDS_POSTPAINT;
        GetParent()->SendMessage(
                WM_NOTIFY,
                WPARAM(GetDlgCtrlID()),
                LPARAM(&drawStage));
    }
}

void CTlcTree::OnPaint()
{
    if (GetUpdateRect(NULL)) // do we have an update rectangle?
    {
        RecalcColumnsWidth();

        // Note: Do not use a CPaintDC here, as it validates the client area in its constructor
        // and can cause bad screen updates when calling DefWindowProc later in the function when this
        // window is embedded in multiple control layers
        CDC * pDC = GetDC();
        pDC->SaveDC();
        {
            MfcControls::MemoryDC memDc(pDC);

            CRect rcClip;
            CRect rcClient;
            pDC->GetClipBox( &rcClip );
            // stop us drawing where the header control is
            GetClientRect(&rcClient);

            // Set clip region to be same as that in paint DC
            CRgn rgn;
            rgn.CreateRectRgnIndirect( &rcClip );
            memDc.SelectClipRgn(&rgn);
            memDc.SetViewportOrg(m_nOffset, 0);
            memDc.SetTextColor(m_wndColor);

            // First let the control do its default drawing.
            CWnd::DefWindowProc(WM_PAINT, (WPARAM)memDc.m_hDC, 0);

            HTREEITEM hItem = GetFirstVisibleItem();
            int n = GetVisibleCount(), m_nWidth;
            memDc.FillSolidRect(GetColumnWidth(0),1,rcClient.Width(),rcClient.Height(),m_wndColor);

            CTLItem *pItem;
            // the most urgent thing is to erase the labels that were drawn by the tree
            while(hItem!=NULL && n>=0)
            {
                CRect rect;
                CRect m_labelRect;
                GetItemRect( hItem, &m_labelRect, TRUE );
                GetItemRect( hItem, &rect, FALSE );

                if(GetColumnsNum()>1)
                    rect.left = min(m_labelRect.left, GetColumnWidth(0));
                else
                    rect.left = m_labelRect.left;
                rect.right = m_nColumnsWidth;
                memDc.FillSolidRect(rect.left,rect.top,rect.Width(),rect.Height(),m_wndColor);

                hItem = GetNextVisibleItem( hItem );
                n--;
            }

            // create the font
            CFont *pFontDC;
            CFont fontDC, boldFontDC;
            LOGFONT logfont;

            CFont *pFont = GetFont();
            pFont->GetLogFont( &logfont );

            fontDC.CreateFontIndirect( &logfont );
            pFontDC = memDc.SelectObject( &fontDC );

            logfont.lfWeight = 700;
            boldFontDC.CreateFontIndirect( &logfont );

            // and now let's get to the painting itself
            hItem = GetFirstVisibleItem();
            n = GetVisibleCount();
            while(hItem!=NULL && n>=0)
            {
                CRect rect;

                UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;

                if ( !(GetItemState( hItem, selflag ) & selflag ))
                {
                    memDc.SetBkMode(TRANSPARENT);

                    pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);

                    CRect m_labelRect;
                    GetItemRect( hItem, &m_labelRect, TRUE );
                    GetItemRect( hItem, &rect, FALSE );
                    if(GetColumnsNum()>1)
                        rect.left = min(m_labelRect.left, GetColumnWidth(0));
                    else
                        rect.left = m_labelRect.left;
                    rect.right = m_nColumnsWidth;


                    memDc.SetBkColor( m_wndColor );

                    memDc.SetTextColor( pItem->Colour() );

                    if(pItem->Bold())
                    {
                        memDc.SelectObject( &boldFontDC );
                    }

                    DrawItemText(
                            &memDc,
                            hItem,
                            0,
                            true,
                            CRect(rect.left+2, rect.top, GetColumnWidth(0), rect.bottom),
                            GetColumnWidth(0)-rect.left-2,
                            GetColumnAlign(0));

                    m_nWidth = 0;
                    for(int i=1;i<m_nColumns;i++)
                    {
                        m_nWidth += GetColumnWidth(i-1);
                        DrawItemText(
                                &memDc,
                                hItem,
                                i,
                                true,
                                CRect(m_nWidth, rect.top, m_nWidth+GetColumnWidth(i), rect.bottom),
                                GetColumnWidth(i),
                                GetColumnAlign(i));
                    }

                    memDc.SetTextColor(::GetSysColor (COLOR_WINDOWTEXT ));

                    if(pItem->Bold())
                    {
                        memDc.SelectObject( &fontDC );
                    }
                }
                else
                {

                    CRect m_labelRect;
                    GetItemRect( hItem, &m_labelRect, TRUE );
                    GetItemRect( hItem, &rect, FALSE );
                    if(GetColumnsNum()>1)
                        rect.left = min(m_labelRect.left, GetColumnWidth(0));
                    else
                        rect.left = m_labelRect.left;
                    rect.right = m_nColumnsWidth;


                    // If the item is selected, paint the rectangle with the system color
                    // COLOR_HIGHLIGHT
                    COLORREF m_highlightColor = ::GetSysColor (COLOR_HIGHLIGHT);
                    CBrush brush(m_highlightColor);
                    memDc.FillRect (rect, &brush);

                    // draw a dotted focus rectangle
                    memDc.DrawFocusRect (rect);
                    pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
                    memDc.SetBkColor(m_highlightColor);
                    memDc.SetTextColor(::GetSysColor (COLOR_HIGHLIGHTTEXT));

                    if(pItem->Bold())
                    {
                        memDc.SelectObject( &boldFontDC );
                    }

                    DrawItemText(
                            &memDc,
                            hItem,
                            0,
                            false,
                            CRect(rect.left+2, rect.top, GetColumnWidth(0), rect.bottom),
                            GetColumnWidth(0)-rect.left-2,
                            GetColumnAlign(0));

                    m_nWidth = 0;
                    for(int i=1;i<m_nColumns;i++)
                    {
                        m_nWidth += GetColumnWidth(i-1);
                        DrawItemText(
                                &memDc,
                                hItem,
                                i,
                                false,
                                CRect(m_nWidth, rect.top, m_nWidth+GetColumnWidth(i), rect.bottom),
                                GetColumnWidth(i),
                                GetColumnAlign(i));
                    }

                    if(pItem->Bold())
                    {
                        memDc.SelectObject( &fontDC );
                    }
                }

                hItem = GetNextVisibleItem( hItem );
                n--;
            }

            memDc.SelectObject( pFontDC );
            rgn.DeleteObject();
            memDc.SetViewportOrg(0, 0);
        }

        pDC->RestoreDC(-1); // return the DC in a pristine state
        ReleaseDC(pDC);
        ValidateRect(NULL);
    }
}

void CTlcTree::OnLButtonDown(UINT nFlags, CPoint point)
{
    CTreeCtrl::OnLButtonDown(nFlags, point);
    UINT flags;
    m_selectedItem = HitTest(point, &flags);

    if((flags & TVHT_ONITEMRIGHT) || (flags & TVHT_ONITEMINDENT) ||
       (flags & TVHT_ONITEM))
    {
        SelectItem(m_selectedItem);
        NMHDR nmHdr;
        nmHdr.hwndFrom = GetParent()->GetSafeHwnd();
        nmHdr.idFrom = GetParent()->GetDlgCtrlID();
        nmHdr.code = TVN_SELCHANGED;
        WPARAM wParam = GetParent()->GetDlgCtrlID();
        LPARAM lParam = (LPARAM)(void*)&nmHdr;
        GetParent()->GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
    }
    const MSG *pMsg = GetCurrentMessage();
    GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void CTlcTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if((GetColumnsNum()==0) || (point.x<GetColumnWidth(0)))
    {
        CTreeCtrl::OnLButtonDblClk(nFlags, point);
    }

    SetFocus();
    const MSG *pMsg = GetCurrentMessage();
    GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void CTlcTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    m_selectedItem=GetSelectedItem();
    const MSG *pMsg = GetCurrentMessage();
    GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void CTlcTree::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
    m_selectedItem=GetSelectedItem();
    const MSG *pMsg = GetCurrentMessage();
    GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

BOOL CTlcTree::SetItemData(HTREEITEM hItem, DWORD dwData)
{
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    if(!pItem)
        return FALSE;
    pItem->SetItemData(dwData);
    return CTreeCtrl::SetItemData(hItem, (LPARAM)pItem);
}

DWORD CTlcTree::GetItemData(HTREEITEM hItem) const
{
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    if(!pItem)
        return NULL;
    return pItem->ItemData();
}

HTREEITEM CTlcTree::InsertItem(
        LPCTSTR lpszItem,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter)
{
    CTLItem *pItem = new CTLItem;
    pItem->InsertItem(lpszItem);
    m_nItems++;

    return CTreeCtrl::InsertItem(
            TVIF_PARAM|TVIF_TEXT,
            "",
            0,
            0,
            0,
            0,
            (LPARAM)pItem,
            hParent,
            hInsertAfter);
}

HTREEITEM CTlcTree::InsertItem(
        LPCTSTR lpszItem,
        int nImage,
        int nSelectedImage,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter)
{
    CTLItem *pItem = new CTLItem;
    pItem->InsertItem(lpszItem);
    m_nItems++;

    return CTreeCtrl::InsertItem(
            TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE,
            "",
            nImage,
            nSelectedImage,
            0,
            0,
            (LPARAM)pItem,
            hParent,
            hInsertAfter);
}

HTREEITEM CTlcTree::InsertItem(
        UINT nMask,
        LPCTSTR lpszItem,
        int nImage,
        int nSelectedImage,
        UINT nState,
        UINT nStateMask,
        LPARAM lParam,
        HTREEITEM hParent,
        HTREEITEM hInsertAfter )
{
    CTLItem *pItem = new CTLItem;
    pItem->InsertItem(lpszItem);
    pItem->SetItemData(lParam);
    m_nItems++;

    return CTreeCtrl::InsertItem(
            nMask,
            "",
            nImage,
            nSelectedImage,
            nState,
            nStateMask,
            (LPARAM)pItem,
            hParent,
            hInsertAfter);
}

HTREEITEM CTlcTree::CopyItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
    if(ItemHasChildren(hItem))
        return NULL;

    TV_ITEM item;
    item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
    item.hItem = hItem;
    GetItem(&item);
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    CTLItem *pNewItem = new CTLItem(*pItem);

    item.lParam = (LPARAM)pNewItem;

    TV_INSERTSTRUCT insStruct;
    insStruct.item = item;
    insStruct.hParent = hParent;
    insStruct.hInsertAfter = hInsertAfter;

    return CTreeCtrl::InsertItem(&insStruct);
}

HTREEITEM CTlcTree::MoveItem(HTREEITEM hItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
    if(ItemHasChildren(hItem))
        return NULL;

    TV_ITEM item;
    item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
    item.hItem = hItem;
    GetItem(&item);
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    CTLItem *pNewItem = new CTLItem(*pItem);
    DeleteItem(hItem);

    item.lParam = (LPARAM)pNewItem;

    TV_INSERTSTRUCT insStruct;
    insStruct.item = item;
    insStruct.hParent = hParent;
    insStruct.hInsertAfter = hInsertAfter;

    return CTreeCtrl::InsertItem(&insStruct);
}

BOOL CTlcTree::SetItemText( HTREEITEM hItem, int nCol ,LPCTSTR lpszItem )
{
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    if(!pItem)
        return FALSE;
    pItem->SetSubstring(nCol, lpszItem);
    return CTreeCtrl::SetItemData(hItem, (LPARAM)pItem);
}

BOOL CTlcTree::SetItemColor( HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate )
{
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    if(!pItem)
        return FALSE;
    pItem->SetColour(m_newColor);
    if(!CTreeCtrl::SetItemData(hItem, (LPARAM)pItem))
        return FALSE;
    if(m_bInvalidate)
        Invalidate();
    return TRUE;
}

BOOL CTlcTree::SetItemBold( HTREEITEM hItem, BOOL m_Bold, BOOL m_bInvalidate )
{
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    if(!pItem)
        return FALSE;
    pItem->SetBold(m_Bold == TRUE);
    if(!CTreeCtrl::SetItemData(hItem, (LPARAM)pItem))
        return FALSE;
    if(m_bInvalidate)
        Invalidate();
    return TRUE;
}

CString CTlcTree::GetItemText( HTREEITEM hItem, int nSubItem )
{
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);

    if(!pItem)
        return _T("");
    return pItem->GetSubstring(nSubItem);
}

BOOL CTlcTree::DeleteItem( HTREEITEM hItem )
{
    CTLItem *pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
    if(!pItem)
        return FALSE;

    m_nItems --;
    // memory leak fix
    delete pItem;
    pItem = NULL;

    return CTreeCtrl::DeleteItem(hItem);
}

BOOL CTlcTree::DeleteItem( int nItem )
{
    return DeleteItem(GetTreeItem(nItem));
}

int CALLBACK CTlcTree::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CTLItem *pItem1 = (CTLItem *)lParam1;
    CTLItem *pItem2 = (CTLItem *)lParam2;

    SSortType *pSortType = (SSortType *)lParamSort;

    CString str1 = pItem1->GetSubstring(pSortType->nCol);
    CString str2 = pItem2->GetSubstring(pSortType->nCol);

    int m_nComp;
    // compare the two strings, but
    // notice:
    // in this case, "xxxx10" comes after "xxxx2"
    {
        CString tmpStr1, tmpStr2;
        int index = str1.FindOneOf("0123456789");
        if(index!=-1)
        {
            tmpStr1 = str1.Right(str1.GetLength()-index);
        }
        index = str2.FindOneOf("0123456789");
        if(index!=-1)
        {
            tmpStr2 = str2.Right(str2.GetLength()-index);
        }

        tmpStr1 = tmpStr1.SpanIncluding("0123456789");
        tmpStr2 = tmpStr2.SpanIncluding("0123456789");

        if((tmpStr1=="") && (tmpStr2==""))
        {
            m_nComp = str1.CompareNoCase(str2);
        }
        else
        {
            int num1 = atoi(tmpStr1);
            int num2 = atoi(tmpStr2);

            tmpStr1 = str1.SpanExcluding("0123456789");
            tmpStr2 = str2.SpanExcluding("0123456789");

            if(tmpStr1 == tmpStr2)
            {
                if(num1 > num2)
                {
                    m_nComp = 1;
                }
                else if(num1 < num2)
                {
                    m_nComp = -1;
                }
                else
                {
                    m_nComp = str1.CompareNoCase(str2);
                }
            }
            else
            {
                m_nComp = str1.CompareNoCase(str2);
            }
        }
    }

    if(!pSortType->bAscending)
    {
        if(m_nComp == 1)
        {
            m_nComp = -1;
        }
        else if(m_nComp == -1)
        {
            m_nComp = 1;
        }
    }

    return m_nComp;
}

BOOL CTlcTree::SortItems( int nCol, BOOL bAscending, HTREEITEM low)
{
    TV_SORTCB tSort;

    tSort.hParent = low;
    tSort.lpfnCompare = CompareFunc;

    SSortType *pSortType = new SSortType;
    pSortType->nCol = nCol;
    pSortType->bAscending = bAscending;
    tSort.lParam = (LPARAM)pSortType;

    BOOL m_bReturn = SortChildrenCB(&tSort);

    delete pSortType;

    return m_bReturn;
}

void CTlcTree::MemDeleteAllItems(HTREEITEM hParent)
{
    HTREEITEM hItem = hParent;
    CTLItem *pItem;

    while(hItem!=NULL)
    {
        pItem = (CTLItem *)CTreeCtrl::GetItemData(hItem);
        MemItemArray.Add(pItem);

        if(ItemHasChildren(hItem))
        {
            MemDeleteAllItems(GetChildItem(hItem));
        }
        hItem = GetNextSiblingItem(hItem);
    }
}

BOOL CTlcTree::DeleteAllItems()
{
    LockWindowUpdate();
    BeginWaitCursor();

    MemDeleteAllItems(GetRootItem());
    BOOL m_bReturn = CTreeCtrl::DeleteAllItems();

    for (int i=0;i<MemItemArray.GetSize();i++)
    {
        delete MemItemArray.GetAt(i);
    }
    MemItemArray.RemoveAll();
    m_nItems = 0;

    EndWaitCursor();
    UnlockWindowUpdate();
    return m_bReturn;
}

void CTlcTree::OnDestroy()
{
    DeleteAllItems();
    CTreeCtrl::OnDestroy();
}

BOOL CTlcTree::DeleteSubItems( HTREEITEM hItem )
{
    HTREEITEM hChild(0);
    BOOL bRet = false;
    BOOL bErr = false;

    while (ItemHasChildren(hItem))
    {
        hChild = GetChildItem(hItem);
        if (ItemHasChildren(hChild))
        {
            DeleteSubItems(hChild);
        }
        bRet = DeleteItem(hChild);
        if (!bRet)
        {
            bErr = true;
        }
    }
    return bErr;
}

void CTlcTree::OnRButtonDown(UINT nFlags, CPoint point)
{
    // ME: 3/19/2002:
    // find what item is selected
    m_selectedItem = HitTest(point, &nFlags);

    if ((nFlags & TVHT_ONITEMRIGHT)
            || (nFlags & TVHT_ONITEMINDENT)
            || (nFlags & TVHT_ONITEM))
    {
        SelectItem(m_selectedItem);
        NMHDR nmHdr;
        nmHdr.hwndFrom = GetParent()->GetSafeHwnd();
        nmHdr.idFrom = GetParent()->GetDlgCtrlID();
        nmHdr.code = TVN_SELCHANGED;
        WPARAM wParam = GetParent()->GetDlgCtrlID();
        LPARAM lParam = (LPARAM)(void*)&nmHdr;
        GetParent()->GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
    }

    const MSG *pMsg = GetCurrentMessage();
    GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);

    CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CTlcTree::OnRButtonUp(UINT nFlags, CPoint point)
{
    CTreeCtrl::OnRButtonUp(nFlags, point);

    const MSG *pMsg = GetCurrentMessage();
    CWnd *pWnd = GetParent();
    pWnd->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void CTlcTree::OnLButtonUp(UINT nFlags, CPoint point)
{
    CTreeCtrl::OnLButtonUp(nFlags, point);

    const MSG *pMsg = GetCurrentMessage();
    GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void CTlcTree::OnMouseMove(UINT nFlags, CPoint point)
{

    if ( m_nHoverTimerID )
    {
        KillTimer( m_nHoverTimerID );
        m_nHoverTimerID = 0;
    }

    // sets counter to expand if hovered over item for a while
    m_nHoverTimerID = SetTimer(2, 750, NULL);

    const MSG *pMsg = GetCurrentMessage();
    GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);


    CTreeCtrl::OnMouseMove(nFlags, point);
}

CString CTlcTree::GetNextLine(CString &message)
{
    CString result;
    BOOL bOK = FALSE;
    char k = 13;
    char j;
    int nSize = message.GetLength();
    int nIndex = 0;

    //Search for the return character in the string
    while(nIndex < nSize && !bOK)
    {
        j = message.GetAt(nIndex);
        if(j == k)
        {
            result = message.Left(nIndex);
            //Drop the carriage return and line feed characters
            message = message.Right(nSize - (nIndex + 2));
            bOK = TRUE;
        }
        nIndex++;
    }
    return result;
}

void CTlcTree::SetBkColor(COLORREF color)
{
    //Set the background colour.
    m_wndColor = color;
}

void CTlcTree::GetTreeHierarchy(
        HTREEITEM hTItem,
        CString &cstrTreeHierarchy,
        const BOOL bTopToBottom)
{
    HTREEITEM hTRootItem = GetParentItem(hTItem);
    if ( hTRootItem == NULL )
    {
        cstrTreeHierarchy += (GetItemText(hTItem)+"\\");
        return;
    }

    if (!bTopToBottom)
    {
        cstrTreeHierarchy += (GetItemText(hTItem)+"\\");
    }

    GetTreeHierarchy(hTRootItem, cstrTreeHierarchy,bTopToBottom);

    if (bTopToBottom)
    {
        cstrTreeHierarchy += (GetItemText(hTItem)+"\\");
    }
}

HTREEITEM CTlcTree::GetChildPositionAt(HTREEITEM HTParentItem, int iChildItem)
{
    //Check to see if the item  passed to us has any children.
    if(ItemHasChildren(HTParentItem))
    {
        HTREEITEM hChildItem = GetChildItem(HTParentItem);

        int iterator = 0;

        while (hChildItem != NULL)
        {
            if(iterator == iChildItem)
            {
                //This is the item  we want.
                return hChildItem;
            }
            else
            {
                hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
                iterator++;
            }
        }
    }

    return NULL;    //Only reach here upon failure.
}

HTREEITEM CTlcTree::GetRootItem( )
{
    return CTreeCtrl::GetRootItem();
}

void CTlcTree::HeaderTracked()
{
    Invalidate();          // re-draw ourselves
}

void CTlcTree::SetHeaderCtrl(MfcControls::CTreeListHeaderCtrl* pHeader)
{
    ASSERT(pHeader);
    ASSERT(pHeader->IsKindOf(RUNTIME_CLASS(CTreeListHeaderCtrl)));
    m_pHeader = pHeader;
}

void CTlcTree::AutoResizeColumn(int nCol)
{
    ASSERT(m_pHeader);
    ASSERT(m_pHeader->IsKindOf(RUNTIME_CLASS(CTreeListHeaderCtrl)));
    CDC* pDC = GetDC();
    CFont *pFont = GetFont();
    CFont * oldFont = pDC->SelectObject(pFont);
    HTREEITEM hRoot = GetRootItem();

    int maxTextWidth = 0;
    HTREEITEM rootItem =GetRootItem();
    while (rootItem != NULL)
    {
        int nTextWidth = GetLongestTextWidthInBranch(rootItem, pDC, nCol, 0);
        maxTextWidth = max(maxTextWidth, nTextWidth);
        rootItem = GetNextSiblingItem(rootItem);
    }
    // Have the widest text width, set the column to be just wider than this
    HD_ITEM hi;
    hi.mask = HDI_WIDTH;
    hi.cxy = maxTextWidth + pDC->GetTextExtent("AB").cx; // Add spacing of 1 character
    pDC->SelectObject(oldFont);
    ReleaseDC(pDC);
    m_pHeader->SetItem(nCol, &hi);
    // Have to resize the others
    HeaderTracked();
}

LRESULT CTlcTree::OnAutoResizeCol(WPARAM wParam, LPARAM)
{
    AutoResizeColumn((int)wParam);
    return 1;
}

int CTlcTree::GetLongestTextWidthInBranch(
        HTREEITEM hItem,
        CDC *pDC,
        int nCol,
        int nLevel /*=0*/)
{
    ASSERT(nCol > -1 && nCol < m_pHeader->GetItemCount());
    ASSERT(hItem);
    ASSERT(pDC);
    CString strItemText = GetItemText(hItem, nCol);
    CSize itemSize = pDC->GetTextExtent(strItemText);
    // Take into account indentation and image width of first column
    if (nCol == 0)
    {
        int imageDimension = 0;
        if (GetImageList(TVSIL_NORMAL) != NULL)
        {
            imageDimension = GetImageDimensions(hItem).cx;
        }
        itemSize.cx += ((GetIndent() * (nLevel+1)) + imageDimension);
    }
    HTREEITEM hChild = GetChildItem(hItem);
    int nBranchMaxWidth = 0;
    int nLongestTextWidth = itemSize.cx;
    nLevel++;
    while (hChild)
    {
        nBranchMaxWidth = GetLongestTextWidthInBranch(hChild, pDC, nCol, nLevel);
        nLongestTextWidth = max(nBranchMaxWidth, nLongestTextWidth);
        hChild = GetNextSiblingItem(hChild);
    }
    return nLongestTextWidth;
}

CSize CTlcTree::GetImageDimensions(HTREEITEM hItem)
{
    ASSERT(hItem);
    ASSERT(GetImageList(TVSIL_NORMAL));

    // Get the image index of the item
    TVITEM tvItem;
    BOOL bSelected = (GetSelectedItem() == hItem);
    tvItem.mask = (bSelected ? TVIF_SELECTEDIMAGE : TVIF_IMAGE);
    tvItem.hItem = hItem;
    GetItem(&tvItem);
    // Get a info about the image from the image list
    IMAGEINFO imageInfo;
    GetImageList(TVSIL_NORMAL)->GetImageInfo((bSelected ? tvItem.iSelectedImage : tvItem.iImage), &imageInfo);
    // Get the size of the image
    CSize imageSize(
        imageInfo.rcImage.right - imageInfo.rcImage.left,
        imageInfo.rcImage.bottom - imageInfo.rcImage.top);
    return imageSize;
}

BOOL CTlcTree::IsPointInBitmap(CPoint pt)
{
    HTREEITEM hItemAtPt = HitTest(pt);
    if (hItemAtPt) // If there is no item, then there can be no bitmap!
    {
        // Need to know the level at which the item is
        int iLevel = GetItemLevel(hItemAtPt);
        ASSERT(iLevel > -1);
        // Get the size of the bitmap (assume all others are the same size)
        CSize bmSize = GetImageDimensions(hItemAtPt);
        // Need the size of the actual item
        CRect itemRect;
        GetItemRect(hItemAtPt, &itemRect, FALSE);
        // Work out the area in the window where the bitmap is
        UINT uiIndent = GetIndent();
        CRect bmRect(
            (uiIndent * (iLevel+1)),
            itemRect.top,
            (uiIndent * (iLevel + 1)) + bmSize.cx,
            itemRect.bottom);
        // Is the point within this area?
        return (bmRect.PtInRect(pt));
    }
    return FALSE;
}

int CTlcTree::GetItemLevel(HTREEITEM hItem, HTREEITEM hBranchRoot /*=NULL*/, int iLevelsCheckedSoFar /*=0*/)
{
    ASSERT(hItem);
    if (hBranchRoot == NULL)
        hBranchRoot = GetRootItem();
    // Have we found our target?
    if (hItem == hBranchRoot)
        return iLevelsCheckedSoFar;
    else
    {
        // Search the branches from here
        iLevelsCheckedSoFar++;
        hBranchRoot = GetChildItem(hBranchRoot);
        while (hBranchRoot)
        {
            int iLevel = GetItemLevel(hItem, hBranchRoot, iLevelsCheckedSoFar);
            if (iLevel > -1)
                return iLevel; // Been found in that branch
            hBranchRoot = GetNextSiblingItem(hBranchRoot);
        }
    }
    return -1; // Did not find hItem anywhere in this branch
}

void CTlcTree::DeleteAllChildren(HTREEITEM hItem)
{
    HTREEITEM hChild = GetChildItem(hItem);
    while (hChild)
    {
        DeleteItem(hChild);
        hChild = GetChildItem(hItem);
    }
}

void CTlcTree::ExpandAll()
{
    ExpandBranch(GetRootItem());
}

void CTlcTree::CollapseAll()
{
    Expand(GetRootItem(), TVE_COLLAPSE);
}

void CTlcTree::ExpandBranch()
{
    ExpandBranch(GetSelectedItem());
}

void CTlcTree::CollapseBranch()
{
    Expand(GetSelectedItem(), TVE_COLLAPSE);
}

void CTlcTree::ExpandBranch(HTREEITEM hItem)
{
    if (ItemHasChildren(hItem))
    {
        Expand(hItem, TVE_EXPAND);
        HTREEITEM hChild = GetChildItem(hItem);
        while (hChild)
        {
            ExpandBranch(hChild);
            hChild = GetNextSiblingItem(hChild);
        }
    }
}

BOOL CTlcTree::PreTranslateMessage(MSG* pMsg)
{
    BOOL handled = GetParent()->PreTranslateMessage(pMsg);
    if (!handled)
    {
        handled = CTreeCtrl::PreTranslateMessage(pMsg);
    }
    return handled;
}

LRESULT CTlcTree::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // pass through to parent
    return GetParent()->SendMessage(WM_MOUSELEAVE, wParam, lParam);
}

CSize CTreeListCtrl::RequiredControlSizeForContent(int maxItemsToShow)
{
    // add up the widths of all the columns
    size_t columnCount = GetHeaderCtrl().GetItemCount();
    int width = 0;
    int height = 0;

    for (size_t column = 0; column < columnCount; ++column)
    {
        int columnWidth = GetColumnWidth(column);

        width += columnWidth;
    }
    // take control border into account
    width += (GetSystemMetrics(SM_CXBORDER) * 4);
    // allow space for a vertical scrollbar
    width += GetSystemMetrics(SM_CXHSCROLL);

    // now work out the required height of the control
    if (IsWindow(GetHeaderCtrl().GetSafeHwnd()))
    {
        CRect headerRect;
        GetHeaderCtrl().GetWindowRect(&headerRect);
        height += headerRect.Height();
    }
    int numItems = GetItemCount();
    if (numItems > 0)
    {
        CRect itemRect;
        GetItemRect(GetRootItem(), &itemRect, LVIR_BOUNDS);
        height += itemRect.Height() * min(numItems, maxItemsToShow);
    }
    else
    {
        // assume default item height of icon only
        height += GetSystemMetrics(SM_CYICON);
    }
    // take control border into account
    height += (GetSystemMetrics(SM_CYBORDER) * 4);
    // allow space for a horizontal scrollbar
    height += GetSystemMetrics(SM_CYHSCROLL);
    return CSize(width, height);
}

void CTreeListCtrl::LoadColumnWidthsByName(UINT resourceStringId)
{
    // make sure columns have correct width
    CTreeListHeaderCtrl* pHeaderCtrl = &GetHeaderCtrl();
    if (pHeaderCtrl != NULL)
    {
        // some controls set the control Id to a specific value, only set to control ID if its not
        // already set to something other than 0, else we could break some ON_HDN_NOTIFICATION messages
        if (pHeaderCtrl->GetDlgCtrlID() == 0)
        {
            pHeaderCtrl->SetDlgCtrlID(GetDlgCtrlID());
        }
        int nColumnCount = pHeaderCtrl->GetItemCount();
        for (int column = 0; column < nColumnCount; column++)
        {
            CString valueKeyName;

            HDITEM item;
            char itemText[256];
            item.mask = HDI_TEXT;
            item.pszText = itemText;
            item.cchTextMax = sizeof(itemText);
            pHeaderCtrl->GetItem(column, &item);
            if (_tcslen(itemText) == 0) // nameless column?
            {
                sprintf_s(itemText, sizeof(itemText), "%d", column);
            }

            valueKeyName.Format(resourceStringId, itemText);
            valueKeyName.Remove('\\');
            int width = AfxGetApp()->GetProfileInt(f_registrySection, valueKeyName, f_noWidthSetup);
            if (width != f_noWidthSetup)
            {
                SetColumnWidth(column, width);
            }
        }
    }
}

void CTreeListCtrl::SaveColumnWidthsByName(UINT resourceStringId)
{
    // save the new column widths to the registry
    CTreeListHeaderCtrl* pHeaderCtrl = &GetHeaderCtrl();
    if (pHeaderCtrl != NULL)
    {
        int nColumnCount = pHeaderCtrl->GetItemCount();
        for (int column = 0; column < nColumnCount; column++)
        {
            CString valueKeyName;

            HDITEM item;
            char itemText[256];
            item.mask = HDI_TEXT;
            item.pszText = itemText;
            item.cchTextMax = sizeof(itemText);
            pHeaderCtrl->GetItem(column, &item);
            if (_tcslen(itemText) == 0) // nameless column?
            {
                sprintf_s(itemText, sizeof(itemText), "%d", column);
            }

            valueKeyName.Format(resourceStringId, itemText);
            valueKeyName.Remove('\\');
            int width = GetColumnWidth(column);
            AfxGetApp()->WriteProfileInt(f_registrySection, valueKeyName, width);
        }
    }
}

void CTreeListCtrl::OnSysColorChange()
{
    // keep ourselves and our child windows up to date with any system colour changes
    CWnd::OnSysColorChange();
    m_Tree.SendMessage(WM_SYSCOLORCHANGE, 0, 0L);
    m_Header.SendMessage(WM_SYSCOLORCHANGE, 0, 0L);
    m_Tree.SetBkColor(GetSysColor(m_Tree.IsWindowEnabled() ? COLOR_WINDOW: COLOR_3DFACE)); // enabled / disabled colour
    m_Tree.Invalidate(TRUE);
    m_Header.Invalidate(TRUE);
}
