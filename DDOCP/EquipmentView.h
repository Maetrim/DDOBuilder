// EquipmentView.h
//
#pragma once
#include "Resource.h"
#include "InventoryDialog.h"

class Character;

class CEquipmentView :
    public CFormView,
    public InventoryObserver
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
        afx_msg void OnButtonNew();
        afx_msg void OnButtonCopy();
        afx_msg void OnButtonDelete();
        afx_msg void OnButtonActiveGearSet();
        DECLARE_MESSAGE_MAP()

        // InventoryObserver overrides
        virtual void UpdateSlotLeftClicked(CInventoryDialog * dialog, InventorySlotType slot) override;
        virtual void UpdateSlotRightClicked(CInventoryDialog * dialog, InventorySlotType slot) override;
    private:
        void PopulateCombobox();
        void EnableControls();
        void PopulateGear();
        std::string SelectedGearSet() const;
        CComboBox m_comboGearSelections;
        CButton m_buttonNew;
        CButton m_buttonCopy;
        CButton m_buttonDelete;
        CButton m_buttonActiveGearSet;
        CInventoryDialog * m_inventoryView;

        CDocument * m_pDocument;
        Character * m_pCharacter;
};
