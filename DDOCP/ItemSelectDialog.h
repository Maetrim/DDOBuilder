// ItemSelectDialog.h
#pragma once
#include "Resource.h"
#include "RaceTypes.h"
#include "InventorySlotTypes.h"
#include "Item.h"
#include "PersistantSize.h"
#include "SortHeaderCtrl.h"
#include "InfoTip.h"
#include "ComboBoxTooltip.h"

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
        afx_msg void OnSelEndOkPersonality();
        afx_msg void OnUpgradeFiligree(UINT nID);
        afx_msg void OnUpgradeFiligreeCancel(UINT nID);
        afx_msg void OnUpgradeFiligreeRare(UINT nID);
        afx_msg void OnKillFocusAugmentEdit(UINT nID);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
        afx_msg void OnEndtrackListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnColumnclickListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnHoverListItems(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnSelEndOkFilter();
        afx_msg void OnButtonSentientJewel();
        afx_msg LRESULT OnHoverComboBox(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        afx_msg void OnWindowPosChanging(WINDOWPOS * pos);
    public:
        DECLARE_MESSAGE_MAP()

    private:
        static int CALLBACK SortCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        void PopulateAvailableItemList();
        void EnableControls();
        void PopulateAugmentList(
                CComboBox * combo,
                CEdit * edit,
                const ItemAugment & augment);
        void PopulateSlotUpgradeList(size_t controlIndex, const SlotUpgrade & upgrade);
        void PopulateDropList(size_t controlIndex, const std::list<std::string> & types);

        void ShowTip(const Item & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const Item & item, CPoint tipTopLeft, CPoint tipAlternate);
        void SetTooltipText(const Augment & augment, CPoint tipTopLeft, CPoint tipAlternate, bool rightAlign);
        void SetupFilterCombobox();
        void AddSpecialSlots();
        void SetSentientWeaponControls();
        void PopulatePersonalityCombobox();
        void PopulateFiligreeCombobox(size_t filigreeIndex);
        void BuildImageList(CImageList * il, const std::list<Augment> & augments);

        InventorySlotType m_slot;
        Item m_item;
        RaceType m_race;

        enum
        {
            MAX_Augments = 10,
            MAX_Upgrades = 4,
            MAX_Filigree = MAX_FILIGREE,
        };
        CStatic m_staticType;
        CComboBox m_comboFilter;
        CListCtrl m_availableItemsCtrl;
        CSortHeaderCtrl m_sortHeader;
        CStatic m_staticAugments;
        CStatic m_augmentType[MAX_Augments];
        CComboBox m_comboAugmentDropList[MAX_Augments];
        CEdit m_augmentValues[MAX_Augments];
        CStatic m_staticUpgrades;
        CStatic m_upgradeType[MAX_Upgrades];
        CComboBox m_comboUpgradeDropList[MAX_Upgrades];
        CButton m_buttonSentientJewel;
        CStatic m_sentientLabel;
        CComboBoxTooltip m_comboSentientPersonality;
        CImageList m_ilJewel;
        CComboBoxTooltip m_comboFiligreeDropList[MAX_Filigree];
        CButton m_buttonFiligreeRare[MAX_Filigree];
        CImageList m_ilFiligree[MAX_Filigree];

        std::list<Item> m_availableItems;
        CImageList m_itemImages;
        bool m_bInitialising;
        CPersistantSize m_sizer;
        ArmorType m_armorType;
        WeaponType m_weaponType;
        // item tooltips
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        int m_hoverItem;
        UINT m_hoverHandle;
        int m_comboHookHandles[MAX_Filigree + 1];
};
