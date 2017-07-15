#pragma once

#include "TreeListHeaderCtrl.h"

namespace MfcControls
{
    struct SSortType
    {
        int nCol;
        BOOL bAscending;
    };

    class CTLItem
    {
        public:

            CTLItem();
            CTLItem(CTLItem & copyItem);

            CString GetItemString()
            {
                return m_itemString;
            };
            CString GetSubstring(int m_nSub);
            CString GetItemText()
            {
                return GetSubstring(0);
            };
            void SetSubstring(int m_nSub, CString m_sText);
            void InsertItem(CString m_sText)
            {
                SetSubstring(0, m_sText);
            };
            void SetColour(COLORREF colour);
            COLORREF Colour() const;
            void SetBold(bool bold);
            bool Bold() const;
            void SetItemData(DWORD data);
            DWORD ItemData() const;

        private:
            CString m_itemString;
            DWORD m_itemData;
            char m_cEnding;
            // visual attributes
            BOOL m_Bold;
            COLORREF m_Color;
    };

    class CTlcTree : public CTreeCtrl
    {
        public:
            CTlcTree();
            DECLARE_DYNAMIC(CTlcTree)

            CImageList m_cImageList;
            CImageList m_StatusImageList;

            int m_nOffset;

            enum CheckState {NOSTATE = 0, UNCHECKED, CHECKED, CHILD_CHECKED, SELF_AND_CHILD_CHECKED };
            enum CheckType  {CHECK, UNCHECK, TOGGLE, REFRESH };

            void AutoResizeColumn(int nCol);
            void SetHeaderCtrl(CTreeListHeaderCtrl * pHeader);
            int GetColumnsNum() { return m_nColumns; };
            int GetColumnsWidth() { return m_nColumnsWidth; };
            int GetItemCount() { return m_nItems; };
            void RecalcColumnsWidth();

            HTREEITEM GetTreeItem(int nItem);
            int GetListItem(HTREEITEM hItem);

            int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
            int GetColumnWidth(int nCol);
            int GetColumnAlign(int nCol);

            BOOL SetItemData(HTREEITEM hItem, DWORD dwData);
            DWORD GetItemData(HTREEITEM hItem) const;

            CString GetItemText(HTREEITEM hItem, int nSubItem = 0);

            HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
            HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
            HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter);

            HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
            HTREEITEM MoveItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);

            BOOL DeleteSubItems(HTREEITEM hItem);
            BOOL DeleteItem(HTREEITEM hItem);
            BOOL DeleteItem(int nItem);

            void MemDeleteAllItems(HTREEITEM hParent);
            BOOL DeleteAllItems();

            BOOL SetItemText(HTREEITEM hItem, int nCol ,LPCTSTR lpszItem);

            BOOL SetItemColor(HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate = TRUE);
            BOOL SetItemBold(HTREEITEM hItem, BOOL m_Bold = TRUE, BOOL m_bInvalidate = TRUE);

            static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
            BOOL SortItems(int nCol, BOOL bAscending, HTREEITEM low);

            void DrawItemText(
                    CDC* pDC,
                    HTREEITEM hItem,
                    size_t column,
                    bool selected,
                    CRect rect, 
                    int nWidth, 
                    int nFormat);

            void SetBkColor(COLORREF color);
            COLORREF m_wndColor;
            void GetTreeHierarchy(HTREEITEM hTItem, CString &cstrTreeHierarchy, const BOOL bTopToBottom);
            HTREEITEM GetChildPositionAt(HTREEITEM HTParentItem, int iChildItem);
            HTREEITEM GetRootItem();
            void HeaderTracked() ;

            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CTlcTree)
            virtual BOOL PreTranslateMessage(MSG* pMsg);
            //}}AFX_VIRTUAL

            void DeleteAllChildren(HTREEITEM hItem);
            int GetItemLevel(HTREEITEM hItem, HTREEITEM hBranchRoot = NULL, int iLevelsCheckedSoFar = 0);
            CSize GetImageDimensions(HTREEITEM hItem);
            void UpdateDisplay();
            BOOL LocatePickLists(CString CustNum,BOOL HighlightLast,int ReturnToPickID);
            CString GetNextLine(CString &message);
            CString Return_SelectedCustNum();
            int Return_SelectedPick();
            BOOL IsInPickingList(CString csItem,CString OrdNum);
            BOOL IsPointInBitmap(CPoint pt);
            void ExpandBranch(HTREEITEM hItem);
            void CollapseBranch();
            void ExpandBranch();
            void CollapseAll();
            void ExpandAll();
            virtual ~CTlcTree();

            // Generated message map functions
        protected:
            //{{AFX_MSG(CMyTreeCtrl)
            afx_msg BOOL OnEraseBkgnd(CDC* pDC);
            afx_msg void OnPaint();
            afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
            afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
            afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
            afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
            afx_msg void OnDestroy();
            afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnTreelist2refresh();
            afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
            afx_msg void OnMouseMove(UINT nFlags, CPoint point);
            afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
            afx_msg LRESULT OnAutoResizeCol(WPARAM wParam, LPARAM lParam);
            afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam) ;
            //}}AFX_MSG

            DECLARE_MESSAGE_MAP()

        private:
            int GetLongestTextWidthInBranch(HTREEITEM hItem, CDC* pDC, int nCol, int nLevel = 0);
            HTREEITEM GetNextTreeItem(HTREEITEM hItem) const;

            int m_nColumns;
            int m_nColumnsWidth;
            int m_nItems;
            CDWordArray m_dwaItems;
            CStringArray TreeLayout;
            UINT m_nHoverTimerID;
            CArray<CTLItem*,CTLItem*> MemItemArray;
            HTREEITEM m_selectedItem;
            CTreeListHeaderCtrl * m_pHeader;
    };


    class CTreeListCtrl : public CWnd
    {
        public:
            CTreeListCtrl(bool hasButtons, bool linesAtRoot);
            virtual ~CTreeListCtrl();
            CTlcTree & GetTreeCtrl() { return m_Tree;};
            CTreeListHeaderCtrl & GetHeaderCtrl() {return m_Header;};
            virtual void Initialise();

            BOOL LockWindowUpdate();
            void UnlockWindowUpdate();

            CSize RequiredControlSizeForContent(int maxItemsToShow);
            void LoadColumnWidthsByName(UINT resourceStringId);
            void SaveColumnWidthsByName(UINT resourceStringId);

            // pass through calls
            int GetColumnsNum();
            int GetColumnsWidth();
            void SetColumnWidth(int column, int width);
            void AutoResizeColumn(int nCol);
            int GetItemCount();
            void RecalcColumnsWidth();
            HTREEITEM GetTreeItem(int nItem);
            int GetListItem(HTREEITEM hItem);
            int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
            int GetColumnWidth(int nCol);
            int GetColumnAlign(int nCol);
            CString GetItemText(HTREEITEM hItem, int nSubItem = 0);
            HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
            HTREEITEM MoveItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
            BOOL DeleteSubItems(HTREEITEM hItem);
            BOOL DeleteItem(int nItem);
            void MemDeleteAllItems(HTREEITEM hParent);
            BOOL SetItemText(HTREEITEM hItem, int nCol ,LPCTSTR lpszItem);
            BOOL SetItemColor(HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate = TRUE);
            BOOL SetItemBold(HTREEITEM hItem, BOOL m_Bold = TRUE, BOOL m_bInvalidate = TRUE);
            BOOL SortItems(int nCol, BOOL bAscending, HTREEITEM low);
            void GetTreeHierarchy(HTREEITEM hTItem, CString &cstrTreeHierarchy, const BOOL bTopToBottom);
            HTREEITEM GetChildPositionAt(HTREEITEM HTParentItem, int iChildItem);
            HTREEITEM GetRootItem();
            CString GetNextLine(CString &message);
            // standard CTreeCtrl interface calls we need to pass through
            BOOL GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly) const;
            BOOL GetSubItemRect(HTREEITEM hItem, int columnIndex, LPRECT lpRect) const;
            UINT GetCount() const;
            UINT GetIndent() const;
            void SetIndent(UINT nIndent);
            CImageList * GetImageList(UINT nImageList) const;
            CImageList * SetImageList(CImageList* pImageList, int nImageListType);
            HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode) const;
            HTREEITEM GetChildItem(HTREEITEM hItem) const;
            HTREEITEM GetNextSiblingItem(HTREEITEM hItem) const;
            HTREEITEM GetPrevSiblingItem(HTREEITEM hItem) const;
            HTREEITEM GetParentItem(HTREEITEM hItem) const;
            HTREEITEM GetFirstVisibleItem() const;
            HTREEITEM GetNextVisibleItem(HTREEITEM hItem) const;
            HTREEITEM GetPrevVisibleItem(HTREEITEM hItem) const;
            HTREEITEM GetSelectedItem() const;
            HTREEITEM GetDropHilightItem() const;
            BOOL GetItem(TVITEM* pItem) const;
            BOOL GetItemImage(HTREEITEM hItem, int& nImage, int& nSelectedImage) const;
            UINT GetItemState(HTREEITEM hItem, UINT nStateMask) const;
            DWORD GetItemData(HTREEITEM hItem) const;
            BOOL SetItem(TVITEM* pItem);
            BOOL SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam);
            BOOL SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage);
            BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask);
            BOOL SetItemData(HTREEITEM hItem, DWORD dwData);
            BOOL ItemHasChildren(HTREEITEM hItem) const;
            CEdit * GetEditControl() const;
            UINT GetVisibleCount() const;
            CToolTipCtrl * GetToolTips() const;
            CToolTipCtrl * SetToolTips(CToolTipCtrl* pWndTip);
            COLORREF GetBkColor() const;
            COLORREF SetBkColor(COLORREF clr);
            SHORT GetItemHeight() const;
            SHORT SetItemHeight(SHORT cyHeight);
            COLORREF GetTextColor() const;
            COLORREF SetTextColor(COLORREF clr);
            BOOL SetInsertMark(HTREEITEM hItem, BOOL fAfter = TRUE);
            BOOL GetCheck(HTREEITEM hItem) const;
            BOOL SetCheck(HTREEITEM hItem, BOOL fCheck = TRUE);
            COLORREF GetInsertMarkColor() const;
            COLORREF SetInsertMarkColor(COLORREF clrNew);

            // Operations
            HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct);
            HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,    int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter);
            HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
            HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
            BOOL DeleteItem(HTREEITEM hItem);
            BOOL DeleteAllItems();
            BOOL Expand(HTREEITEM hItem, UINT nCode);
            BOOL Select(HTREEITEM hItem, UINT nCode);
            BOOL SelectItem(HTREEITEM hItem);
            BOOL SelectDropTarget(HTREEITEM hItem);
            BOOL SelectSetFirstVisible(HTREEITEM hItem);
            CEdit * EditLabel(HTREEITEM hItem);
            HTREEITEM HitTest(CPoint pt, UINT* pFlags = NULL) const;
            HTREEITEM HitTest(TVHITTESTINFO* pHitTestInfo) const;
            CImageList * CreateDragImage(HTREEITEM hItem);
            BOOL SortChildren(HTREEITEM hItem);
            BOOL EnsureVisible(HTREEITEM hItem);
            BOOL SortChildrenCB(LPTVSORTCB pSort);
            void CollapseBranch();
            void ExpandBranch();
            void CollapseAll();
            void ExpandAll();
            void ResizeControls(int cx, int cy); // Resizes the controls based on the given width (cx) and the given height (cy)

            // Overrides
            // ClassWizard generated virtual function overrides
            //{{AFX_VIRTUAL(CTreeListCtrl)
            virtual BOOL Create(
                    LPCTSTR lpszClassName,
                    LPCTSTR lpszWindowName,
                    DWORD dwStyle,
                    const RECT& rect,
                    CWnd* pParentWnd,
                    UINT nID,
                    CCreateContext* pContext = NULL);
            virtual BOOL CreateEx(
                    DWORD dwExStyle,
                    LPCTSTR lpszClassName,
                    LPCTSTR lpszWindowName,
                    DWORD dwStyle,
                    const RECT& rect,
                    CWnd* pParentWnd,
                    UINT nID,
                    CCreateContext* pContext = NULL);
            //}}AFX_VIRTUAL
            virtual BOOL EnableWindow(BOOL bEnable);

            void DeleteAllChildren(HTREEITEM hItem);
            BOOL IsPtInBitmap(CPoint pt);

        protected:
            int StretchWidth(int m_nWidth, int m_nMeasure);

            // Generated message map functions
            //{{AFX_MSG(CTreeListCtrl)
            afx_msg void OnSize(UINT nType, int cx, int cy);
            afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
            afx_msg BOOL OnEraseBkgnd(CDC* pDC);
            afx_msg void OnDestroy();
            afx_msg void OnSetFocus(CWnd* pOldWnd);
            afx_msg void OnExpandAll();
            afx_msg void OnUpdateExpandAll(CCmdUI* pCmdUI);
            afx_msg void OnExpandBranch();
            afx_msg void OnUpdateExpandBranch(CCmdUI* pCmdUI);
            afx_msg void OnCollapseAll();
            afx_msg void OnUpdateCollapseAll(CCmdUI* pCmdUI);
            afx_msg void OnCollapseBranch();
            afx_msg void OnUpdateCollapseBranch(CCmdUI* pCmdUI);
            afx_msg void OnSysColorChange();
            //}}AFX_MSG
            afx_msg void OnEndtrackHeader(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg LRESULT OnAutoResizeCol(WPARAM wParam, LPARAM lParam);
            DECLARE_MESSAGE_MAP()

        private:
            CTlcTree m_Tree;
            CTreeListHeaderCtrl m_Header;
            bool m_showHorzScrollBar;
            bool m_hasButtons;
            bool m_linesAtRoot;
    };
}
