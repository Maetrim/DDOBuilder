// ItemSelectDialog.h
#pragma once
#include "Resource.h"
#include "InventorySlotTypes.h"
#include "Item.h"

class CItemSelectDialog : public CDialog
{
    public:
        CItemSelectDialog(CWnd* pParent, InventorySlotType slot, const Item & item);   // standard constructor
        virtual ~CItemSelectDialog();

        Item SelectedItem();

        // Dialog Data
        enum { IDD = IDD_ITEM_SELECT };

        DECLARE_DYNAMIC(CItemSelectDialog)

    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        afx_msg void OnItemSelected(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnAugmentSelect(UINT nID);

    public:
        DECLARE_MESSAGE_MAP()

    private:
        void PopulateAvailableItemList();
        void EnableControls();
        void PopulateAugmentList(
                CComboBoxEx * combo,
                AugmentType type,
                const std::string & selectedAugment);
        InventorySlotType m_slot;
        Item m_item;

        enum
        {
            MAX_Augments = 10
        };
        CStatic m_staticType;
        CListCtrl m_availableItemsCtrl;
        CStatic m_augmentType[MAX_Augments];
        CComboBoxEx m_comboAugmentDropList[MAX_Augments];
        std::list<Item> m_availableItems;
        CImageList m_itemImages;
        bool m_bInitialising;
};
