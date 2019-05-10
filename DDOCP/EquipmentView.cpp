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
#include "XmlLib\SaxReader.h"

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
    DDX_Control(pDX, IDC_BUTTON_PASTE, m_buttonPaste);
    DDX_Control(pDX, IDC_BUTTON_DELETE, m_buttonDelete);
    DDX_Control(pDX, IDC_STATIC_NUM_FILIGREES, m_staticNumFiligrees);
    DDX_Control(pDX, IDC_COMBO_NUM_FILLIGREES, m_comboNumFiligrees);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CEquipmentView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_COMMAND(ID_GEAR_NEW, OnGearNew)
    ON_COMMAND(ID_GEAR_COPY, OnGearCopy)
    ON_COMMAND(ID_GEAR_PASTE, OnGearPaste)
    ON_COMMAND(ID_GEAR_DELETE, OnGearDelete)
    ON_UPDATE_COMMAND_UI(ID_GEAR_NEW, OnUpdateGearNew)
    ON_UPDATE_COMMAND_UI(ID_GEAR_COPY, OnUpdateGearCopy)
    ON_UPDATE_COMMAND_UI(ID_GEAR_PASTE, OnUpdateGearPaste)
    ON_UPDATE_COMMAND_UI(ID_GEAR_DELETE, OnUpdateGearDelete)
    ON_BN_CLICKED(IDC_BUTTON_NEW, OnGearNew)
    ON_BN_CLICKED(IDC_BUTTON_COPY, OnGearCopy)
    ON_BN_CLICKED(IDC_BUTTON_PASTE, OnGearPaste)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, OnGearDelete)
    ON_CBN_SELENDOK(IDC_COMBO_GEAR_NAME, OnGearSelectionSelEndOk)
    ON_CBN_SELENDOK(IDC_COMBO_NUM_FILLIGREES, OnGearNumFiligreesSelEndOk)
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
        m_inventoryView->SetGearSet(m_pCharacter, m_pCharacter->ActiveGearSet());
    }
    // Images for new/copy/delete buttons
    m_buttonNew.SetImage(IDB_BITMAP_NEW);
    m_buttonCopy.SetImage(IDB_BITMAP_COPY);
    m_buttonPaste.SetImage(IDB_BITMAP_PASTE);
    m_buttonDelete.SetImage(IDB_BITMAP_DELETE);

    EnableControls();
}

void CEquipmentView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_inventoryView != NULL)
    {
        // position all the windows
        // +---------------------------------------------------------+
        // | [Drop List Combo] [N][C][P][D] [Num Filigrees] [FopList]|
        // | +----------------------------+ +-----------------------+|
        // | |                            | |                       ||
        // | | Inventory Bitmap           | | Filigrees Bitmap      ||
        // | |                            | |                       ||
        // | +---------------------   ----+ +-----------------------+|
        CRect rctCombo;
        m_comboGearSelections.GetWindowRect(&rctCombo);
        rctCombo -= rctCombo.TopLeft();
        rctCombo += CPoint(c_controlSpacing, c_controlSpacing);
        CRect rctNew;
        CRect rctCopy;
        CRect rctPaste;
        CRect rctDelete;
        CRect rctNumFiligrees;
        CRect rctNumFiligreesCombo;
        m_buttonNew.GetWindowRect(&rctNew);
        m_buttonCopy.GetWindowRect(&rctCopy);
        m_buttonPaste.GetWindowRect(&rctPaste);
        m_buttonDelete.GetWindowRect(&rctDelete);

        rctDelete -= rctDelete.TopLeft();
        rctDelete += CSize(223 + c_controlSpacing - rctDelete.Width(), c_controlSpacing);
        rctPaste -= rctPaste.TopLeft();
        rctPaste += CSize(rctDelete.left - c_controlSpacing - rctPaste.Width(), c_controlSpacing);
        rctCopy -= rctCopy.TopLeft();
        rctCopy += CSize(rctPaste.left - c_controlSpacing - rctCopy.Width(), c_controlSpacing);
        rctNew -= rctNew.TopLeft();
        rctNew += CSize(rctCopy.left - c_controlSpacing - rctNew.Width(), c_controlSpacing);
        rctCombo.right = rctNew.left - c_controlSpacing;
        m_staticNumFiligrees.GetWindowRect(rctNumFiligrees);
        rctNumFiligrees -= rctNumFiligrees.TopLeft();
        rctNumFiligrees += CPoint(rctDelete.right + c_controlSpacing, c_controlSpacing);
        m_comboNumFiligrees.GetWindowRect(rctNumFiligreesCombo);
        rctNumFiligreesCombo -= rctNumFiligreesCombo.TopLeft();
        rctNumFiligreesCombo += CPoint(rctNumFiligrees.right + c_controlSpacing, c_controlSpacing);
        rctNumFiligreesCombo.bottom = cy;   // drop list to bottom of view

        CRect rctInventory(
                c_controlSpacing,
                rctDelete.bottom + c_controlSpacing,
                c_controlSpacing + 418,
                rctDelete.bottom + c_controlSpacing + 290);
        m_comboGearSelections.MoveWindow(rctCombo);
        m_buttonNew.MoveWindow(rctNew);
        m_buttonCopy.MoveWindow(rctCopy);
        m_buttonPaste.MoveWindow(rctPaste);
        m_buttonDelete.MoveWindow(rctDelete);
        m_inventoryView->MoveWindow(rctInventory);
        m_staticNumFiligrees.MoveWindow(rctNumFiligrees);
        m_comboNumFiligrees.MoveWindow(rctNumFiligreesCombo);
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
        if (IsWindow(GetSafeHwnd()))
        {
            SetScrollPos(SB_HORZ, 0, TRUE);
            SetScrollPos(SB_VERT, 0, TRUE);
        }
        m_pCharacter->AttachObserver(this);
    }
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
        IDC_BUTTON_NEW,
        IDC_BUTTON_COPY,
        IDC_BUTTON_PASTE,
        IDC_BUTTON_DELETE,
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

    // populate the Filigree combobox
    m_comboNumFiligrees.LockWindowUpdate();
    m_comboNumFiligrees.ResetContent();
    for (size_t i = 0; i <= MAX_FILIGREE; ++i)
    {
        CString text;
        text.Format("%d", i);
        m_comboNumFiligrees.AddString(text);
    }
    size_t count = 0;
    if (m_pCharacter != NULL)
    {
        count = m_pCharacter->ActiveGearSet().SentientIntelligence().NumFiligrees();
    }
    m_comboNumFiligrees.SetCurSel(count);
    m_comboNumFiligrees.UnlockWindowUpdate();
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
        m_buttonPaste.EnableWindow(FALSE);
        m_buttonDelete.EnableWindow(FALSE);
        m_staticNumFiligrees.EnableWindow(FALSE);
        m_comboNumFiligrees.EnableWindow(FALSE);
    }
    else
    {
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        m_comboGearSelections.EnableWindow(setups.size() > 1);
        m_inventoryView->EnableWindow(setups.size() > 0);
        m_buttonNew.EnableWindow(TRUE);     // always available
        m_buttonCopy.EnableWindow(setups.size() > 0);
        m_buttonPaste.EnableWindow(IsClipboardFormatAvailable(CF_PRIVATEFIRST));
        m_buttonDelete.EnableWindow(setups.size() > 0);
        m_staticNumFiligrees.EnableWindow(TRUE);
        m_comboNumFiligrees.EnableWindow(TRUE);
    }
}

void CEquipmentView::PopulateGear()
{
    if (m_inventoryView != NULL
            && m_pCharacter != NULL)
    {
        m_inventoryView->SetGearSet(m_pCharacter, m_pCharacter->ActiveGearSet());
    }
    else
    {
        EquippedGear emptyGear;
        m_inventoryView->SetGearSet(NULL, emptyGear);
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
            && !CanEquipTo2ndWeapon(m_pCharacter, gear.ItemInSlot(Inventory_Weapon1)))
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
            gear.SetItem(slot, m_pCharacter, dlg.SelectedItem());
            m_pCharacter->SetGear(SelectedGearSet(), slot, dlg.SelectedItem());
            m_inventoryView->SetGearSet(m_pCharacter, m_pCharacter->ActiveGearSet());
        }
        GetMouseHook()->RestoreState();
    }
}

void CEquipmentView::UpdateSlotRightClicked(
        CInventoryDialog * dialog,
        InventorySlotType slot)
{
    // option to clear the gear item from the selected slot
    EquippedGear gear = m_pCharacter->GetGearSet(SelectedGearSet());
    if (gear.HasItemInSlot(slot))
    {
        int sel = AfxMessageBox("Clear the equipped item from this slot?", MB_YESNO);
        if (sel == IDYES)
        {
            m_pCharacter->ClearGearInSlot(SelectedGearSet(), slot);
            m_inventoryView->SetGearSet(m_pCharacter, m_pCharacter->ActiveGearSet());
        }
    }
}

void CEquipmentView::UpdateGearChanged(Character * charData, InventorySlotType slot)
{
    m_inventoryView->SetGearSet(m_pCharacter, m_pCharacter->ActiveGearSet());
}

void CEquipmentView::OnUpdateGearNew(CCmdUI * pCmdUi)
{
    // can always create a new gear set if a document is open
    pCmdUi->Enable(m_pCharacter != NULL);
}

void CEquipmentView::OnUpdateGearCopy(CCmdUI * pCmdUi)
{
    if (m_pCharacter != NULL)
    {
        // can copy a gear set if we have an active one
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        pCmdUi->Enable(setups.size() > 0);
    }
    else
    {
        pCmdUi->Enable(FALSE);
    }
}

void CEquipmentView::OnUpdateGearPaste(CCmdUI * pCmdUi)
{
    // can paste if we have data of the correct format available on the clipboard
    bool enable = m_pCharacter != NULL
            && ::IsClipboardFormatAvailable(CF_PRIVATEFIRST);
    pCmdUi->Enable(enable);
    m_buttonPaste.EnableWindow(enable);
}

void CEquipmentView::OnUpdateGearDelete(CCmdUI * pCmdUi)
{
    if (m_pCharacter != NULL)
    {
        // can delete a gear set if we have an active one
        const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
        pCmdUi->Enable(setups.size() > 0);
    }
    else
    {
        pCmdUi->Enable(FALSE);
    }
}

void CEquipmentView::OnGearNew()
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
            // once added the new gear set automatically becomes active
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

void CEquipmentView::OnGearCopy()
{
    // copy the current gear set to the clipboard as xml text using a custom
    // clipboard format to preserve any other data present on the clipboard
    HGLOBAL hData = NULL;
    if (m_pCharacter!= NULL)
    {
        // get the current gear
        EquippedGear gear = m_pCharacter->GetGearSet(SelectedGearSet());
        // write the gear as xml text
        XmlLib::SaxWriter writer;
        gear.Write(&writer);
        std::string xmlText = writer.Text();

        // read the file into a global memory handle
        hData = GlobalAlloc(GMEM_MOVEABLE, xmlText.size() + 1); // space for \0
        if (hData != NULL)
        {
            char * buffer = (char *)GlobalLock(hData);
            strcpy_s(buffer, xmlText.size() + 1, xmlText.data());
            GlobalUnlock(hData);
        }
        if (::OpenClipboard(NULL))
        {
            ::SetClipboardData(CF_PRIVATEFIRST, hData);
            ::CloseClipboard();
            // if we copied, paste is available
            m_buttonPaste.EnableWindow(true);
        }
        else
        {
            AfxMessageBox("Failed to open the clipboard.", MB_ICONERROR);
        }
    }
}

void CEquipmentView::OnGearPaste()
{
    if (m_pCharacter != NULL
            && ::IsClipboardFormatAvailable(CF_PRIVATEFIRST))
    {
        if (::OpenClipboard(NULL))
        {
            // is the data in the right format
            HGLOBAL hGlobal = ::GetClipboardData(CF_PRIVATEFIRST);
            if (hGlobal != NULL)
            {
                // get the data as text from the clipboard
                std::string xmlText;
                char * buffer = (char *)::GlobalLock(hGlobal);
                xmlText = buffer;
                GlobalUnlock(hGlobal);
                ::CloseClipboard();

                // parse the xml text and read into a local object
                EquippedGear gear;
                XmlLib::SaxReader reader(&gear, gear.ElementName());
                bool ok = reader.ParseText(xmlText);
                if (ok)
                {
                    // looks like it read ok, we can progress
                    CGearSetNameDialog dlg(this, m_pCharacter);
                    if (dlg.DoModal() == IDOK)
                    {
                        // ensure the gear set name is unique
                        if (!m_pCharacter->DoesGearSetExist(dlg.Name()))
                        {
                            gear.Set_Name(dlg.Name());
                            m_pCharacter->AddGearSet(gear);
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
                        else
                        {
                            // gear sets names must be unique, they can paste again
                            AfxMessageBox("A Gear Set with that name already exists.", MB_ICONERROR);
                        }
                    }
                }
                else
                {
                    // something is wrong with the data on the clipboard
                    AfxMessageBox("Failed to read data from clipboard.", MB_ICONERROR);
                }
            }
        }
        else
        {
            AfxMessageBox("Failed to open the clipboard.", MB_ICONERROR);
        }
    }
    else
    {
        // no data of correct format on the clipboard
        AfxMessageBox("No Gear Set data available on the clipboard.", MB_ICONERROR);
    }
}

void CEquipmentView::OnGearDelete()
{
    // delete the current gear selection
    int sel = m_comboGearSelections.GetCurSel();
    if (sel != CB_ERR)
    {
        // get the name of the gear set to delete
        CString name;
        m_comboGearSelections.GetLBText(sel, name);
        // double check that is what they really want
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

void CEquipmentView::OnGearNumFiligreesSelEndOk()
{
    int sel = m_comboNumFiligrees.GetCurSel();
    if (sel != CB_ERR)
    {
        // applies to active gear set
        m_pCharacter->SetNumFiligrees(sel);
        PopulateGear();
    }
}
