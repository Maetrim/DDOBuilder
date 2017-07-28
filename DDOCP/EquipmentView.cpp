// EquipmentView.cpp
//
#include "stdafx.h"
#include "EquipmentView.h"

#include "DDOCP.h"
#include "InventoryDialog.h"
#include "GlobalSupportFunctions.h"
#include "GearSetNameDialog.h"
#include "ItemSelectDialog.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
}

IMPLEMENT_DYNCREATE(CEquipmentView, CFormView)

CEquipmentView::CEquipmentView() :
    CFormView(CEquipmentView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_inventoryView(NULL)
{

}

CEquipmentView::~CEquipmentView()
{
    delete m_inventoryView;
    m_inventoryView = NULL;
}

void CEquipmentView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_GEAR_NAME, m_comboGearSelections);
    DDX_Control(pDX, IDC_BUTTON_NEW, m_buttonNew);
    DDX_Control(pDX, IDC_BUTTON_COPY, m_buttonCopy);
    DDX_Control(pDX, IDC_BUTTON_DELETE, m_buttonDelete);
    DDX_Control(pDX, IDC_ACTIVE_GEAR, m_buttonActiveGearSet);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CEquipmentView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
    ON_BN_CLICKED(IDC_BUTTON_COPY, OnButtonCopy)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
    ON_BN_CLICKED(IDC_ACTIVE_GEAR, OnButtonActiveGearSet)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CEquipmentView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEquipmentView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CEquipmentView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    CRect rctButton;
    m_buttonNew.GetWindowRect(&rctButton);
    ScreenToClient(&rctButton);
    CRect rctInventory(
            c_controlSpacing,
            rctButton.bottom + c_controlSpacing,
            c_controlSpacing + 223,
            rctButton.bottom + c_controlSpacing + 290);
    m_inventoryView = new CInventoryDialog(this);
    m_inventoryView->Create(CInventoryDialog::IDD, this);
    m_inventoryView->MoveWindow(&rctInventory);
    m_inventoryView->ShowWindow(SW_SHOW);
    m_inventoryView->AttachObserver(this);
    if (m_pCharacter != NULL)
    {
        m_inventoryView->SetGearSet(m_pCharacter->ActiveGearSet());
    }

    EnableControls();
}

void CEquipmentView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_inventoryView != NULL)
    {
        // position all the windows
        CRect rctCombo;
        m_comboGearSelections.GetWindowRect(&rctCombo);
        rctCombo -= rctCombo.TopLeft();
        rctCombo += CPoint(c_controlSpacing, c_controlSpacing);
        rctCombo.right = rctCombo.left + 223;   // same width as bitmap
        CRect rctNew;
        CRect rctCopy;
        CRect rctDelete;
        CRect rctMakeActive;
        m_buttonNew.GetWindowRect(&rctNew);
        m_buttonCopy.GetWindowRect(&rctCopy);
        m_buttonDelete.GetWindowRect(&rctDelete);
        m_buttonActiveGearSet.GetWindowRect(&rctMakeActive);
        rctNew -= rctNew.TopLeft();
        rctNew += CPoint(rctCombo.right + c_controlSpacing, c_controlSpacing);
        rctCopy -= rctCopy.TopLeft();
        rctCopy += CPoint(rctCombo.right + c_controlSpacing, rctNew.bottom + c_controlSpacing);
        rctDelete -= rctDelete.TopLeft();
        rctDelete += CPoint(rctCombo.right + c_controlSpacing, rctCopy.bottom + c_controlSpacing);
        rctMakeActive -= rctMakeActive.TopLeft();
        rctMakeActive += CPoint(rctNew.right + c_controlSpacing, c_controlSpacing);
        CRect rctInventory(
                c_controlSpacing,
                rctNew.bottom + c_controlSpacing,
                c_controlSpacing + 223,
                rctNew.bottom + c_controlSpacing + 290);
        m_comboGearSelections.MoveWindow(rctCombo);
        m_buttonNew.MoveWindow(rctNew);
        m_buttonCopy.MoveWindow(rctCopy);
        m_buttonDelete.MoveWindow(rctDelete);
        m_buttonActiveGearSet.MoveWindow(rctMakeActive);
        m_inventoryView->MoveWindow(rctInventory);
    }
}

LRESULT CEquipmentView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    //if (m_pCharacter != NULL)
    //{
    //    m_pCharacter->DetachObserver(this);
    //}

    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    //if (m_pCharacter != NULL)
    //{
    //    m_pCharacter->AttachObserver(this);
    //}
    //else
    //{
    //}
    PopulateCombobox();
    PopulateGear();
    EnableControls();
    if (m_pCharacter != NULL)
    {
        m_inventoryView->SetGearSet(m_pCharacter->ActiveGearSet());
    }
    else
    {
        EquippedGear emptyGear;
        m_inventoryView->SetGearSet(emptyGear);
    }
    return 0L;
}

BOOL CEquipmentView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_COMBO_GEAR_NAME,
        IDC_BUTTON_NEW,
        IDC_BUTTON_COPY,
        IDC_BUTTON_DELETE,
        IDC_ACTIVE_GEAR,
        0 // end marker
    };

    if (m_inventoryView != NULL
            && ::IsWindow(m_inventoryView->GetSafeHwnd())
            && ::IsWindowVisible(m_inventoryView->GetSafeHwnd()))
    {
        CRect rctWnd;
        m_inventoryView->GetWindowRect(&rctWnd);
        ScreenToClient(&rctWnd);
        pDC->ExcludeClipRect(&rctWnd);
    }

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CEquipmentView::PopulateCombobox()
{
    // show the list of selectable gear layouts
    m_comboGearSelections.LockWindowUpdate();
    m_comboGearSelections.ResetContent();
    bool selected = false;
    if (m_pCharacter != NULL)
    {
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        std::list<EquippedGear>::const_iterator it = setups.begin();
        size_t indexIntoList = 0;
        while (it != setups.end())
        {
            int index = m_comboGearSelections.AddString((*it).Name().c_str());
            m_comboGearSelections.SetItemData(index, indexIntoList);
            if ((*it).Name() == m_pCharacter->ActiveGear())
            {
                m_comboGearSelections.SetCurSel(index);
                m_buttonActiveGearSet.SetCheck(BST_CHECKED);
                selected = true;
            }
            ++indexIntoList;
            ++it;
        }
        if (!selected && setups.size() > 0)
        {
            // default to the first gear setup in the list
            m_comboGearSelections.SetCurSel(0);
        }
    }
    m_comboGearSelections.UnlockWindowUpdate();
}

std::string CEquipmentView::SelectedGearSet() const
{
    std::string gearSet;
    int sel = m_comboGearSelections.GetCurSel();
    if (sel != CB_ERR)
    {
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        std::list<EquippedGear>::const_iterator it = setups.begin();
        std::advance(it, sel);
        gearSet = (*it).Name();
    }
    return gearSet;
}

void CEquipmentView::EnableControls()
{
    // controls disabled if no document
    if (m_pCharacter == NULL)
    {
        m_comboGearSelections.EnableWindow(FALSE);
        m_buttonNew.EnableWindow(FALSE);
        m_buttonCopy.EnableWindow(FALSE);
        m_buttonDelete.EnableWindow(FALSE);
        m_inventoryView->EnableWindow(FALSE);
        m_buttonActiveGearSet.EnableWindow(FALSE);
    }
    else
    {
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        //m_comboGearSelections.EnableWindow(setups.size() > 0);
        //m_buttonNew.EnableWindow(TRUE);                 // can always create a new one
        //m_buttonCopy.EnableWindow(setups.size() > 0);   // can only copy a layout if one exists
        //m_buttonDelete.EnableWindow(setups.size() > 0); // can only delete if one exists
        m_inventoryView->EnableWindow(setups.size() > 0);
        //m_buttonActiveGearSet.EnableWindow(FALSE);
    }
}

void CEquipmentView::OnButtonNew()
{
    // create a new gear set that they must name
    CGearSetNameDialog dlg(this, m_pCharacter);
    if (dlg.DoModal() == IDOK)
    {
        EquippedGear newGear(dlg.Name());
        m_pCharacter->AddGearSet(newGear);
        // add it to the combo box
        // index is the count of gear items
        int index = m_comboGearSelections.AddString(dlg.Name().c_str());
        m_comboGearSelections.SetItemData(index, m_pCharacter->GearSetups().size());
        // new entry starts selected
        m_comboGearSelections.SetCurSel(index);
        PopulateGear();
        // have the correct enable state
        EnableControls();
    }
}

void CEquipmentView::OnButtonCopy()
{
    // create a new gear set from the current selected gear set
    // which they will have to name
}

void CEquipmentView::OnButtonDelete()
{
    // delete the current gear selection
    int sel = m_comboGearSelections.GetCurSel();
    if (sel != CB_ERR)
    {
        // get the name of the gear set to delete
        sel = m_comboGearSelections.GetItemData(sel);   // index into list
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        std::list<EquippedGear>::const_iterator it = setups.begin();
        std::advance(it, sel);
        std::string name = (*it).Name();
        m_pCharacter->DeleteGearSet(name);
        PopulateCombobox();
        // have the correct enable state
        EnableControls();
        // now show correct gear in inventory window
        PopulateGear();
    }
}

void CEquipmentView::OnButtonActiveGearSet()
{
    // deactivate or activate this gear set
}

void CEquipmentView::PopulateGear()
{
}

// InventoryObserver overrides
void CEquipmentView::UpdateSlotLeftClicked(
        CInventoryDialog * dialog,
        InventorySlotType slot)
{
    // determine the item selected in this slot already (if any)
    EquippedGear gear = m_pCharacter->GetGearSet(SelectedGearSet());
    Item item;
    if (gear.HasItemInSlot(slot))
    {
        item = gear.ItemInSlot(slot);
    }
    CItemSelectDialog dlg(this, slot, item);
    if (dlg.DoModal() == IDOK)
    {
        gear.SetItem(slot, dlg.SelectedItem());
        m_pCharacter->SetGear(SelectedGearSet(), gear);
        m_inventoryView->SetGearSet(gear);
    }
}

void CEquipmentView::UpdateSlotRightClicked(
        CInventoryDialog * dialog,
        InventorySlotType slot)
{
    AfxMessageBox("Inventory right clicked");
}

