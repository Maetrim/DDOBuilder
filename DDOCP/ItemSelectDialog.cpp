// ItemSelectDialog.cpp
//

#include "stdafx.h"
#include "ItemSelectDialog.h"
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
        RaceType race) :
    CDialog(CItemSelectDialog::IDD, pParent),
    m_slot(slot),
    m_item(item),
    m_race(race),
    m_bInitialising(false),
    m_showingTip(false),
    m_tipCreated(false),
    m_hoverItem(-1),
    m_hoverHandle(0),
    m_armorType(Armor_Unknown)
{
    if (m_item.HasArmor())
    {
        m_armorType = m_item.Armor();
    }
    else
    {
        m_armorType = Armor_Light;
    }
    // race overrides current armor type
    if (m_race == Race_Warforged
            || m_race == Race_BladeForged)
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
    DDX_Control(pDX, IDC_RADIO_CLOTH, m_buttonCloth);
    DDX_Control(pDX, IDC_RADIO_LIGHT, m_buttonLight);
    DDX_Control(pDX, IDC_RADIO_MEDIUM, m_buttonMedium);
    DDX_Control(pDX, IDC_RADIO_HEAVY, m_buttonHeavy);
    DDX_Control(pDX, IDC_RADIO_DOCENT, m_buttonDocent);
    DDX_Control(pDX, IDC_ITEM_TYPE, m_staticType);
    DDX_Control(pDX, IDC_ITEM_LIST, m_availableItemsCtrl);
    if (!pDX->m_bSaveAndValidate)
    {
        VERIFY(m_sortHeader.SubclassWindow(m_availableItemsCtrl.GetHeaderCtrl()->GetSafeHwnd()));
    }
    for (size_t i = 0; i < MAX_Augments; ++i)
    {
        DDX_Control(pDX, IDC_STATIC_AUGMENT_TYPE1 + i, m_augmentType[i]);
        DDX_Control(pDX, IDC_COMBO_AUGMENT1 + i, m_comboAugmentDropList[i]);
    }
    for (size_t i = 0; i < MAX_Upgrades; ++i)
    {
        DDX_Control(pDX, IDC_STATIC_UPGRADE_TYPE1 + i, m_upgradeType[i]);
        DDX_Control(pDX, IDC_COMBO_UPGRADE1 + i, m_comboUpgradeDropList[i]);
    }
}

BEGIN_MESSAGE_MAP(CItemSelectDialog, CDialog)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_LIST, OnItemSelected)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_AUGMENT1, IDC_COMBO_AUGMENT1 + MAX_Augments, OnAugmentSelect)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_UPGRADE1, IDC_COMBO_UPGRADE1 + MAX_Upgrades, OnUpgradeSelect)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(HDN_ENDTRACK, IDC_ITEM_LIST, OnEndtrackListItems)
    ON_NOTIFY(HDN_DIVIDERDBLCLICK, IDC_ITEM_LIST, OnEndtrackListItems)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_ITEM_LIST, OnColumnclickListItems)
    ON_NOTIFY(NM_HOVER, IDC_ITEM_LIST, OnHoverListItems)
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_BN_CLICKED(IDC_RADIO_CLOTH, OnRadioCloth)
    ON_BN_CLICKED(IDC_RADIO_LIGHT, OnRadioLight)
    ON_BN_CLICKED(IDC_RADIO_MEDIUM, OnRadioMedium)
    ON_BN_CLICKED(IDC_RADIO_HEAVY, OnRadioHeavy)
    ON_BN_CLICKED(IDC_RADIO_DOCENT, OnRadioDocent)
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
    SetArmorButtonStates();
    // add list control columns
    m_availableItemsCtrl.InsertColumn(0, "Item Name", LVCFMT_LEFT, 150);
    m_availableItemsCtrl.InsertColumn(1, "Level", LVCFMT_LEFT, 50);
    PopulateAvailableItemList();
    m_staticType.SetWindowText(text);
    m_availableItemsCtrl.SetExtendedStyle(
            m_availableItemsCtrl.GetExtendedStyle()
            | LVS_EX_FULLROWSELECT
            | LVS_EX_TRACKSELECT
            //| LVS_EX_LABELTIP); // stop hover tooltips from working
            );
    LoadColumnWidthsByName(&m_availableItemsCtrl, "ItemSelectDialog_%s");
    m_sortHeader.SetSortArrow(0, TRUE);

    EnableControls();

    m_sizer.Hook(GetSafeHwnd(), "ItemSelectDialog");

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
    // filter the list of items loaded to those that match the slot type
    const std::list<Item> & allItems = Items();
    m_availableItems.clear();
    std::list<Item>::const_iterator it = allItems.begin();
    while (it != allItems.end())
    {
        if ((*it).CanEquipToSlot(m_slot, m_armorType))
        {
            // user can select his item, add it to the available list
            m_availableItems.push_back((*it));
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

    int sel = -1;           // item to be selected
    // now populate the control
    size_t itemIndex = 0;
    it = m_availableItems.begin();
    while (it != m_availableItems.end())
    {
        int item = m_availableItemsCtrl.InsertItem(
                m_availableItemsCtrl.GetItemCount(),
                (*it).Name().c_str(),
                itemIndex);
        if ((*it).Name() == m_item.Name())
        {
            sel = itemIndex;
        }
        CString level;
        level.Format("%d", (*it).MinLevel());
        m_availableItemsCtrl.SetItemText(item, 1, level);
        m_availableItemsCtrl.SetItemData(item, itemIndex);

        ++itemIndex;
        ++it;
    }
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
    }
    m_availableItemsCtrl.UnlockWindowUpdate();
}

void CItemSelectDialog::EnableControls()
{
    // show / hide the available augments based on the selected item
    std::vector<ItemAugment> augments = m_item.Augments();
    ASSERT(augments.size() < MAX_Augments);

    for (size_t i = 0; i < MAX_Augments; ++i)
    {
        if (i < augments.size())
        {
            // we have an augment, populate it with the available for this type
            m_augmentType[i].SetWindowText(EnumEntryText(augments[i].Type(), augmentTypeMap));
            PopulateAugmentList(
                    &m_comboAugmentDropList[i],
                    augments[i].Type(),
                    augments[i].HasSelectedAugment() ? augments[i].SelectedAugment() : "");
            m_augmentType[i].ShowWindow(SW_SHOW);
            m_comboAugmentDropList[i].ShowWindow(SW_SHOW);
        }
        else
        {
            // no augment, hide control
            m_augmentType[i].ShowWindow(SW_HIDE);
            m_comboAugmentDropList[i].ShowWindow(SW_HIDE);
        }
    }
    // now show any upgrade slots
    size_t upgradeIndex = 0;
    if (m_item.PrimaryUpgrade().size() > 0)
    {
        // we have a primary upgrade list
        PopulatePrimaryUpgradeList(upgradeIndex);
        ++upgradeIndex;
    }
    if (m_item.SecondaryUpgrade().size() > 0)
    {
        // we have a primary upgrade list
        PopulateSecondaryUpgradeList(upgradeIndex);
        ++upgradeIndex;
    }
    if (m_item.LegendarySlavelordUpgrade().size() > 0)
    {
        // we have a primary upgrade list
        PopulateLegendarySlavelordUpgradeList(upgradeIndex);
        ++upgradeIndex;
    }
    // add other upgrade types here
    // now hide the unused
    for (;upgradeIndex < MAX_Upgrades; ++upgradeIndex)
    {
        m_upgradeType[upgradeIndex].ShowWindow(SW_HIDE);
        m_comboUpgradeDropList[upgradeIndex].ShowWindow(SW_HIDE);
    }
}

void CItemSelectDialog::PopulateAugmentList(
        CComboBoxEx * combo,
        AugmentType type,
        const std::string & selectedAugment)
{
    int sel = CB_ERR;
    combo->LockWindowUpdate();
    combo->ResetContent();
    // get all the augments compatible with this slot type
    const std::list<Augment> & augments = Augments();
    std::list<Augment>::const_iterator it = augments.begin();
    size_t index = 0;
    while (it != augments.end())
    {
        if ((*it).IsCompatibleWithSlot(type))
        {
            char buffer[_MAX_PATH];
            strcpy_s(buffer, (*it).Name().c_str());
            COMBOBOXEXITEM item;
            item.mask = CBEIF_TEXT;
            item.iItem = index;
            item.iImage = 0;
            item.iSelectedImage = 0;
            item.pszText = buffer;
            item.lParam = index;
            size_t pos = combo->InsertItem(&item);
            if ((*it).Name() == selectedAugment)
            {
                sel = pos;
            }
            ++index;
        }
        ++it;
    }
    if (sel != CB_ERR)
    {
        combo->SetCurSel(sel);
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
                std::list<Item>::const_iterator it = m_availableItems.begin();
                std::advance(it, sel);
                m_item = (*it);
                // update the other controls
                EnableControls();
                // item selected, can now click ok!
                GetDlgItem(IDOK)->EnableWindow(TRUE);
            }
        }
    }
    *pResult = 0;
}

void CItemSelectDialog::OnAugmentSelect(UINT nID)
{
    // the user has selected a feat in one of the three feat combo boxes
    int augmentIndex = nID - IDC_COMBO_AUGMENT1;
    ASSERT(augmentIndex >= 0 && augmentIndex < (int)m_item.Augments().size());
    int sel = m_comboAugmentDropList[augmentIndex].GetCurSel();
    if (sel != CB_ERR)
    {
        // user has selected an augment. Get its name
        char buffer[_MAX_PATH];
        memset(buffer, 0, _MAX_PATH);
        COMBOBOXEXITEM item;
        item.mask = CBEIF_TEXT;
        item.iItem = sel;
        item.pszText = buffer;
        item.cchTextMax = _MAX_PATH;
        m_comboAugmentDropList[augmentIndex].GetItem(&item);
        std::vector<ItemAugment> augments = m_item.Augments();
        augments[augmentIndex].Set_SelectedAugment(item.pszText);
        m_item.Set_Augments(augments);
    }
}

void CItemSelectDialog::PopulatePrimaryUpgradeList(size_t controlIndex)
{
    m_upgradeTypeModelled[controlIndex] = Upgrade_Primary;
    // set the text of the display item
    m_upgradeType[controlIndex].SetWindowText("Primary Upgrade");
    // add the entries to the drop list control
    PopulateDropList(controlIndex, m_item.PrimaryUpgrade());
    // finally ensure the control is displayed
    m_upgradeType[controlIndex].ShowWindow(SW_SHOW);
    m_comboUpgradeDropList[controlIndex].ShowWindow(SW_SHOW);
}

void CItemSelectDialog::PopulateSecondaryUpgradeList(size_t controlIndex)
{
    m_upgradeTypeModelled[controlIndex] = Upgrade_Secondary;
    // set the text of the display item
    m_upgradeType[controlIndex].SetWindowText("Secondary Upgrade");
    // add the entries to the drop list control
    PopulateDropList(controlIndex, m_item.SecondaryUpgrade());
    // finally ensure the control is displayed
    m_upgradeType[controlIndex].ShowWindow(SW_SHOW);
    m_comboUpgradeDropList[controlIndex].ShowWindow(SW_SHOW);
}

void CItemSelectDialog::PopulateLegendarySlavelordUpgradeList(size_t controlIndex)
{
    m_upgradeTypeModelled[controlIndex] = Upgrade_LegendarySlavelords;
    // set the text of the display item
    m_upgradeType[controlIndex].SetWindowText("Legendary Slavelords Upgrade");
    // add the entries to the drop list control
    PopulateDropList(controlIndex, m_item.LegendarySlavelordUpgrade());
    // finally ensure the control is displayed
    m_upgradeType[controlIndex].ShowWindow(SW_SHOW);
    m_comboUpgradeDropList[controlIndex].ShowWindow(SW_SHOW);
}

void CItemSelectDialog::PopulateDropList(size_t controlIndex, const std::list<AugmentType> & types)
{
    m_comboUpgradeDropList[controlIndex].ResetContent();
    std::list<AugmentType>::const_iterator it = types.begin();
    size_t index = 0;
    while (it != types.end())
    {
        CString entry = EnumEntryText((*it), augmentTypeMap);
        char buffer[_MAX_PATH];
        strcpy_s(buffer, entry);
        COMBOBOXEXITEM item;
        item.mask = CBEIF_TEXT;
        item.iItem = index;
        item.iImage = 0;
        item.iSelectedImage = 0;
        item.pszText = buffer;
        item.lParam = index;
        size_t pos = m_comboUpgradeDropList[controlIndex].InsertItem(&item);
        // move on to the next item
        ++index;
        ++it;
    }
}

void CItemSelectDialog::OnUpgradeSelect(UINT nID)
{
    // an upgrade selection has been made. Determine which augment slot type
    // needs to be added and remove the upgrade list from the item. This action
    // cannot be undone. The user will have to reselect he base item to undo this
    size_t controlIndex = nID - IDC_COMBO_UPGRADE1;
    int selection = m_comboUpgradeDropList[controlIndex].GetCurSel();
    // now find what was selected
    std::list<AugmentType> augments;
    switch (m_upgradeTypeModelled[controlIndex])
    {
    case Upgrade_Primary:
        augments = m_item.PrimaryUpgrade();
        break;
    case Upgrade_Secondary:
        augments = m_item.SecondaryUpgrade();
        break;
    case Upgrade_LegendarySlavelords:
        augments = m_item.LegendarySlavelordUpgrade();
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    // determine the augment type to add
    ASSERT(selection >= 0 && selection < (int)augments.size());
    std::list<AugmentType>::iterator it = augments.begin();
    std::advance(it, selection);

    std::vector<ItemAugment> currentAugments = m_item.Augments();
    ItemAugment newAugment;
    newAugment.Set_Type((*it));
    currentAugments.push_back(newAugment);
    m_item.Set_Augments(currentAugments);

    // now ensure the upgrade cannot be selected again
    std::list<AugmentType> empty;
    switch (m_upgradeTypeModelled[controlIndex])
    {
    case Upgrade_Primary:
        m_item.Set_PrimaryUpgrade(empty);
        break;
    case Upgrade_Secondary:
        m_item.Set_SecondaryUpgrade(empty);
        break;
    case Upgrade_LegendarySlavelords:
        m_item.Set_LegendarySlavelordUpgrade(empty);
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    // now get all controls re-repopulate
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
}

void CItemSelectDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // minimum size
    lpMMI->ptMinTrackSize.x = 640;
    lpMMI->ptMinTrackSize.y = 465;
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
    // allow the user to sort the skills list based on the selected column
    // skill selected is identified by the item data which maps to the SkillType enum
    // when the control was populated.
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
    // the feat tooltip for this item
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
            // mouse is over a valid automatic feat, get the items rectangle and
            // show the feat tooltip
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
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate);
    m_tooltip.SetItem(&item);
    m_tooltip.Show();
}

void CItemSelectDialog::OnRadioCloth()
{
    m_armorType = Armor_Cloth;
    SetArmorButtonStates();
    PopulateAvailableItemList();
}

void CItemSelectDialog::OnRadioLight()
{
    m_armorType = Armor_Light;
    SetArmorButtonStates();
    PopulateAvailableItemList();
}

void CItemSelectDialog::OnRadioMedium()
{
    m_armorType = Armor_Medium;
    SetArmorButtonStates();
    PopulateAvailableItemList();
}

void CItemSelectDialog::OnRadioHeavy()
{
    m_armorType = Armor_Heavy;
    SetArmorButtonStates();
    PopulateAvailableItemList();
}

void CItemSelectDialog::OnRadioDocent()
{
    m_armorType = Armor_Docent;
    SetArmorButtonStates();
    PopulateAvailableItemList();
}

void CItemSelectDialog::SetArmorButtonStates()
{
    if (m_slot == Inventory_Armor)
    {
        bool construct = (m_race == Race_Warforged || m_race == Race_BladeForged);
        m_buttonCloth.EnableWindow(!construct);
        m_buttonLight.EnableWindow(!construct);
        m_buttonMedium.EnableWindow(!construct);
        m_buttonHeavy.EnableWindow(!construct);
        m_buttonDocent.EnableWindow(construct);
        // if we have an item default to its armor type
        m_buttonCloth.SetCheck((m_armorType == Armor_Cloth) ? BST_CHECKED : BST_UNCHECKED);
        m_buttonLight.SetCheck((m_armorType == Armor_Light) ? BST_CHECKED : BST_UNCHECKED);
        m_buttonMedium.SetCheck((m_armorType == Armor_Medium) ? BST_CHECKED : BST_UNCHECKED);
        m_buttonHeavy.SetCheck((m_armorType == Armor_Heavy) ? BST_CHECKED : BST_UNCHECKED);
        m_buttonDocent.SetCheck((m_armorType == Armor_Docent) ? BST_CHECKED : BST_UNCHECKED);
    }
    else
    {
        // armor selection buttons not enabled
        m_buttonCloth.EnableWindow(FALSE);
        m_buttonLight.EnableWindow(FALSE);
        m_buttonMedium.EnableWindow(FALSE);
        m_buttonHeavy.EnableWindow(FALSE);
        m_buttonDocent.EnableWindow(FALSE);
    }
}


