// SelfAndPartyBuffsView.h
//
#pragma once
#include "Resource.h"
#include "Character.h"
#include "InfoTip.h"

class CSelfAndPartyBuffsView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_SELFANDPARTYBUFFS };
        DECLARE_DYNCREATE(CSelfAndPartyBuffsView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CSelfAndPartyBuffsView();           // protected constructor used by dynamic creation
        virtual ~CSelfAndPartyBuffsView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnItemchangedListBuffs(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnHoverListBuffs(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

    private:
        void CheckSelectedItems();
        void ShowTip(const std::string & name, CRect itemRect);
        void HideTip();
        CDocument * m_pDocument;
        Character * m_pCharacter;

        CListCtrl m_listBuffs;
        bool m_bPopulating;
        // item tooltips
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        int m_hoverItem;
        UINT m_hoverHandle;
};
