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
    DDX_Control(pDX, IDC_CHECK_SENTIENT_SLOTTED, m_buttonSentientJewel);
    DDX_Control(pDX, IDC_STATIC_SENTIENT_LABEL, m_sentientLabel);
    DDX_Control(pDX, IDC_COMBO_PERSONALITY, m_comboSentientPersonality);
    for (size_t i = 0; i < MAX_Filigree; ++i)
    {
        DDX_Control(pDX, IDC_COMBO_FILIGREE1 + i, m_comboFiligreeDropList[i]);
        DDX_Control(pDX, IDC_CHECK_FILGREE_RARE1 + i, m_buttonFiligreeRare[i]);
    }
}

BEGIN_MESSAGE_MAP(CItemSelectDialog, CDialog)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_LIST, OnItemSelected)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_AUGMENT1, IDC_COMBO_AUGMENT1 + MAX_Augments, OnAugmentSelect)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_UPGRADE1, IDC_COMBO_UPGRADE1 + MAX_Upgrades, OnUpgradeSelect)
    ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_AUGMENT1, IDC_EDIT_AUGMENT1 + MAX_Augments, OnKillFocusAugmentEdit)
    ON_CBN_SELENDOK(IDC_COMBO_PERSONALITY, OnSelEndOkPersonality)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_UPGRADE1, IDC_COMBO_FILIGREE1 + MAX_Filigree, OnUpgradeFiligree)
    ON_CONTROL_RANGE(CBN_SELENDCANCEL, IDC_COMBO_UPGRADE1, IDC_COMBO_FILIGREE1 + MAX_Filigree, OnUpgradeFiligreeCancel)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_FILGREE_RARE1, IDC_CHECK_FILGREE_RARE1 + MAX_Filigree, OnUpgradeFiligreeRare)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_WINDOWPOSCHANGING()
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
    ON_BN_CLICKED(IDC_CHECK_SENTIENT_SLOTTED, OnButtonSentientJewel)
    ON_MESSAGE(WM_MOUSEHOVER, OnHoverComboBox)
    ON_MESSAGE(WM_MOUSEENTER, OnMouseEnter)
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
    m_sortHeader.SetSortArrow(1, FALSE);     // sort by level by default
    m_availableItemsCtrl.SortItems(
            CItemSelectDialog::SortCompareFunction,
            (long)GetSafeHwnd());

    EnableControls();

    //m_sizer.Hook(GetSafeHwnd(), "ItemSelectDialog");

    if (m_slot != Inventory_Weapon1)
    {
        // resize the dialog so that the sentient weapon filigree controls
        // are not visible
        CRect rct;
        m_sentientLabel.GetWindowRect(rct);
        ScreenToClient(rct);
        int newWidth = rct.left;
        GetWindowRect(rct);
        rct.right = rct.left + newWidth;
        MoveWindow(rct);
    }
    SetSentientWeaponControls();
    // add monitor locations to show sentient weapon and filigree tooltips
    if (m_slot == Inventory_Weapon1)
    {
        CRect rect;
        m_comboSentientPersonality.GetWindowRect(&rect);
        m_comboHookHandles[0] = GetMouseHook()->AddRectangleToMonitor(
                this->GetSafeHwnd(),
                rect,           // screen coordinates,
                WM_MOUSEENTER,
                WM_MOUSELEAVE,
                false);
        for (size_t i = 0 ; i < MAX_Filigree; ++i)
        {
            m_comboFiligreeDropList[i].GetWindowRect(&rect);
            m_comboHookHandles[i+1] = GetMouseHook()->AddRectangleToMonitor(
                    this->GetSafeHwnd(),
                    rect,           // screen coordinates,
                    WM_MOUSEENTER,
                    WM_MOUSELEAVE,
                    false);
        }
    }

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
            CString text;
            text.Format("Item Selection and Configuration - %s",
                    m_item.Name().c_str());
            SetWindowText(text);
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
        // cannot select sentient jewel until an item selected
        m_buttonSentientJewel.EnableWindow(FALSE);
        // show the name of the selected item in the dialog title
        SetWindowText("Item Selection and Configuration - No item selected");
    }
    m_availableItemsCtrl.UnlockWindowUpdate();
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
    std::list<Augment> augments = CompatibleAugments(augment.Type());
    std::list<Augment>::const_iterator it = augments.begin();
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
                std::list<Item>::const_iterator it = m_availableItems.begin();
                std::advance(it, sel);
                m_item = (*it);
                AddSpecialSlots();  // adds reaper or mythic slots to the item
                // update the other controls
                EnableControls();
                // item selected, can now click ok!
                GetDlgItem(IDOK)->EnableWindow(TRUE);
                // ensure sentient weapon controls are correct
                m_buttonSentientJewel.EnableWindow(m_slot == Inventory_Weapon1
                        && m_item.HasCanAcceptSentientJewel());
                SetSentientWeaponControls();
                CString text;
                text.Format("Item Selection and Configuration - %s",
                        m_item.Name().c_str());
                SetWindowText(text);
            }
        }
    }
    *pResult = 0;
}

void CItemSelectDialog::OnAugmentSelect(UINT nID)
{
    // the user has selected a feat in one of the augment combo boxes
    int augmentIndex = nID - IDC_COMBO_AUGMENT1;
    ASSERT(augmentIndex >= 0 && augmentIndex < (int)m_item.Augments().size());
    int sel = m_comboAugmentDropList[augmentIndex].GetCurSel();
    if (sel != CB_ERR)
    {
        // user has selected an augment. Get its name
        CString text;
        m_comboAugmentDropList[augmentIndex].GetLBText(sel, text);
        std::vector<ItemAugment> augments = m_item.Augments();
        augments[augmentIndex].Set_SelectedAugment((LPCTSTR)text);
        m_item.Set_Augments(augments);
        // update the controls after a selection as edit controls may need
        // to be displayed for some augment selection controls.
        EnableControls();
    }
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
    // cannot be undone. The user will have to reselect he base item to undo this
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
        ItemAugment newAugment;
        newAugment.Set_Type((*it));
        currentAugments.push_back(newAugment);
        m_item.Set_Augments(currentAugments);

        // now erase the upgrade from the upgrade list as its been actioned
        upgrades.erase(sit);
        m_item.Set_SlotUpgrades(upgrades);

        // now get all controls re-repopulate
        EnableControls();
    }
}

void CItemSelectDialog::OnButtonSentientJewel()
{
    bool enabled = (m_buttonSentientJewel.GetCheck() != 0);
    if (enabled)
    {
        // add sentient jewel to the item
        SentientJewel jewel;
        m_item.Set_SentientIntelligence(jewel);
    }
    else
    {
        // just clear the sentient jewel
        m_item.Clear_SentientIntelligence();
    }
    SetSentientWeaponControls();
}

void CItemSelectDialog::OnSelEndOkPersonality()
{
    int selection = m_comboSentientPersonality.GetCurSel();
    if (selection != CB_ERR)
    {
        selection = m_comboSentientPersonality.GetItemData(selection);
        // user has selected a personality. Get its name
        CString text;
        m_comboSentientPersonality.GetLBText(selection, text);
        SentientJewel jewel = m_item.SentientIntelligence();
        jewel.Set_Personality((LPCTSTR)text);
        m_item.Set_SentientIntelligence(jewel);
    }
}

void CItemSelectDialog::OnUpgradeFiligree(UINT nID)
{
    HideTip();
    size_t filigreeIndex = nID - IDC_COMBO_FILIGREE1;
    int selection = m_comboFiligreeDropList[filigreeIndex].GetCurSel();
    if (selection != CB_ERR)
    {
        // user has selected a Filigree. Get its name
        CString text;
        m_comboFiligreeDropList[filigreeIndex].GetLBText(selection, text);
        SentientJewel jewel = m_item.SentientIntelligence();
        jewel.SetFiligree(filigreeIndex, (LPCTSTR)text);
        m_item.Set_SentientIntelligence(jewel);
        // update the controls after a selection as options in other
        // drop lists will need to be limited
        SetSentientWeaponControls();
    }
}
void CItemSelectDialog::OnUpgradeFiligreeCancel(UINT nID)
{
    HideTip();
}


void CItemSelectDialog::OnUpgradeFiligreeRare(UINT nID)
{
    // user is changing the rare state of a selected filigree
    size_t filigreeIndex = nID - IDC_CHECK_FILGREE_RARE1;
    bool checked = (m_buttonFiligreeRare[filigreeIndex].GetCheck() != 0);
    SentientJewel jewel = m_item.SentientIntelligence();
    jewel.SetFiligreeRare(filigreeIndex, checked);
    m_item.Set_SentientIntelligence(jewel);
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
    if (IsWindow(m_comboSentientPersonality.GetSafeHwnd())
            && m_slot == Inventory_Weapon1)
    {
        // update the mouse hook handles for tooltips
        CRect rect;
        m_comboSentientPersonality.GetWindowRect(&rect);
        // gives the wrong rectangle, ensure large enough
        rect.bottom = rect.top + 32 + GetSystemMetrics(SM_CYBORDER) * 4;
        GetMouseHook()->UpdateRectangle(
                m_comboHookHandles[0],
                rect);          // screen coordinates
        for (size_t i = 0; i < MAX_Filigree; ++i)
        {
            CRect rect;
            m_comboFiligreeDropList[i].GetWindowRect(&rect);
            GetMouseHook()->UpdateRectangle(
                    m_comboHookHandles[i+1],
                    rect);          // screen coordinates
        }
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

void CItemSelectDialog::AddSpecialSlots()
{
    // add Mythic and reaper slots to the item as long as its a named item.
    // We can tell its named as it will not have a minimum level of 1 as all
    // non-named items (random loot and Cannith crafted) are set to min level 1
    if (m_item.MinLevel() > 1)
    {
        std::vector<ItemAugment> currentAugments = m_item.Augments();
        // add mythic
        ItemAugment mythicAugment;
        mythicAugment.Set_Type("Mythic");
        currentAugments.push_back(mythicAugment);
        // add reaper
        ItemAugment reaperAugment;
        reaperAugment.Set_Type("Reaper");
        currentAugments.push_back(reaperAugment);
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

void CItemSelectDialog::SetSentientWeaponControls()
{
    if (m_slot == Inventory_Weapon1)
    {
        m_buttonSentientJewel.SetCheck(m_item.HasSentientIntelligence()
                ? BST_CHECKED
                : BST_UNCHECKED);
        // enable and populate the Filigree drop list controls
        m_comboSentientPersonality.EnableWindow(m_item.HasSentientIntelligence());
        PopulatePersonalityCombobox();
        for (size_t fi = 0; fi < MAX_Filigree; ++fi)
        {
            PopulateFiligreeCombobox(fi);
        }
    }
    else
    {
        m_buttonSentientJewel.EnableWindow(false);
    }
}

void CItemSelectDialog::PopulatePersonalityCombobox()
{
    if (m_item.HasSentientIntelligence())
    {
        m_comboSentientPersonality.EnableWindow(true);
        // populate with the list of available personalities
        std::list<Augment> augments = CompatibleAugments("Personality");
        // build the image list for this control
        BuildImageList(&m_ilJewel, augments);
        m_comboSentientPersonality.SetImageList(&m_ilJewel);
        // now add the available personalities
        m_comboSentientPersonality.ResetContent();
        std::string selectedPersonality;
        if (m_item.SentientIntelligence().HasPersonality())
        {
            selectedPersonality = m_item.SentientIntelligence().Personality();
        }
        size_t index = 0;
        std::list<Augment>::iterator it = augments.begin();
        while (it != augments.end())
        {
            size_t pos = m_comboSentientPersonality.AddString((*it).Name().c_str());
            m_comboSentientPersonality.SetItemData(pos, index);
            if ((*it).Name() == selectedPersonality)
            {
                 // this is the selected personality, select it
                m_comboSentientPersonality.SetCurSel(pos);
            }
            ++it;
            ++index;
        }
    }
    else
    {
        // disabled with no items or selections
        m_comboSentientPersonality.EnableWindow(false);
        m_comboSentientPersonality.ResetContent();
    }
}

void CItemSelectDialog::PopulateFiligreeCombobox(size_t filigreeIndex)
{
    if (m_item.HasSentientIntelligence())
    {
        m_comboFiligreeDropList[filigreeIndex].EnableWindow(true);
        m_buttonFiligreeRare[filigreeIndex].EnableWindow(true);
        // populate with the list of available filigree minus any selections
        // made in other filigree slots
        std::list<Augment> augments = CompatibleAugments("Filigree");
        for (size_t fi = 0; fi < MAX_Filigree; ++fi)
        {
            // do not remove any selection from current slot
            if (fi != filigreeIndex)
            {
                // get current selection (if any)
                std::string filigree = m_item.SentientIntelligence().Filigree(fi);
                if (filigree != "")
                {
                    // remove it from the augments list
                    std::list<Augment>::iterator it = augments.begin();
                    while (it != augments.end())
                    {
                        if ((*it).Name() == filigree)
                        {
                            it = augments.erase(it);
                        }
                        ++it;
                    }
                }
            }
        }
        // now add the available augments
        m_comboFiligreeDropList[filigreeIndex].ResetContent();
        std::string selectedFiligree =
                m_item.SentientIntelligence().Filigree(filigreeIndex);
        // build the image list for this control
        BuildImageList(&m_ilFiligree[filigreeIndex], augments);
        m_comboFiligreeDropList[filigreeIndex].SetImageList(&m_ilFiligree[filigreeIndex]);
        bool isRare = m_item.SentientIntelligence().IsRareFiligree(filigreeIndex);
        size_t index = 0;
        std::list<Augment>::iterator it = augments.begin();
        while (it != augments.end())
        {
            size_t pos = m_comboFiligreeDropList[filigreeIndex].AddString((*it).Name().c_str());
            m_comboFiligreeDropList[filigreeIndex].SetItemData(pos, index);
            if ((*it).Name() == selectedFiligree)
            {
                 // this is the selected augment, select it
                m_comboFiligreeDropList[filigreeIndex].SetCurSel(pos);
                m_buttonFiligreeRare[filigreeIndex].SetCheck(isRare ? BST_CHECKED : BST_UNCHECKED);
            }
            ++it;
            ++index;
        }
    }
    else
    {
        // disabled with no items or selections
        m_comboFiligreeDropList[filigreeIndex].EnableWindow(false);
        m_comboFiligreeDropList[filigreeIndex].ResetContent();
        m_buttonFiligreeRare[filigreeIndex].EnableWindow(false);
    }
}

void CItemSelectDialog::BuildImageList(
        CImageList * il,
        const std::list<Augment> & augments)
{
    il->DeleteImageList();
    il->Create(
            32,             // all icons are 32x32 pixels
            32,
            ILC_COLOR32,
            0,
            augments.size());
    std::list<Augment>::const_iterator it = augments.begin();
    while (it != augments.end())
    {
        (*it).AddImage(il);
        ++it;
    }
}

LRESULT CItemSelectDialog::OnHoverComboBox(WPARAM wParam, LPARAM lParam)
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
        // we have a selection, get the feats name
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
    return 0;
}

LRESULT CItemSelectDialog::OnMouseEnter(WPARAM wParam, LPARAM lParam)
{
    // is it a jewel or a filigree?
    if (wParam == m_comboHookHandles[0])
    {
        // its over the jewel
        int sel = m_comboSentientPersonality.GetCurSel();
        if (sel != CB_ERR)
        {
            CString augmentName;
            m_comboSentientPersonality.GetLBText(
                    sel,
                    augmentName);
            if (!augmentName.IsEmpty())
            {
                CRect itemRect;
                m_comboSentientPersonality.GetWindowRect(&itemRect);
                Augment augment = FindAugmentByName((LPCTSTR)augmentName);
                CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
                CPoint tipAlternate(itemRect.left, itemRect.top - 2);
                SetTooltipText(augment, tipTopLeft, tipAlternate, false);
            }
        }
    }
    else
    {
        // must be over one of the filigrees
        for (size_t i = 0 ; i < MAX_Filigree; ++i)
        {
            if (wParam == m_comboHookHandles[i+1])
            {
                int sel = m_comboFiligreeDropList[i].GetCurSel();
                if (sel != CB_ERR)
                {
                    CString augmentName;
                    m_comboFiligreeDropList[i].GetLBText(
                            sel,
                            augmentName);
                    if (!augmentName.IsEmpty())
                    {
                        CRect itemRect;
                        m_comboFiligreeDropList[i].GetWindowRect(&itemRect);
                        Augment augment = FindAugmentByName((LPCTSTR)augmentName);
                        CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
                        CPoint tipAlternate(itemRect.left, itemRect.top - 2);
                        SetTooltipText(augment, tipTopLeft, tipAlternate, false);
                        break;
                    }
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
