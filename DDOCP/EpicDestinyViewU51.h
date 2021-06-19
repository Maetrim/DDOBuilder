// EnhancementsView.h
//
#pragma once
#include "Resource.h"
#include <list>
#include "Character.h"
#include "IconButton.h"
#include "InfoTip.h"
#include "BreakdownItem.h"

class EnhancementTree;
class CEnhancementTreeDialog;

class CEpicDestinyViewU51 :
    public CFormView,
    public CharacterObserver,
    public BreakdownObserver
{
    public:
        enum { IDD = IDD_U51_EPIC_DESTINY_VIEW };
        DECLARE_DYNCREATE(CEpicDestinyViewU51)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CEpicDestinyViewU51();           // protected constructor used by dynamic creation
        virtual ~CEpicDestinyViewU51();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnTreeSelect(UINT nID);
        afx_msg LRESULT OnUpdateTrees(WPARAM wParam, LPARAM lParam);
        afx_msg void OnClaimTree(UINT nID);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        DECLARE_MESSAGE_MAP()

        // CharacterObserver overrides
        void UpdateEnhancementTreeReset(Character * charData) override;
        void UpdateActionPointsChanged(Character * charData) override;
        void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) override;
        void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        void UpdateEnhancementTreeOrderChanged(Character * charData) override;

        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        std::list<EnhancementTree> DetermineTrees();
        void CreateEnhancementWindows();
        void DestroyEnhancementWindows();
        void UpdateEnhancementWindows();
        void UpdateWindowTitle();
        void PopulateTreeCombo(CComboBox * combo, const std::string & selectedTree);
        void EnableDisableComboboxes();
        void UpdateTrees();
        void AddCustomButtons();
        void ShowTip(const CIconButton & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const CIconButton & item, CPoint tipTopLeft, CPoint tipAlternate);
        enum MaxSupportTrees
        {
            MST_Number = MAX_EPIC_DESTINY_TREES
        };
        CDocument * m_pDocument;
        Character * m_pCharacter;
        std::list<EnhancementTree> m_availableTrees;
        CComboBox m_comboTreeSelect[MST_Number];
        CIconButton m_destinyTrees[12];
        std::vector<CEnhancementTreeDialog *> m_treeViews;
        std::vector<size_t> m_visibleTrees; // only some trees will be visible (indexes into m_treeViews)
        size_t m_numDestinyButtons;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        const CIconButton * m_pTooltipItem;
};
