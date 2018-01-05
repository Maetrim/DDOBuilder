// LevelUpView.h
//
#pragma once
#include "Resource.h"
#include "Character.h"
#include "SortHeaderCtrl.h"
#include "InfoTip.h"
#include "LevelButton.h"
#include "ComboBoxTooltip.h"

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
        afx_msg void OnWindowPosChanging(WINDOWPOS * pos);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnEndtrackListSkills(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnEndtrackListAutomaticFeats(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnEndtrackListGrantedFeats(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnHoverAutomaticFeats(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnHoverGrantedFeats(NMHDR* pNMHDR, LRESULT* pResult);
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
        afx_msg void OnButtonSkillsDialog();
        afx_msg void OnFeatSelection(UINT nID);
        afx_msg void OnFeatSelectionCancel(UINT nID);
        afx_msg void OnCustomDrawSkills(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
        afx_msg LRESULT OnUpdateComplete(WPARAM wParam, LPARAM lParam);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        afx_msg void OnButtonLevel(UINT nID);
        afx_msg LRESULT OnHoverComboBox(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

        // DocumentObserver overrides
        virtual void UpdateAlignmentChanged(Character * pCharacter, AlignmentType alignment) override;
        virtual void UpdateSkillSpendChanged(Character * pCharacter, size_t level, SkillType skill) override;
        virtual void UpdateSkillTomeChanged(Character * pCharacter, SkillType skill) override;
        virtual void UpdateClassChoiceChanged(Character * pCharacter) override;
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateAbilityValueChanged(Character * pCharacter, AbilityType ability) override;
        virtual void UpdateAbilityTomeChanged(Character * pCharacter, AbilityType ability) override;
        virtual void UpdateRaceChanged(Character * pCharacter, RaceType race) override;
        virtual void UpdateFeatTrained(Character * pCharacter, const std::string & featName) override;
        virtual void UpdateFeatRevoked(Character * pCharacter, const std::string & featName) override;
        virtual void UpdateGrantedFeatsChanged(Character * charData) override;

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
        void PopulateGrantedFeats();
        void ShowFeatTip(size_t featIndex, CRect itemRect);
        void ShowLevelTip(size_t level, CRect itemRect);
        void HideTip();
        void SetFeatTooltipText(
                const CString & featName,
                CPoint tipTopLeft,
                CPoint tipAlternate,
                bool rightAlign,
                TrainableFeatTypes type,
                bool alreadyTrained);
        void SetLevelTooltipText(size_t level, CPoint tipTopLeft, CPoint tipAlternate);
        void ShowBab();
        void SetAbilitiesAtLevel();

        Character * m_pCharacter;
        CLevelButton m_buttonLevels[MAX_LEVEL];
        CStatic m_staticClass;
        CComboBox m_comboClass[3];
        CStatic m_staticFeatDescription[3];
        CComboBoxTooltip m_comboFeatSelect[3];
        CStatic m_abilitiesAtLevel[6];
        CStatic m_staticBab;
        CStatic m_staticAvailableSpend;
        CEdit m_editSkillPoints;
        CButton m_buttonPlus;
        CButton m_buttonMinus;
        CButton m_buttonSkillsDialog;
        CComboBox m_comboSkillTome;
        CListCtrl m_listSkills;
        CListCtrl m_listAutomaticFeats;
        CListCtrl m_listGrantedFeats;
        CImageList m_imagesSkills;
        CImageList m_imagesFeats[3];
        CImageList m_imagesAutomaticFeats;
        CImageList m_imagesGrantedFeats;
        CSortHeaderCtrl m_sortHeader;
        SkillType m_skillBeingEdited;
        size_t m_level;
        std::vector<TrainableFeatTypes> m_trainable;
        bool m_canPostMessage;

        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        int m_selectedSkill;

        UINT m_hookFeatHandles[3];
        UINT m_hookLevelHandles[MAX_LEVEL];
        UINT m_automaticHandle;
        int m_hoverItem;
};
