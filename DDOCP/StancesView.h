// StancesView.h
//
#pragma once
#include "Resource.h"
#include <vector>
#include "Character.h"
#include "StanceButton.h"
#include "InfoTip.h"

class CStancesView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_STANCES_VIEW };
        DECLARE_DYNCREATE(CStancesView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CStancesView();           // protected constructor used by dynamic creation
        virtual ~CStancesView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

    private:
        // Character observers
        virtual void UpdateNewStance(Character * charData, const Stance & stance);
        virtual void UpdateRevokeStance(Character * charData, const Stance & stance);
        virtual void UpdateStanceActivated(Character * charData, const std::string & stanceName);
        virtual void UpdateStanceDeactivated(Character * charData, const std::string & stanceName);

        void CreateStanceWindows();
        void AddStance(const Stance & stance);
        void RevokeStance(const Stance & stance);
        void DestroyAllStances();

        void ShowTip(const CStanceButton & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const CStanceButton & item, CPoint tipTopLeft);

        CDocument * m_pDocument;
        Character * m_pCharacter;
        std::vector<CStanceButton *> m_stancebuttons;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        const CStanceButton * m_pTooltipItem;
        int m_nextStanceId;
};
