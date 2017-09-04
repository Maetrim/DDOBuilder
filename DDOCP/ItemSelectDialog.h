// ItemSelectDialog.h
#pragma once
#include "Resource.h"
#include "InventorySlotTypes.h"
#include "Item.h"
#include "PersistantSize.h"
#include "SortHeaderCtrl.h"
#include "InfoTip.h"

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
        afx_msg void OnUpgradeSelect(UINT nID);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
        afx_msg void OnEndtrackListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnColumnclickListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnHoverListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

    public:
        DECLARE_MESSAGE_MAP()

    private:
        static int CALLBACK SortCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        void PopulateAvailableItemList();
        void EnableControls();
        void PopulateAugmentList(
                CComboBoxEx * combo,
                AugmentType type,
                const std::string & selectedAugment);
        void PopulatePrimaryUpgradeList(size_t controlIndex);
        void PopulateSecondaryUpgradeList(size_t controlIndex);
        void PopulateLegendarySlavelordUpgradeList(size_t controlIndex);
        void PopulateDropList(size_t controlIndex, const std::list<AugmentType> & types);

        void ShowTip(const Item & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const Item & item, CPoint tipTopLeft, CPoint tipAlternate);

        InventorySlotType m_slot;
        Item m_item;

        enum
        {
            MAX_Augments = 10,
            MAX_Upgrades = 4
        };
        enum UpgradeType
        {
            Upgrade_Primary,                // epic orchard items
            Upgrade_Secondary,              // epic orchard items
            Upgrade_LegendarySlavelords,    // legendary slave lord items
        };
        CStatic m_staticType;
        CListCtrl m_availableItemsCtrl;
        CSortHeaderCtrl m_sortHeader;
        CStatic m_augmentType[MAX_Augments];
        CComboBoxEx m_comboAugmentDropList[MAX_Augments];
        CStatic m_upgradeType[MAX_Upgrades];
        CComboBoxEx m_comboUpgradeDropList[MAX_Upgrades];
        std::list<Item> m_availableItems;
        CImageList m_itemImages;
        bool m_bInitialising;
        UpgradeType m_upgradeTypeModelled[MAX_Upgrades];
        CPersistantSize m_sizer;
        // item tooltips
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        int m_hoverItem;
        UINT m_hoverHandle;
};
