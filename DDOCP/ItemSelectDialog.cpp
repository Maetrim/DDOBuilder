// ItemSelectDialog.cpp
//

#include "stdafx.h"
#include "ItemSelectDialog.h"
#include "Character.h"
#include "GlobalSupportFunctions.h"
#include "MouseHook.h"

// CItemSelectDialog dialog

namespace
{
    enum ItemListColumns
    {
        ILC_name = 0,
        ILC_level,
    };
}

IMPLEMENT_DYNAMIC(CItemSelectDialog, CDialog)

CItemSelectDialog::CItemSelectDialog(
        CWnd* pParent,
        InventorySlotType slot,
        const Item & item,
        Character * pCharacter) :
    CDialog(CItemSelectDialog::IDD, pParent),
    m_slot(slot),
    m_item(item),
    m_pCharacter(pCharacter),
    m_bInitialising(false),
    m_showingTip(false),
    m_tipCreated(false),
    m_hoverItem(-1),
    m_hoverHandle(0),
    m_armorType(Armor_Unknown),
    m_weaponType(Weapon_Unknown),
    m_bIgnoreNextMessage(false)
{
    if (m_item.HasArmor())
    {
        m_armorType = m_item.Armor();
    }
    if (m_item.HasWeapon())
    {
        m_weaponType = m_item.Weapon();
    }
    // race overrides current armor type
    RaceType race = m_pCharacter->Race();
    if (race == Race_Warforged
            || race == Race_BladeForged)
    {
        m_armorType = Armor_Docent;
    }
}

CItemSelectDialog::~CItemSelectDialog()
{
}

void CItemSelectDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_FILTER, m_comboFilter);
    DDX_Control(pDX, IDC_ITEM_TYPE, m_staticType);
    DDX_Control(pDX, IDC_EDIT_TEXT, m_editSearchText);
    DDX_Control(pDX, IDC_BUTTON_FILTER, m_buttonFilter);
    DDX_Control(pDX, IDC_ITEM_LIST, m_availableItemsCtrl);
    if (!pDX->m_bSaveAndValidate)
    {
        VERIFY(m_sortHeader.SubclassWindow(m_availableItemsCtrl.GetHeaderCtrl()->GetSafeHwnd()));
    }
    DDX_Control(pDX, IDC_STATIC_AUGMENTS, m_staticAugments);
    for (size_t i = 0; i < MAX_Augments; ++i)
    {
        DDX_Control(pDX, IDC_STATIC_AUGMENT_TYPE1 + i, m_augmentType[i]);
        DDX_Control(pDX, IDC_COMBO_AUGMENT1 + i, m_comboAugmentDropList[i]);
        DDX_Control(pDX, IDC_EDIT_AUGMENT1 + i, m_augmentValues[i]);
    }
    DDX_Control(pDX, IDC_STATIC_UPGRADES, m_staticUpgrades);
    for (size_t i = 0; i < MAX_Upgrades; ++i)
    {
        DDX_Control(pDX, IDC_STATIC_UPGRADE_TYPE1 + i, m_upgradeType[i]);
        DDX_Control(pDX, IDC_COMBO_UPGRADE1 + i, m_comboUpgradeDropList[i]);
    }
}

BEGIN_MESSAGE_MAP(CItemSelectDialog, CDialog)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_LIST, OnItemSelected)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_AUGMENT1, IDC_COMBO_AUGMENT1 + MAX_Augments - 1, OnAugmentSelect)
    ON_CONTROL_RANGE(CBN_SELENDCANCEL, IDC_COMBO_AUGMENT1, IDC_COMBO_AUGMENT1 + MAX_Augments - 1, OnAugmentCancel)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_UPGRADE1, IDC_COMBO_UPGRADE1 + MAX_Upgrades - 1, OnUpgradeSelect)
    ON_CONTROL_RANGE(CBN_SELENDCANCEL, IDC_COMBO_UPGRADE1, IDC_COMBO_UPGRADE1 + MAX_Upgrades - 1, OnUpgradeCancel)
    ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_AUGMENT1, IDC_EDIT_AUGMENT1 + MAX_Augments - 1, OnKillFocusAugmentEdit)
    ON_CBN_SELENDOK(IDC_COMBO_FILTER, OnSelEndOkFilter)
    ON_WM_SIZE()
    ON_WM_WINDOWPOSCHANGING()
    ON_NOTIFY(HDN_ENDTRACK, IDC_ITEM_LIST, OnEndtrackListItems)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, IDC_ITEM_LIST, OnEndtrackListItems)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_ITEM_LIST, OnColumnclickListItems)
    ON_NOTIFY(NM_HOVER, IDC_ITEM_LIST, OnHoverListItems)
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_MESSAGE(WM_MOUSEHOVER, OnHoverComboBox)
    ON_MESSAGE(WM_MOUSEENTER, OnMouseEnter)
    ON_EN_KILLFOCUS(IDC_EDIT_TEXT, OnSearchTextKillFocus)
END_MESSAGE_MAP()

// CItemSelectDialog message handlers
BOOL CItemSelectDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_bInitialising = true;

    m_tooltip.Create(this);
    m_tipCreated = true;

    // show item type of selection
    CString text;
    switch(m_slot)
    {
    case Inventory_Arrows: text = "Arrows"; break;
    case Inventory_Armor: text = "Armor"; break;
    case Inventory_Belt: text = "Belt"; break;
    case Inventory_Boots: text = "Boots"; break;
    case Inventory_Bracers: text = "Bracers"; break;
    case Inventory_Cloak: text = "Cloak"; break;
    case Inventory_Gloves: text = "Gloves"; break;
    case Inventory_Goggles: text = "Goggles"; break;
    case Inventory_Helmet: text = "Helm"; break;
    case Inventory_Necklace: text = "Necklace"; break;
    case Inventory_Quiver: text = "Quiver"; break;
    case Inventory_Ring1:
    case Inventory_Ring2: text = "Ring"; break;
    case Inventory_Trinket: text = "Trinket"; break;
    case Inventory_Weapon1: text = "Weapon"; break;
    case Inventory_Weapon2: text = "Weapon, Shield, Orb"; break;
    }
    SetupFilterCombobox();
    // add list control columns
    m_availableItemsCtrl.InsertColumn(0, "Item Name", LVCFMT_LEFT, 150);
    m_availableItemsCtrl.InsertColumn(1, "Level", LVCFMT_LEFT, 50);
    m_sortHeader.SetSortArrow(1, FALSE);     // sort by level by default

    PopulateAvailableItemList();
    m_staticType.SetWindowText(text);
    m_availableItemsCtrl.SetExtendedStyle(
            m_availableItemsCtrl.GetExtendedStyle()
            | LVS_EX_FULLROWSELECT
            | LVS_EX_TRACKSELECT
            | LVS_EX_ONECLICKACTIVATE
            //| LVS_EX_LABELTIP); // stop hover tooltips from working
            );
    EnableControls();

    // add monitor locations to show augment tooltips
    for (size_t i = 0 ; i < MAX_Augments; ++i)
    {
        CRect rect;
        m_comboAugmentDropList[i].GetWindowRect(&rect);
        rect.DeflateRect(0, 1, 0, 1);   // ensure they do not overlap
        m_augmentHookHandles[i] = GetMouseHook()->AddRectangleToMonitor(
                this->GetSafeHwnd(),
                rect,           // screen coordinates,
                WM_MOUSEENTER,
                WM_MOUSELEAVE,
                false);
    }

    LoadColumnWidthsByName(&m_availableItemsCtrl, "ItemSelectDialog_%s");
    m_bInitialising = false;
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

Item CItemSelectDialog::SelectedItem()
{
    return m_item;
}

void CItemSelectDialog::PopulateAvailableItemList()
{
    m_availableItemsCtrl.LockWindowUpdate();
    m_availableItemsCtrl.DeleteAllItems();

    CString searchText;
    m_editSearchText.GetWindowText(searchText);
    searchText.MakeLower(); // case less text match

    // need to know how many levels and of what classes they have trained
    std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
    // need to know which feats have already been trained by this point
    // include any feats also trained at the current level
    std::list<TrainedFeat> currentFeats = m_pCharacter->CurrentFeats(MAX_LEVEL);

    RaceType race = m_pCharacter->Race();
    // filter the list of items loaded to those that match the slot type
    const std::list<Item> & allItems = Items();
    m_availableItems.clear();
    std::list<Item>::const_iterator it = allItems.begin();
    while (it != allItems.end())
    {
        if ((*it).CanEquipToSlot(m_slot))
        {
            // if its armor or a weapon, we may need to filter items further
            bool canSelect = true;  // assume
            switch (m_slot)
            {
            case Inventory_Armor:
                canSelect = (m_armorType == Armor_Unknown)
                        || (m_armorType == (*it).Armor());
                if (m_armorType == Armor_Unknown
                        && (*it).Armor() == Armor_Docent)
                {
                    if (race != Race_Warforged && race != Race_BladeForged)
                    {
                        // can't select docents for non-forged characters
                        canSelect = false;
                    }
                }
                break;
            case Inventory_Weapon1:
            case Inventory_Weapon2:
                canSelect = (m_weaponType == Weapon_Unknown)
                        || (m_weaponType == (*it).Weapon());
                break;
            }
            // must have the required search text present in the item
            if (searchText.GetLength() > 0)
            {
                canSelect &= (*it).ContainsSearchText((LPCTSTR)searchText);
            }
            // need to include the selected item in the list regardless of
            // filter category
            if ((*it).Name() == m_item.Name())
            {
                // always include current selection
                canSelect = true;
            }
            // some items have requirements to be able to use, see if they are met
            if ((*it).HasRequirementsToUse())
            {
                if (!(*it).RequirementsToUse().Met(*m_pCharacter, classLevels, MAX_LEVEL, currentFeats, true))
                {
                    canSelect = false;
                }
            }
            if (canSelect)
            {
                // user can select this item, add it to the available list
                m_availableItems.push_back((*it));
            }
        }
        ++it;
    }
    // now we have the list, build an image list of the item icons
    m_itemImages.DeleteImageList();
    m_itemImages.Create(32, 32, ILC_COLOR32, 0, m_availableItems.size());
    // now add the images to the image list
    it = m_availableItems.begin();
    while (it != m_availableItems.end())
    {
        (*it).AddImage(&m_itemImages);
        ++it;
    }
    m_availableItemsCtrl.SetImageList(&m_itemImages, LVSIL_SMALL);
    m_availableItemsCtrl.SetImageList(&m_itemImages, LVSIL_NORMAL);

    // now populate the control
    size_t itemIndex = 0;
    it = m_availableItems.begin();
    while (it != m_availableItems.end())
    {
        int item = m_availableItemsCtrl.InsertItem(
                m_availableItemsCtrl.GetItemCount(),
                (*it).Name().c_str(),
                itemIndex);
        CString level;
        level.Format("%d", (*it).MinLevel());
        m_availableItemsCtrl.SetItemText(item, 1, level);
        m_availableItemsCtrl.SetItemData(item, itemIndex);

        ++itemIndex;
        ++it;
    }

    m_availableItemsCtrl.SortItems(
            CItemSelectDialog::SortCompareFunction,
            (long)GetSafeHwnd());

    CString text;
    text.Format("Item Selection and Configuration - %s",
            m_item.Name().c_str());
    SetWindowText(text);
    int sel = -1;           // item to be selected
    LVFINDINFO fi;
    fi.flags = LVFI_STRING;
    fi.psz = m_item.Name().c_str();
    sel = m_availableItemsCtrl.FindItem(&fi);
    if (sel >= 0)
    {
        m_availableItemsCtrl.SetSelectionMark(sel);
        m_availableItemsCtrl.SetItemState(sel, LVIS_SELECTED, LVIS_SELECTED);
        m_availableItemsCtrl.EnsureVisible(sel, FALSE);
    }
    else
    {
        // ok not available until item selected
        GetDlgItem(IDOK)->EnableWindow(FALSE);
        // show the name of the selected item in the dialog title
        SetWindowText("Item Selection and Configuration - No item selected");
    }
    m_availableItemsCtrl.UnlockWindowUpdate();
    HideTip();
}

void CItemSelectDialog::EnableControls()
{
    // show / hide the available augments based on the selected item
    std::vector<ItemAugment> augments = m_item.Augments();
    ASSERT(augments.size() < MAX_Augments);

    m_staticAugments.ShowWindow(augments.size() > 0 ? SW_SHOW : SW_HIDE);
    for (size_t i = 0; i < MAX_Augments; ++i)
    {
        if (i < augments.size())
        {
            // we have an augment, populate it with the available for this type
            m_augmentType[i].SetWindowText(augments[i].Type().c_str());
            PopulateAugmentList(
                    &m_comboAugmentDropList[i],
                    &m_augmentValues[i],
                    augments[i]);
            m_augmentType[i].ShowWindow(SW_SHOW);
            m_comboAugmentDropList[i].ShowWindow(SW_SHOW);
        }
        else
        {
            // no augment, hide control
            m_augmentType[i].ShowWindow(SW_HIDE);
            m_comboAugmentDropList[i].ShowWindow(SW_HIDE);
            m_augmentValues[i].ShowWindow(SW_HIDE);
        }
    }
    // now show any upgrade slots
    size_t upgradeIndex = 0;
    std::vector<SlotUpgrade> upgrades = m_item.SlotUpgrades();
    m_staticUpgrades.ShowWindow(upgrades.size() > 0 ? SW_SHOW : SW_HIDE);
    for (; upgradeIndex < upgrades.size(); ++upgradeIndex)
    {
        // we have a primary upgrade list
        PopulateSlotUpgradeList(upgradeIndex, upgrades[upgradeIndex]);
    }
    // now hide the unused
    for (;upgradeIndex < MAX_Upgrades; ++upgradeIndex)
    {
        m_upgradeType[upgradeIndex].ShowWindow(SW_HIDE);
        m_comboUpgradeDropList[upgradeIndex].ShowWindow(SW_HIDE);
    }
}

void CItemSelectDialog::PopulateAugmentList(
        CComboBox * combo,
        CEdit * edit,
        const ItemAugment & augment)
{
    int sel = CB_ERR;
    combo->LockWindowUpdate();
    combo->ResetContent();
    // get all the augments compatible with this slot type
    std::vector<Augment> augments = CompatibleAugments(augment.Type());
    std::vector<Augment>::const_iterator it = augments.begin();
    std::string selectedAugment = augment.HasSelectedAugment()
            ? augment.SelectedAugment()
            : "";

    // note that this list can be sorted
    int index = 0;
    while (it != augments.end())
    {
        int itemIndex = combo->AddString((*it).Name().c_str());
        combo->SetItemData(itemIndex, index);
        ++it;
        ++index;
    }

    if (selectedAugment != "")
    {
        combo->SelectString(0, selectedAugment.c_str());
        Augment selAugment = FindAugmentByName(selectedAugment);
        // this augment is configurable by the user
        if (selAugment.HasEnterValue())
        {
            edit->ShowWindow(SW_SHOW);
            // show the value of this augment in the control
            CString text;
            text.Format("%.0f", augment.HasValue() ? augment.Value() : 0);
            edit->SetWindowText(text);
        }
        else
        {
            // augment has a fixed bonus
            edit->ShowWindow(SW_HIDE);
        }
    }
    else
    {
        // no augment selected
        edit->ShowWindow(SW_HIDE);
    }
    combo->UnlockWindowUpdate();
}

void CItemSelectDialog::OnItemSelected(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (!m_bInitialising)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        if ((pNMListView->uChanged & LVIF_STATE) 
                && (pNMListView->uNewState & LVIS_SELECTED))
        {
            // item selection has changed, select it
            int sel = pNMListView->iItem;
            sel = m_availableItemsCtrl.GetItemData(sel);
            if (sel >= 0 && sel < (int)m_availableItems.size())
            {
                // must be a different item to the one already selected
                std::list<Item>::const_iterator it = m_availableItems.begin();
                std::advance(it, sel);
                if ((*it).Name() != m_item.Name())
                {
                    m_item = (*it);
                    AddSpecialSlots();  // adds reaper or mythic slots to the item
                    // update the other controls
                    EnableControls();
                    // item selected, can now click ok!
                    GetDlgItem(IDOK)->EnableWindow(TRUE);
                    CString text;
                    text.Format("Item Selection and Configuration - %s",
                            m_item.Name().c_str());
                    SetWindowText(text);
                }
            }
        }
    }
    *pResult = 0;
}

void CItemSelectDialog::OnAugmentSelect(UINT nID)
{
    // the user has selected an augment in one of the augment combo boxes
    int augmentIndex = nID - IDC_COMBO_AUGMENT1;
    ASSERT(augmentIndex >= 0 && augmentIndex < (int)m_item.Augments().size());
    int sel = m_comboAugmentDropList[augmentIndex].GetCurSel();
    if (sel != CB_ERR)
    {
        std::vector<ItemAugment> augments = m_item.Augments();
        // user has selected an augment. Get its name
        CString text;
        m_comboAugmentDropList[augmentIndex].GetLBText(sel, text);
        if (text == " No Augment")
        {
            // "No Augment" select, clear any augment
            augments[augmentIndex].Clear_SelectedAugment();
            augments[augmentIndex].Clear_Value();
        }
        else
        {
            // may need to clear previous augments granted augment slot
            std::string oldSelection;
            if (augments[augmentIndex].HasSelectedAugment())
            {
                oldSelection = augments[augmentIndex].SelectedAugment();
            }
            augments[augmentIndex].Set_SelectedAugment((LPCTSTR)text);
            augments[augmentIndex].Clear_Value(); // if we just selected an augment ensure any previous value does not carry over
            if (oldSelection != "")
            {
                const Augment & oldAugment =
                        FindAugmentByName(oldSelection);
                if (oldAugment.HasGrantAugment())
                {
                    RemoveAugment(&augments, oldAugment.GrantAugment());
                }
                // add a granted augment if required (only granted while augment selected)
                if (oldAugment.HasGrantConditionalAugment())
                {
                    if (oldAugment.HasWeaponClass()
                            && m_item.HasWeapon()
                            && IsInWeaponClass(oldAugment.WeaponClass(), m_item.Weapon()))
                    {
                        // this is a new augment slot that needs to be added
                        RemoveAugment(&augments, oldAugment.GrantConditionalAugment());
                    }
                }
            }
            // if an augment selected has AddAugment fields, add them to the
            // item also if they do not already exist
            const Augment & augment = FindAugmentByName((LPCTSTR)text);
            std::list<std::string> augmentsToAdd = augment.AddAugment();
            std::list<std::string>::const_iterator it = augmentsToAdd.begin();
            while (it != augmentsToAdd.end())
            {
                AddAugment(&augments, (*it));
                ++it;
            }
            // add a granted augment if required (only granted while augment selected)
            if (augment.HasGrantAugment())
            {
                // this is a new augment slot that needs to be added
                AddAugment(&augments, augment.GrantAugment());
            }
            // add a granted augment if required (only granted while augment selected)
            if (augment.HasGrantConditionalAugment())
            {
                if (augment.HasWeaponClass()
                        && m_item.HasWeapon()
                        && IsInWeaponClass(augment.WeaponClass(), m_item.Weapon()))
                {
                    // this is a new augment slot that needs to be added
                    AddAugment(&augments, augment.GrantConditionalAugment());
                }
            }
        }
        m_item.Set_Augments(augments);
        // update the controls after a selection as edit controls may need
        // to be displayed for some augment selection controls.
        EnableControls();
    }
    HideTip();
}

void CItemSelectDialog::OnAugmentCancel(UINT nID)
{
    HideTip();
    m_bIgnoreNextMessage = true;
}

void CItemSelectDialog::PopulateSlotUpgradeList(
        size_t controlIndex,
        const SlotUpgrade & upgrade)
{
    // set the text of the display item
    m_upgradeType[controlIndex].SetWindowText(upgrade.Type().c_str());
    // add the entries to the drop list control
    m_comboUpgradeDropList[controlIndex].ResetContent();
    std::vector<std::string> types = upgrade.UpgradeType();
    std::vector<std::string>::const_iterator it = types.begin();
    size_t index = 0;
    while (it != types.end())
    {
        size_t pos = m_comboUpgradeDropList[controlIndex].AddString((*it).c_str());
        m_comboUpgradeDropList[controlIndex].SetItemData(pos, index);
        // move on to the next item
        ++it;
        ++index;
    }
    // finally ensure the control is displayed
    m_upgradeType[controlIndex].ShowWindow(SW_SHOW);
    m_comboUpgradeDropList[controlIndex].ShowWindow(SW_SHOW);
}

void CItemSelectDialog::OnUpgradeSelect(UINT nID)
{
    // an upgrade selection has been made. Determine which augment slot type
    // needs to be added and remove the upgrade list from the item. This action
    // cannot be undone. The user will have to reselect the base item to undo this
    size_t controlIndex = nID - IDC_COMBO_UPGRADE1;
    int selection = m_comboUpgradeDropList[controlIndex].GetCurSel();
    if (selection != CB_ERR)
    {
        selection = m_comboUpgradeDropList[controlIndex].GetItemData(selection);
        // now find what was selected
        std::vector<SlotUpgrade> upgrades = m_item.SlotUpgrades();
        std::vector<SlotUpgrade>::iterator sit = upgrades.begin();
        std::advance(sit, controlIndex);

        // determine the augment type to add
        std::vector<std::string> augments = (*sit).UpgradeType();
        ASSERT(selection >= 0 && selection < (int)augments.size());
        std::vector<std::string>::iterator it = augments.begin();
        std::advance(it, selection);

        // add the selected augment type to the item
        std::vector<ItemAugment> currentAugments = m_item.Augments();
        AddAugment(&currentAugments, (*it));
        m_item.Set_Augments(currentAugments);

        // now erase the upgrade from the upgrade list as its been actioned
        upgrades.erase(sit);
        m_item.Set_SlotUpgrades(upgrades);

        // now get all controls re-repopulate
        EnableControls();
    }
    HideTip();
}

void CItemSelectDialog::OnUpgradeCancel(UINT nID)
{
    HideTip();
    m_bIgnoreNextMessage = true;
}

void CItemSelectDialog::OnKillFocusAugmentEdit(UINT nID)
{
    // user may have completed editing an edit field for a selectable augment
    // read the value and update if its changed
    CString text;
    size_t augmentIndex = nID - IDC_EDIT_AUGMENT1;
    m_augmentValues[augmentIndex].GetWindowText(text);
    double value = atof(text);
    std::vector<ItemAugment> augments = m_item.Augments();
    augments[augmentIndex].Set_Value(value);
    m_item.Set_Augments(augments);
    EnableControls();
}

void CItemSelectDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if (IsWindow(m_staticType.GetSafeHwnd()))
    {
        // assign half the space to each set of controls:
        // +----------------------++-------------------+
        // | Item select control  || Augments section  |
        // |                      ||                   |
        // +----------------------++-------------------+
        //                         [OK] [CANCEL]

        // TBD!
    }
    if (IsWindow(m_staticType.GetSafeHwnd()))
    {
        // update the mouse hook handles for tooltips
        CRect rect;
        for (size_t i = 0 ; i < MAX_Augments; ++i)
        {
            m_comboAugmentDropList[i].GetWindowRect(&rect);
            rect.DeflateRect(0, 1, 0, 1);   // ensure they do not overlap
            GetMouseHook()->UpdateRectangle(
                    m_augmentHookHandles[i],
                    rect);          // screen coordinates
        }
    }
}

void CItemSelectDialog::OnEndtrackListItems(NMHDR* pNMHDR, LRESULT* pResult)
{
    // just save the column widths to registry so restored next time we run
    UNREFERENCED_PARAMETER(pNMHDR);
    UNREFERENCED_PARAMETER(pResult);
    SaveColumnWidthsByName(&m_availableItemsCtrl, "ItemSelectDialog_%s");
}

void CItemSelectDialog::OnColumnclickListItems(NMHDR* pNMHDR, LRESULT* pResult)
{
    // allow the user to sort the item list based on the selected column
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    size_t columnToSort = pNMListView->iSubItem;
    int column;
    bool bAscending;
    m_sortHeader.GetSortArrow(&column, &bAscending);
    if (column == columnToSort)
    {
        // just change sort direction
        bAscending = !bAscending;
    }
    else
    {
        // just change the column
        column = columnToSort;
    }
    m_sortHeader.SetSortArrow(column, bAscending);
    m_availableItemsCtrl.SortItems(CItemSelectDialog::SortCompareFunction, (long)GetSafeHwnd());

    *pResult = 0;
}

int CItemSelectDialog::SortCompareFunction(
        LPARAM lParam1,
        LPARAM lParam2,
        LPARAM lParamSort)
{
    // this is a static function so we need to make our own this pointer
    CWnd * pWnd = CWnd::FromHandle((HWND)lParamSort);
    CItemSelectDialog * pThis = static_cast<CItemSelectDialog*>(pWnd);

    int sortResult = 0;
    size_t index1 = lParam1; // item data index
    size_t index2 = lParam2; // item data index

    // need to find the actual current entry in the list control to compare the
    // text content
    index1 = FindItemIndexByItemData(&pThis->m_availableItemsCtrl, index1);
    index2 = FindItemIndexByItemData(&pThis->m_availableItemsCtrl, index2);

    // get the current sort settings
    int column;
    bool bAscending;
    pThis->m_sortHeader.GetSortArrow(&column, &bAscending);

    // get the control text entries for the column being sorted
    CString index1Text = pThis->m_availableItemsCtrl.GetItemText(index1, column);
    CString index2Text = pThis->m_availableItemsCtrl.GetItemText(index2, column);

    // some columns are converted to numeric to do the sort
    // while others are compared as ASCII
    switch (column)
    {
    case ILC_name:
        // ASCII sorts
        sortResult = (index1Text < index2Text) ? -1 : 1;
        break;
    case ILC_level:
        {
            // numeric sorts
            double val1 = atof(index1Text);
            double val2 = atof(index2Text);
            if (val1 == val2)
            {
                // if numeric match, sort by item name instead
                index1Text = pThis->m_availableItemsCtrl.GetItemText(index1, ILC_name);
                index2Text = pThis->m_availableItemsCtrl.GetItemText(index2, ILC_name);
                sortResult = (index1Text < index2Text) ? -1 : 1;
            }
            else
            {
                sortResult = (val1 < val2) ? -1 : 1;
            }
        }
        break;
    }
    if (!bAscending)
    {
        // switch sort direction result
        sortResult = -sortResult;
    }
    return sortResult;
}

void CItemSelectDialog::OnHoverListItems(NMHDR* pNMHDR, LRESULT* pResult)
{
    // the user it hovering over a list control item. Identify it and display
    // the item tooltip for this item
    CPoint mousePosition;
    GetCursorPos(&mousePosition);
    m_availableItemsCtrl.ScreenToClient(&mousePosition);

    LVHITTESTINFO hitInfo;
    hitInfo.pt = mousePosition;
    if (m_availableItemsCtrl.SubItemHitTest(&hitInfo) >= 0)
    {
        if (m_hoverItem != hitInfo.iItem)
        {
            // the item under the hover has changed
            m_hoverItem = hitInfo.iItem;
            // mouse is over a valid item, get the items rectangle and
            // show the item tooltip
            CRect rect;
            m_availableItemsCtrl.GetItemRect(hitInfo.iItem, &rect, LVIR_BOUNDS);
            HideTip();
            // get the item index
            size_t itemIndex = m_availableItemsCtrl.GetItemData(hitInfo.iItem);
            m_availableItemsCtrl.ClientToScreen(&rect);
            CPoint tipTopLeft(rect.left, rect.bottom);
            CPoint tipAlternate(rect.left, rect.top);
            std::list<Item>::const_iterator it = m_availableItems.begin();
            std::advance(it, itemIndex);
            SetTooltipText((*it), tipTopLeft, tipAlternate);
            m_showingTip = true;
            // make sure we don't stack multiple monitoring of the same rectangle
            if (m_hoverHandle == 0)
            {
                m_hoverHandle = GetMouseHook()->AddRectangleToMonitor(
                        this->GetSafeHwnd(),
                        rect,           // screen coordinates,
                        WM_MOUSEENTER,
                        WM_MOUSELEAVE,
                        true);
            }
        }
    }
    *pResult = 1;   // stop the hover selecting the item
}

LRESULT CItemSelectDialog::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    if (wParam == m_hoverHandle)
    {
        // no longer monitoring this location
        GetMouseHook()->DeleteRectangleToMonitor(m_hoverHandle);
        m_hoverHandle = 0;
        m_hoverItem = -1;
    }
    return 0;
}

void CItemSelectDialog::ShowTip(const Item & item, CRect itemRect)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    ClientToScreen(&itemRect);
    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
    CPoint tipAlternate(itemRect.left, itemRect.top - 2);
    SetTooltipText(item, tipTopLeft, tipAlternate);
    m_showingTip = true;
}

void CItemSelectDialog::HideTip()
{
    // tip not shown if not over a gear slot
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
    }
}

void CItemSelectDialog::SetTooltipText(
        const Item & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetItem(&item);
    m_tooltip.Show();
    m_showingTip = true;
}

void CItemSelectDialog::SetTooltipText(
        const Augment & augment,
        CPoint tipTopLeft,
        CPoint tipAlternate,
        bool rightAlign)
{
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, rightAlign);
    m_tooltip.SetAugment(&augment);
    m_tooltip.Show();
    m_showingTip = true;
}

void CItemSelectDialog::OnSelEndOkFilter()
{
    int sel = m_comboFilter.GetCurSel();
    if (sel != CB_ERR)
    {
        sel = m_comboFilter.GetItemData(sel);
        // selection is based on the slot type
        switch (m_slot)
        {
        case Inventory_Armor:
            m_armorType = (ArmorType)sel;
            break;
        case Inventory_Weapon1:
        case Inventory_Weapon2:
            m_weaponType = (WeaponType)sel;
            break;
        }
        PopulateAvailableItemList();
    }
}

void CItemSelectDialog::SetupFilterCombobox()
{
    m_comboFilter.ResetContent();
    int selItem = 0;        // assume 1st item
    if (m_slot == Inventory_Armor)
    {
        RaceType race = m_pCharacter->Race();
        bool construct = (race == Race_Warforged || race == Race_BladeForged);
        // limit to docent only for constructs
        if (construct)
        {
            int i = m_comboFilter.AddString("Docent");
            m_comboFilter.SetItemData(i, Armor_Docent);
            selItem = 0;
        }
        else
        {
            m_comboFilter.AddString("All");
            m_comboFilter.SetItemData(0, 0);
            int i = m_comboFilter.AddString("Cloth/Robe");
            m_comboFilter.SetItemData(i, Armor_Cloth);
            i = m_comboFilter.AddString("Light Armor");
            m_comboFilter.SetItemData(i, Armor_Light);
            i = m_comboFilter.AddString("Medium Armor");
            m_comboFilter.SetItemData(i, Armor_Medium);
            i = m_comboFilter.AddString("Heavy Armor");
            m_comboFilter.SetItemData(i, Armor_Heavy);
            selItem = m_armorType;  // 1..4
        }
    }
    else if (m_slot == Inventory_Weapon1)
    {
        m_comboFilter.AddString("All");
        m_comboFilter.SetItemData(0, 0);
        // any weapon
        for (size_t i = Weapon_Unknown; i < Weapon_Count; ++i)
        {
            if (IsMeleeWeapon((WeaponType)i)
                    || IsRangedWeapon((WeaponType)i))
            {
                // we can add this one
                int index = m_comboFilter.AddString(EnumEntryText((WeaponType)i, weaponTypeMap));
                m_comboFilter.SetItemData(index, i);
                if (i == m_weaponType)
                {
                    selItem = index;
                }
            }
        }
    }
    else if (m_slot == Inventory_Weapon2)
    {
        m_comboFilter.AddString("All");
        m_comboFilter.SetItemData(0, 0);
        // any single handed weapon, orb, shield or rune-arm
        // any one handed weapon
        for (size_t i = Weapon_Unknown; i < Weapon_Count; ++i)
        {
            if ((IsOneHandedWeapon((WeaponType)i)
                    || IsShield((WeaponType)i)
                    || i == Weapon_Orb
                    || (i == Weapon_RuneArm && m_pCharacter->IsFeatTrained("Artificer Rune Arm Use")))
                    && i != Weapon_BastardSword)
            {
                // we can add this one
                int index = m_comboFilter.AddString(EnumEntryText((WeaponType)i, weaponTypeMap));
                m_comboFilter.SetItemData(index, i);
                if (i == m_weaponType)
                {
                    selItem = index;
                }
            }
        }
    }
    else
    {
        // selection combo not enabled
        m_comboFilter.EnableWindow(FALSE);
    }
    m_comboFilter.SetCurSel(selItem);
}

void CItemSelectDialog::AddSpecialSlots()
{
    // add Mythic and reaper slots to the item as long as its a named item.
    // We can tell its named as it will not have a minimum level of 1 as all
    // non-named items (random loot and Cannith crafted) are set to min level 1
    if (m_item.MinLevel() > 1)
    {
        std::vector<ItemAugment> currentAugments = m_item.Augments();
        AddAugment(&currentAugments, "Mythic", true);
        AddAugment(&currentAugments, "Reaper", true);
        // now set the slots on the item
        m_item.Set_Augments(currentAugments);
    }
    else
    {
        // add upgrade slots for Cannith crafted and random loot
        SlotUpgrade slot;
        slot.Set_Type("Upgrade Slot");
        std::vector<std::string> slots;
        slots.push_back("Colorless");
        slots.push_back("Blue");
        slots.push_back("Green");
        slots.push_back("Yellow");
        slots.push_back("Orange");
        slots.push_back("Red");
        slots.push_back("Purple");
        slot.Set_UpgradeType(slots);

        std::vector<SlotUpgrade> upgrades;
        upgrades.push_back(slot);   // 2 standard upgrade slots
        upgrades.push_back(slot);
        m_item.Set_SlotUpgrades(upgrades);
    }
}

LRESULT CItemSelectDialog::OnHoverComboBox(WPARAM wParam, LPARAM lParam)
{
    if (!m_bIgnoreNextMessage)
    {
        // wParam = selected index
        // lParam = control ID
        // as these are all augment combo boxes, we can treat them all the same
        if (m_showingTip)
        {
            m_tooltip.Hide();
        }
        if (wParam >= 0)
        {
            // we have a selection, get the filigree name
            CString augmentName;
            CWnd * pWnd = GetDlgItem(lParam);
            CComboBox * pCombo =  dynamic_cast<CComboBox*>(pWnd);
            pCombo->GetLBText(wParam, augmentName);
            if (!augmentName.IsEmpty())
            {
                CRect rctWindow;
                pCombo->GetWindowRect(&rctWindow);
                rctWindow.right = rctWindow.left + pCombo->GetDroppedWidth();
                // tip is shown to the left or the right of the combo box
                CPoint tipTopLeft(rctWindow.left, rctWindow.top);
                CPoint tipAlternate(rctWindow.right, rctWindow.top);
                Augment augment = FindAugmentByName((LPCTSTR)augmentName);
                SetTooltipText(augment, tipTopLeft, tipAlternate, true);
                m_showingTip = true;
            }
        }
    }
    m_bIgnoreNextMessage = false;
    return 0;
}

LRESULT CItemSelectDialog::OnMouseEnter(WPARAM wParam, LPARAM lParam)
{
    // is it a jewel or a filigree?
    for (size_t i = 0 ; i < MAX_Augments; ++i)
    {
        if (wParam == m_augmentHookHandles[i])
        {
            int sel = m_comboAugmentDropList[i].GetCurSel();
            if (sel != CB_ERR)
            {
                CString augmentName;
                m_comboAugmentDropList[i].GetLBText(
                        sel,
                        augmentName);
                if (!augmentName.IsEmpty())
                {
                    CRect itemRect;
                    m_comboAugmentDropList[i].GetWindowRect(&itemRect);
                    Augment augment = FindAugmentByName((LPCTSTR)augmentName);
                    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
                    CPoint tipAlternate(itemRect.left, itemRect.top - 2);
                    SetTooltipText(augment, tipTopLeft, tipAlternate, false);
                    break;
                }
            }
        }
    }
    return 0;
}

void CItemSelectDialog::OnWindowPosChanging(WINDOWPOS * pos)
{
    // ensure tooltip locations are correct on window move
    CDialog::OnWindowPosChanging(pos);
    PostMessage(WM_SIZE, SIZE_RESTORED, MAKELONG(pos->cx, pos->cy));
}

void CItemSelectDialog::AddAugment(
        std::vector<ItemAugment> * augments,
        const std::string & name,
        bool atEnd)
{
    // only add if it is not already present
    bool found = false;
    for (size_t i = 0; i < augments->size(); ++i)
    {
        if ((*augments)[i].Type() == name)
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        // this is a new augment slot that needs to be added
        ItemAugment newAugment;
        newAugment.Set_Type(name);
        // all new augments are added before any "Mythic" slot if present
        bool foundMythic = false;
        size_t i;
        for (i = 0; i < augments->size(); ++i)
        {
            if ((*augments)[i].Type() == "Mythic")
            {
                foundMythic = true;
                break;
            }
        }
        if (!atEnd && foundMythic)
        {
            std::vector<ItemAugment>::iterator it = augments->begin();
            std::advance(it, i);
            augments->insert(it, newAugment);
        }
        else
        {
            // just add to the end if no current Mythic slot
            augments->push_back(newAugment);
        }
    }
}

void CItemSelectDialog::RemoveAugment(
        std::vector<ItemAugment> * augments,
        const std::string & name)
{
    for (size_t i = 0; i < augments->size(); ++i)
    {
        if ((*augments)[i].Type() == name)
        {
            // this one needs removing
            std::vector<ItemAugment>::iterator it = augments->begin();
            std::advance(it, i);
            augments->erase(it);
            // we're done
            break;
        }
    }
}

void CItemSelectDialog::OnSearchTextKillFocus()
{
    CString text;
    m_editSearchText.GetWindowText(text);
    if (text != m_previousSearchText)
    {
        // just update the list of items as the search text changes
        m_bInitialising = true;
        PopulateAvailableItemList();
        m_bInitialising = false;
        m_previousSearchText = text;
    }
}

BOOL CItemSelectDialog::PreTranslateMessage(MSG* pMsg)
{
    BOOL handled = FALSE;
    // if the user presses return/enter on text in the filter control,
    // we just set the focus to the filter button
    if (pMsg->message == WM_KEYDOWN
            && pMsg->wParam == VK_RETURN
            && GetFocus() == &m_editSearchText)
    {
        m_buttonFilter.SetFocus();
        handled = TRUE;
    }
    if (!handled)
    {
        handled = CDialog::PreTranslateMessage(pMsg);
    }
    return handled;
}

