// EquipmentView.h
//
#pragma once
#include "Resource.h"
#include "Character.h"
#include "InventoryDialog.h"
#include "StanceButton.h"

class CEquipmentView :
    public CFormView,
    public InventoryObserver,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_EQUIPMENT_VIEW };
        DECLARE_DYNCREATE(CEquipmentView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CEquipmentView();           // protected constructor used by dynamic creation
        virtual ~CEquipmentView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnGearNew();
        afx_msg void OnGearCopy();
        afx_msg void OnGearPaste();
        afx_msg void OnGearDelete();
        afx_msg void OnGearImport();
        afx_msg void OnUpdateGearNew(CCmdUI * pCmdUi);
        afx_msg void OnUpdateGearCopy(CCmdUI * pCmdUi);
        afx_msg void OnUpdateGearPaste(CCmdUI * pCmdUi);
        afx_msg void OnUpdateGearDelete(CCmdUI * pCmdUi);
        afx_msg void OnUpdateGearimport(CCmdUI * pCmdUi);
        afx_msg void OnGearSelectionSelEndOk();
        afx_msg void OnGearNumFiligreesSelEndOk();
        afx_msg void OnToggleFiligreeMenu();
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

        // InventoryObserver overrides
        virtual void UpdateSlotLeftClicked(CInventoryDialog * dialog, InventorySlotType slot) override;
        virtual void UpdateSlotRightClicked(CInventoryDialog * dialog, InventorySlotType slot) override;

        // character overrides
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot) override;
        virtual void UpdateNewStance(Character * charData, const Stance & stance) override;
        virtual void UpdateRevokeStance(Character * charData, const Stance & stance) override;
        virtual void UpdateRaceChanged(Character * charData, RaceType race) override;

    private:
        void PopulateCombobox();
        void EnableControls();
        void PopulateGear();
        std::string SelectedGearSet() const;
        void ShowTip(const CStanceButton & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const CStanceButton & item, CPoint tipTopLeft, CPoint tipAlternate);
        void DestroyAllSets();
        void AddStance(const Stance & stance);
        void RevokeStance(const Stance & stance);

        CComboBox m_comboGearSelections;
        CMFCButton m_buttonNew;
        CMFCButton m_buttonCopy;
        CMFCButton m_buttonPaste;
        CMFCButton m_buttonDelete;
        CMFCButton m_buttonImport;
        CStatic m_staticNumFiligrees;
        CComboBox m_comboNumFiligrees;
        CButton m_filigreeMenu;
        CInventoryDialog * m_inventoryView;
        std::vector<CStanceButton *> m_setsStancebuttons;
        const CStanceButton * m_pTooltipItem;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        int m_nextSetId;

        CDocument * m_pDocument;
        Character * m_pCharacter;
};
