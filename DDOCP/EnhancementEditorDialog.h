// EnhancementEditorDialog.h
#pragma once

class Character;
class CEnhancementTreeDialog;

class CEnhancementEditorDialog : public CDialog
{
    public:
        CEnhancementEditorDialog(CWnd* pParent, Character * pCharacter);   // standard constructor
        virtual ~CEnhancementEditorDialog();

        // Dialog Data
        enum { IDD = IDD_ENHANCEMENT_TREE_EDITOR };

        DECLARE_DYNAMIC(CEnhancementEditorDialog)

    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();

    public:
        afx_msg void OnClickedButtonBrowse();
        afx_msg void OnClickedButtonSave();
        afx_msg void OnClickedButtonDelete();
        afx_msg void OnClickedCheckArrowleft();
        afx_msg void OnClickedCheckArrowright();
        afx_msg void OnClickedCheckArrowup();
        afx_msg void OnClickedCheckLongarrowup();
        afx_msg void OnClickedCheckExtraLongarrowup();
        afx_msg void OnKillFocusEditTitle();
        afx_msg void OnKillFocusEditInternalName();
        afx_msg void OnKillFocusEditDescription();
        afx_msg void OnKillFocusEditCost();
        afx_msg void OnKillFocusEditRanks();
        afx_msg void OnKillFocusEditMinSpent();
        afx_msg void OnSelendokEnhancement();
        afx_msg void OnClickedCheckClickie();
        afx_msg void OnKillfocusEditIcon();
        afx_msg void OnClickedButtonAddFeatRequirement();
        afx_msg void OnSelendokComboNumSelections();
        afx_msg void OnSelendokComboEditSelection();
        afx_msg void OnKillfocusEditIcon2();
        afx_msg void OnKillfocusEditSelectionDescription();
        afx_msg void OnKillfocusEditCost2();
        afx_msg void OnKillfocusEditSelectionTitle();
        afx_msg void OnClickedButtonAddFeatRequirementSelection();
        afx_msg void OnClickedButtonAddClassrequirement();
        DECLARE_MESSAGE_MAP()

    private:
        void PopulateComboList();
        void UpdateTree();
        std::list<EnhancementTreeItem>::iterator FindItem(std::list<EnhancementTreeItem> & items, size_t x, size_t y);
        void SetupSelections();
        void PopulateSelection();

        std::list<EnhancementTree> m_loadedTrees;
        Character * m_pCharacter;

        CEdit m_editInternalName;
        CEdit m_editMinSpent;
        CEdit m_editRanks;
        CEdit m_editFilename;
        CEdit m_editDescription;
        CEdit m_editCost;
        CEdit m_editTitle;
        CComboBox m_comboList;
        CEnhancementTreeDialog * m_pTreeDialog;
        CButton m_buttonArrowLeft;
        CButton m_buttonArrowRight;
        CButton m_buttonArrowUp;
        CButton m_buttonLongArrowUp;
        CButton m_buttonExtraLongArrowUp;
        CButton m_buttonClickie;
        CEdit m_editIcon;
        CEdit m_editFeatName;
        CEdit m_editClass;
        CEdit m_editClassLevel;
        CComboBox m_comboNumSelections;
        CComboBox m_comboSelection;
        CEdit m_editSelectionTitle;
        CEdit m_editSelectionDescription;
        CEdit m_editSelectionIcon;
        CEdit m_editSelectionCost;
        CEdit m_editFeatSelection;
        CButton m_buttonAddFeatRequirementSelection;
public:
    afx_msg void OnClickedButtonicon1Transparent();
    afx_msg void OnClickedButtonicon2Transparent();
    void MakeIconTransparent(CString name);
};
