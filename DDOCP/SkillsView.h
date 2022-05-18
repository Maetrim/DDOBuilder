// SkillsView.h
//
#pragma once
#include "Resource.h"

#include "Character.h"
#include "SkillsHeaderCtrl.h"

class CSkillsView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_SKILLS_VIEW };
        DECLARE_DYNCREATE(CSkillsView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual void OnInitialUpdate();
        virtual BOOL PreTranslateMessage(MSG* pMsg);

    protected:
        CSkillsView();           // protected constructor used by dynamic creation
        virtual ~CSkillsView();

        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnCustomDrawSkillsList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnLeftClickSkillsList(NMHDR*, LRESULT* pResult);
        afx_msg void OnRightClickSkillsList(NMHDR*, LRESULT* pResult);
        afx_msg void OnItemchangedSkillsList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnButtonMaxThisSkill();
        afx_msg void OnButtonClearThisSkill();
        afx_msg void OnButtonClearAllSkills();
        afx_msg void OnButtonAutoSpendSkillPoints();
        afx_msg LRESULT OnUpdateView(WPARAM wParam, LPARAM lParam);
        afx_msg void OnColumnClickSkillsList(NMHDR* pNMHDR, LRESULT* pResult);
        DECLARE_MESSAGE_MAP()

    private:
        // all things that can effect skills
        virtual void UpdateSkillSpendChanged(Character * charData, size_t level, SkillType skill) override;
        virtual void UpdateSkillTomeChanged(Character * charData, SkillType skill) override;
        virtual void UpdateClassChoiceChanged(Character * charData) override;
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateAbilityValueChanged(Character * charData, AbilityType ability) override;
        virtual void UpdateAbilityTomeChanged(Character * charData, AbilityType ability) override;
        virtual void UpdateRaceChanged(Character * charData, RaceType race) override;

        CDocument * m_pDocument;
        Character * m_pCharacter;
        void PopulateItems();
        CPoint IdentifyItemUnderMouse(bool wasClick);
        void MaxThisSkill(SkillType skill, bool suppressUpdate);
        void EnableControls();
        CListCtrl m_skillsList;
        CSkillsHeaderCtrl m_skillsHeader;
        CButton m_buttonMaxThisSkill;
        CButton m_buttonClearThisSkill;
        CButton m_buttonClearAllSkills;
        CButton m_buttonAutoBuySkills;
        CImageList m_classIcons;
        int m_hlightlightColumn;    // column mouse is over
        int m_hlightlightRow;       // row the mouse is over
        bool m_bUpdatePending;
        int m_selectedColumn;       // column whos button has been clicked on
};
