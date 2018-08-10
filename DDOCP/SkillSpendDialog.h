// SkillSpendDialog.h
//

#pragma once

#include "Resource.h"
#include "SkillTypes.h"

class Character;

class CSkillSpendDialog :
    public CDialog
{
    public:
        CSkillSpendDialog(Character * pCharacter);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CSkillSpendDialog)
        enum { IDD = IDD_SKILL_SPEND_DIALOG };
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CSkillSpendDialog)
        virtual void DoDataExchange(CDataExchange* pDX);
        virtual BOOL OnInitDialog();
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CSkillSpendDialog)
        afx_msg void OnCustomDrawSkillsList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnLeftClickSkillsList(NMHDR*, LRESULT* pResult);
        afx_msg void OnRightClickSkillsList(NMHDR*, LRESULT* pResult);
        afx_msg void OnItemchangedSkillsList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnButtonMaxThisSkill();
        afx_msg void OnButtonClearThisSkill();
        afx_msg void OnButtonClearAllSkills();
        afx_msg void OnButtonAutoSpendSkillPoints();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void PopulateItems();
        CPoint IdentifyItemUnderMouse(bool wasClick);
        void MaxThisSkill(SkillType skill, bool suppressUpdate);
        CListCtrl m_skillsList;
        CButton m_buttonMaxThisSkill;
        CButton m_buttonClearThisSkill;
        CButton m_buttonClearAllSkills;
        Character * m_pCharacter;
        CImageList m_classIcons;
        int m_hlightlightColumn;
        int m_hlightlightRow;
};

//{{AFX_INSERT_LOCATION}}
