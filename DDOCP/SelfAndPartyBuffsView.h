// SelfAndPartyBuffsView.h
//
#pragma once
#include "Resource.h"
#include "Character.h"

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
        DECLARE_MESSAGE_MAP()

    private:
        CDocument * m_pDocument;
        Character * m_pCharacter;

        CListCtrl m_listBuffs;
};
