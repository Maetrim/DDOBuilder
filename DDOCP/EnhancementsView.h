// EnhancementsView.h
//
#pragma once
#include "Resource.h"
#include <list>
#include "Character.h"
#include "IconButton.h"
#include "InfoTip.h"

class EnhancementTree;
class CEnhancementTreeDialog;

class CEnhancementsView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_ENHANCEMENTS_VIEW };
        DECLARE_DYNCREATE(CEnhancementsView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CEnhancementsView();           // protected constructor used by dynamic creation
        virtual ~CEnhancementsView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnTreeSelect(UINT nID);
        afx_msg LRESULT OnUpdateTrees(WPARAM wParam, LPARAM lParam);
        afx_msg void OnUniversalTree(UINT nID);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        DECLARE_MESSAGE_MAP()

        // CharacterObserver overrides
        void UpdateAlignmentChanged(Character * charData, AlignmentType alignmen) override;
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        void UpdateRaceChanged(Character * charData, RaceType race) override;
        void UpdateEnhancementTreeReset(Character * charData) override;
        void UpdateActionPointsChanged(Character * charData) override;
        void UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect) override;
        void UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect) override;
        void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        void UpdateEnhancementTreeOrderChanged(Character * charData) override;
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
            MST_Number = MAX_ENHANCEMENT_TREES
        };
        CDocument * m_pDocument;
        Character * m_pCharacter;
        std::list<EnhancementTree> m_availableTrees;
        CComboBox m_comboTreeSelect[MST_Number-1];  // 1st tree type is always fixed
        CIconButton m_universalTrees[8];
        std::vector<CEnhancementTreeDialog *> m_treeViews;
        std::vector<size_t> m_visibleTrees; // only some trees will be visible (indexes into m_treeViews)
        size_t m_numUniversalButtons;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        const CIconButton * m_pTooltipItem;
};
