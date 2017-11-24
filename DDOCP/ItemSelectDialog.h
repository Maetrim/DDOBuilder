// ItemSelectDialog.h
#pragma once
#include "Resource.h"
#include "RaceTypes.h"
#include "InventorySlotTypes.h"
#include "Item.h"
#include "PersistantSize.h"
#include "SortHeaderCtrl.h"
#include "InfoTip.h"

class CItemSelectDialog : public CDialog
{
    public:
        CItemSelectDialog(CWnd* pParent, InventorySlotType slot, const Item & item, RaceType race);   // standard constructor
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
        afx_msg void OnUpgradeSelect(UINT nID);
        afx_msg void OnKillFocusAugmentEdit(UINT nID);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
        afx_msg void OnEndtrackListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnColumnclickListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnHoverListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        afx_msg void OnRadioCloth();
        afx_msg void OnRadioLight();
        afx_msg void OnRadioMedium();
        afx_msg void OnRadioHeavy();
        afx_msg void OnRadioDocent();
    public:
        DECLARE_MESSAGE_MAP()

    private:
        static int CALLBACK SortCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        void PopulateAvailableItemList();
        void EnableControls();
        void PopulateAugmentList(
                CComboBoxEx * combo,
                CEdit * edit,
                const ItemAugment & augment);
        void PopulateSlotUpgradeList(size_t controlIndex, const SlotUpgrade & upgrade);
        void PopulateDropList(size_t controlIndex, const std::list<std::string> & types);

        void ShowTip(const Item & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const Item & item, CPoint tipTopLeft, CPoint tipAlternate);
        void SetArmorButtonStates();
        void AddSpecialSlots();

        InventorySlotType m_slot;
        Item m_item;
        RaceType m_race;

        enum
        {
            MAX_Augments = 10,
            MAX_Upgrades = 4
        };
        CStatic m_staticType;
        CButton m_buttonCloth;
        CButton m_buttonLight;
        CButton m_buttonMedium;
        CButton m_buttonHeavy;
        CButton m_buttonDocent;
        CListCtrl m_availableItemsCtrl;
        CSortHeaderCtrl m_sortHeader;
        CStatic m_augmentType[MAX_Augments];
        CComboBoxEx m_comboAugmentDropList[MAX_Augments];
        CEdit m_augmentValues[MAX_Augments];
        CStatic m_upgradeType[MAX_Upgrades];
        CComboBoxEx m_comboUpgradeDropList[MAX_Upgrades];
        std::list<Item> m_availableItems;
        CImageList m_itemImages;
        bool m_bInitialising;
        CPersistantSize m_sizer;
        ArmorType m_armorType;
        // item tooltips
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        int m_hoverItem;
        UINT m_hoverHandle;
};
