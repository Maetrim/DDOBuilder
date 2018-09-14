// DCView.h
//
#pragma once
#include "Resource.h"

#include "Character.h"
#include "DCButton.h"
#include "InfoTip.h"

class CDCView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_DC };
        DECLARE_DYNCREATE(CDCView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();

        const std::vector<CDCButton *> & DCs() const;
    protected:
        CDCView();           // protected constructor used by dynamic creation
        virtual ~CDCView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

    private:
        // Character observers
        virtual void UpdateNewDC(Character * charData, const DC & dc) override;
        virtual void UpdateRevokeDC(Character * charData, const DC & dc) override;

        void AddDC(const DC & dc);
        void RevokeDC(const DC & dc);
        void DestroyAllDCs();

        void ShowTip(const CDCButton & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const CDCButton & item, CPoint tipTopLeft, CPoint tipAlternate);

        CDocument * m_pDocument;
        Character * m_pCharacter;
        std::vector<CDCButton *> m_dcButtons;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        const CDCButton * m_pTooltipItem;
        int m_nextDcId;
};
