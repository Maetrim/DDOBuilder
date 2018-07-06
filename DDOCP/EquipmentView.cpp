// EquipmentView.cpp
//
#include "stdafx.h"
#include "EquipmentView.h"

#include "DDOCP.h"
#include "InventoryDialog.h"
#include "GlobalSupportFunctions.h"
#include "GearSetNameDialog.h"
#include "ItemSelectDialog.h"
#include "MouseHook.h"

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
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CEquipmentView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_COMMAND(ID_GEAR_NEW, OnEditGearNew)
    ON_COMMAND(ID_GEAR_COPY, OnEditGearCopy)
    ON_COMMAND(ID_GEAR_DELETE, OnEditGearDelete)
    ON_UPDATE_COMMAND_UI(ID_GEAR_NEW, OnUpdateEditGearNew)
    ON_UPDATE_COMMAND_UI(ID_GEAR_COPY, OnUpdateEditGearCopy)
    ON_UPDATE_COMMAND_UI(ID_GEAR_DELETE, OnUpdateEditGearDelete)
    ON_BN_CLICKED(IDC_BUTTON_NEW, OnEditGearNew)
    ON_BN_CLICKED(IDC_BUTTON_COPY, OnEditGearCopy)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, OnEditGearDelete)
    ON_CBN_SELENDOK(IDC_COMBO_GEAR_NAME, OnGearSelectionSelEndOk)
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
    // Images for new/copy/delete buttons
    m_buttonNew.SetImage(IDB_BITMAP_NEW);
    m_buttonCopy.SetImage(IDB_BITMAP_COPY);
    m_buttonDelete.SetImage(IDB_BITMAP_DELETE);

    EnableControls();
}

void CEquipmentView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_inventoryView != NULL)
    {
        // position all the windows
        // +-----------------------------+
        // | [Drop List Combo] [N][C][D] |
        // | +-------------------------+ |
        // | |                         | |
        // | | Inventory Bitmap        | |
        // | |                         | |
        // | +-------------------------+ |
        CRect rctCombo;
        m_comboGearSelections.GetWindowRect(&rctCombo);
        rctCombo -= rctCombo.TopLeft();
        rctCombo += CPoint(c_controlSpacing, c_controlSpacing);
        CRect rctNew;
        CRect rctCopy;
        CRect rctDelete;
        m_buttonNew.GetWindowRect(&rctNew);
        m_buttonCopy.GetWindowRect(&rctCopy);
        m_buttonDelete.GetWindowRect(&rctDelete);

        rctDelete -= rctDelete.TopLeft();
        rctDelete += CSize(223 - c_controlSpacing - rctDelete.Width(), c_controlSpacing);
        rctCopy -= rctCopy.TopLeft();
        rctCopy += CSize(rctDelete.left - c_controlSpacing - rctCopy.Width(), c_controlSpacing);
        rctNew -= rctNew.TopLeft();
        rctNew += CSize(rctCopy.left - c_controlSpacing - rctNew.Width(), c_controlSpacing);
        rctCombo.right = rctNew.left - c_controlSpacing;

        CRect rctInventory(
                c_controlSpacing,
                rctDelete.bottom + c_controlSpacing,
                c_controlSpacing + 223,
                rctDelete.bottom + c_controlSpacing + 290);
        m_comboGearSelections.MoveWindow(rctCombo);
        m_buttonNew.MoveWindow(rctNew);
        m_buttonCopy.MoveWindow(rctCopy);
        m_buttonDelete.MoveWindow(rctDelete);
        m_inventoryView->MoveWindow(rctInventory);
        SetScrollSizes(
                MM_TEXT,
                CSize(
                        rctInventory.right + c_controlSpacing,
                        rctInventory.bottom + c_controlSpacing));
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
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
    //else
    //{
    //}
    PopulateCombobox();
    PopulateGear();
    EnableControls();
    return 0L;
}

BOOL CEquipmentView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_COMBO_GEAR_NAME,
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
            if ((*it).Name() == m_pCharacter->ActiveGear())
            {
                m_comboGearSelections.SetCurSel(index);
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
        CString name;
        m_comboGearSelections.GetLBText(sel, name);
        gearSet = (LPCTSTR)name;
    }
    return gearSet;
}

void CEquipmentView::EnableControls()
{
    // controls disabled if no document
    if (m_pCharacter == NULL)
    {
        m_comboGearSelections.EnableWindow(FALSE);
        m_inventoryView->EnableWindow(FALSE);
        m_buttonNew.EnableWindow(FALSE);
        m_buttonCopy.EnableWindow(FALSE);
        m_buttonDelete.EnableWindow(FALSE);
    }
    else
    {
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        m_comboGearSelections.EnableWindow(setups.size() > 1);
        m_inventoryView->EnableWindow(setups.size() > 0);
        m_buttonNew.EnableWindow(TRUE);     // always available
        m_buttonCopy.EnableWindow(setups.size() > 0);
        m_buttonDelete.EnableWindow(setups.size() > 0);
    }
}

void CEquipmentView::PopulateGear()
{
    if (m_inventoryView != NULL
            && m_pCharacter != NULL)
    {
        m_inventoryView->SetGearSet(m_pCharacter->ActiveGearSet());
    }
    else
    {
        EquippedGear emptyGear;
        m_inventoryView->SetGearSet(emptyGear);
    }
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
    if (slot == Inventory_Weapon2
            && gear.HasItemInSlot(Inventory_Weapon1)
            && !CanEquipTo2ndWeapon(gear.ItemInSlot(Inventory_Weapon1)))
    {
        // not allowed to equip in this due to item in weapon slot 1
        ::MessageBeep(MB_OK);
    }
    else
    {
        // no tooltips while a dialog is displayed
        GetMouseHook()->SaveState();
        CItemSelectDialog dlg(this, slot, item, m_pCharacter);
        if (dlg.DoModal() == IDOK)
        {
            gear.SetItem(slot, dlg.SelectedItem());
            m_pCharacter->SetGear(SelectedGearSet(), slot, dlg.SelectedItem());
            m_inventoryView->SetGearSet(m_pCharacter->ActiveGearSet());
        }
        GetMouseHook()->RestoreState();
    }
}

void CEquipmentView::UpdateSlotRightClicked(
        CInventoryDialog * dialog,
        InventorySlotType slot)
{
    EquippedGear gear = m_pCharacter->GetGearSet(SelectedGearSet());
    if (gear.HasItemInSlot(slot))
    {
        int sel = AfxMessageBox("Clear the equipped item from this slot?", MB_YESNO);
        if (sel == IDYES)
        {
            m_pCharacter->ClearGearInSlot(SelectedGearSet(), slot);
            m_inventoryView->SetGearSet(m_pCharacter->ActiveGearSet());
        }
    }
}

void CEquipmentView::UpdateGearChanged(Character * charData, InventorySlotType slot)
{
    m_inventoryView->SetGearSet(m_pCharacter->ActiveGearSet());
}

void CEquipmentView::OnUpdateEditGearNew(CCmdUI * pCmdUi)
{
    pCmdUi->Enable(m_pCharacter != NULL);
}

void CEquipmentView::OnUpdateEditGearCopy(CCmdUI * pCmdUi)
{
    if (m_pCharacter != NULL)
    {
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        pCmdUi->Enable(setups.size() > 0);
    }
    else
    {
        pCmdUi->Enable(FALSE);
    }
}

void CEquipmentView::OnUpdateEditGearDelete(CCmdUI * pCmdUi)
{
    if (m_pCharacter != NULL)
    {
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        pCmdUi->Enable(setups.size() > 0);
    }
    else
    {
        pCmdUi->Enable(FALSE);
    }
}

void CEquipmentView::OnEditGearNew()
{
    // no tooltips while a dialog is displayed
    GetMouseHook()->SaveState();
    // create a new gear set that they must name
    CGearSetNameDialog dlg(this, m_pCharacter);
    if (dlg.DoModal() == IDOK)
    {
        // ensure the gear set name is unique
        if (!m_pCharacter->DoesGearSetExist(dlg.Name()))
        {
            EquippedGear newGear(dlg.Name());
            m_pCharacter->AddGearSet(newGear);
            // onc added the new gear set automatically becomes active
            m_pCharacter->SetActiveGearSet(dlg.Name());

            // add it to the combo box
            // index is the count of gear items
            int index = m_comboGearSelections.AddString(dlg.Name().c_str());
            // new entry starts selected
            m_comboGearSelections.SetCurSel(index);
            PopulateGear();
            // have the correct enable state
            EnableControls();
        }
        else
        {
            AfxMessageBox(
                    "Error: A gear set must have a unique name.\n"
                    "A gear set with the name you selected already exists.\n"
                    "Try again but use a different name.",
                    MB_ICONERROR);
        }
    }
    GetMouseHook()->RestoreState();
}

void CEquipmentView::OnEditGearCopy()
{
    // create a new gear set from the current selected gear set
    // which they will have to name
    // create a new gear set that they must name
    CGearSetNameDialog dlg(this, m_pCharacter);
    if (dlg.DoModal() == IDOK)
    {
        // ensure the gear set name is unique
        if (!m_pCharacter->DoesGearSetExist(dlg.Name()))
        {
            EquippedGear copyGear = m_pCharacter->ActiveGearSet();
            copyGear.Set_Name(dlg.Name());
            m_pCharacter->AddGearSet(copyGear);
            // once added the copy gear set automatically becomes active
            m_pCharacter->SetActiveGearSet(dlg.Name());

            // add it to the combo box
            // index is the count of gear items
            int index = m_comboGearSelections.AddString(dlg.Name().c_str());
            // new entry starts selected
            m_comboGearSelections.SetCurSel(index);
            PopulateGear();
            // have the correct enable state
            EnableControls();
        }
    }
}

void CEquipmentView::OnEditGearDelete()
{
    // delete the current gear selection
    int sel = m_comboGearSelections.GetCurSel();
    if (sel != CB_ERR)
    {
        // get the name of the gear set to delete
        CString name;
        m_comboGearSelections.GetLBText(sel, name);
        // double check thats what they really want
        CString message;
        message.Format("Are you sure you want to delete the gear set called:\n"
            "\"%s\"?", name);
        UINT ret = AfxMessageBox(message, MB_ICONQUESTION | MB_YESNO);
        if (ret == IDYES)
        {
            m_pCharacter->DeleteGearSet((LPCTSTR)name);
            PopulateCombobox();
            // have the correct enable state
            EnableControls();
            // now show correct gear in inventory window
            PopulateGear();
        }
    }
}

void CEquipmentView::OnGearSelectionSelEndOk()
{
    int sel = m_comboGearSelections.GetCurSel();
    if (sel != CB_ERR)
    {
        CString name;
        m_comboGearSelections.GetLBText(sel, name);
        m_pCharacter->SetActiveGearSet((LPCTSTR)name);
        PopulateGear();
    }
}
