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
        const std::vector<CStanceButton *> & UserStances() const;
        const std::vector<CStanceButton *> & AutoStances() const;
        const CStanceButton * GetStance(const std::string & stanceName) const;
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
        virtual void UpdateNewStance(Character * charData, const Stance & stance) override;
        virtual void UpdateRevokeStance(Character * charData, const Stance & stance) override;
        virtual void UpdateStanceActivated(Character * charData, const std::string & stanceName) override;
        virtual void UpdateStanceDeactivated(Character * charData, const std::string & stanceName) override;
        virtual void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName,  const Effect & effect) override;

        void CreateStanceWindows();
        void AddStance(const Stance & stance);
        void RevokeStance(const Stance & stance);
        void DestroyAllStances();

        void ShowTip(const CStanceButton & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const CStanceButton & item, CPoint tipTopLeft, CPoint tipAlternate);

        CDocument * m_pDocument;
        Character * m_pCharacter;
        CStatic m_userStances;
        CStatic m_autoStances;
        std::vector<CStanceButton *> m_userStancebuttons;
        std::vector<CStanceButton *> m_autoStancebuttons;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        const CStanceButton * m_pTooltipItem;
        int m_nextStanceId;
};
