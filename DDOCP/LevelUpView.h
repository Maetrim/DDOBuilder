// LevelUpView.h
//
#pragma once
#include "Resource.h"
#include "Character.h"
#include "SortHeaderCtrl.h"
#include "InfoTip.h"
#include "LevelButton.h"

class CLevelUpView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_LEVEL_UP_VIEW };
        DECLARE_DYNCREATE(CLevelUpView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CLevelUpView();           // protected constructor used by dynamic creation
        virtual ~CLevelUpView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnEndtrackListSkills(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnEndtrackListAutomaticFeats(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnColumnclickListSkills(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnDoubleClickListSkills(NMHDR*, LRESULT* pResult);
        afx_msg void OnLeftClickListSkills(NMHDR*, LRESULT* pResult);
        afx_msg void OnRightClickListSkills(NMHDR*, LRESULT* pResult);
        afx_msg void OnItemchangedListSkills(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnClass1Selected();
        afx_msg void OnClass2Selected();
        afx_msg void OnClass3Selected();
        afx_msg void OnSkillTomeSelected();
        afx_msg void OnSkillTomeCancel();
        afx_msg void OnButtonSkillPlus();
        afx_msg void OnButtonSkillMinus();
        afx_msg void OnFeatSelection(UINT nID);
        afx_msg void OnCustomDrawSkills(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
        afx_msg LRESULT OnUpdateComplete(WPARAM wParam, LPARAM lParam);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        afx_msg void OnButtonLevel(UINT nID);
        DECLARE_MESSAGE_MAP()

        // DocumentObserver overrides
        void UpdateAlignmentChanged(Character * pCharacter, AlignmentType alignment) override;
        void UpdateSkillSpendChanged(Character * pCharacter, size_t level, SkillType skill) override;
        void UpdateSkillTomeChanged(Character * pCharacter, SkillType skill) override;
        void UpdateClassChoiceChanged(Character * pCharacter) override;
        void UpdateClassChanged(Character * pCharacter, ClassType type, size_t level) override;
        void UpdateAbilityValueChanged(Character * pCharacter, AbilityType ability) override;
        void UpdateAbilityTomeChanged(Character * pCharacter, AbilityType ability) override;
        void UpdateRaceChanged(Character * pCharacter, RaceType race) override;
        void UpdateFeatTrained(Character * pCharacter, const std::string & featName) override;
        void UpdateFeatRevoked(Character * pCharacter, const std::string & featName) override;

    private:
        static int CALLBACK SortCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        void PopulateControls();
        void SetLevelButtonStates();
        void SetAvailableClasses();
        void EnableControls();
        void EnableBuyButtons();
        bool CanBuySkill(SkillType skill) const;
        bool CanRevokeSkill(SkillType skill) const;
        void DetermineTrainableFeats();
        void PopulateCombobox(size_t index, const std::string & selection);
        void PopulateSkills();
        void PopulateAutomaticFeats();
        void ShowFeatTip(size_t featIndex, CRect itemRect);
        void ShowLevelTip(size_t level, CRect itemRect);
        void HideTip();
        void SetFeatTooltipText(const CString & featName, CPoint tipTopLeft, CPoint tipAlternate);
        void SetLevelTooltipText(size_t level, CPoint tipTopLeft, CPoint tipAlternate);

        Character * m_pCharacter;
        CLevelButton m_buttonLevels[MAX_LEVEL];
        CStatic m_staticClass;
        CComboBox m_comboClass[3];
        CStatic m_staticFeatDescription[3];
        CComboBoxEx m_comboFeatSelect[3];
        CStatic m_staticAvailableSpend;
        CEdit m_editSkillPoints;
        CButton m_buttonPlus;
        CButton m_buttonMinus;
        CButton m_buttonAutoSpend;
        CComboBox m_comboSkillTome;
        CListCtrl m_listSkills;
        CListCtrl m_listAutomaticFeats;
        CImageList m_imagesSkills;
        CImageList m_imagesFeats[3];
        CImageList m_imagesAutomaticFeats;
        CSortHeaderCtrl m_sortHeader;
        SkillType m_skillBeingEdited;
        size_t m_level;
        std::vector<TrainableFeatTypes> m_trainable;
        bool m_canPostMessage;

        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        const CWnd * m_pTooltipItem;
        int m_selectedSkill;
};
