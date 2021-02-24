// EnhancementEditorDialog.cpp
//

#include "stdafx.h"
#include "DDOCP.h"
#include "EnhancementEditorDialog.h"
#include "DestinyTreeDialog.h"
#include "EnhancementsFile.h"
#include "GlobalSupportFunctions.h"

// CEnhancementEditorDialog dialog

IMPLEMENT_DYNAMIC(CEnhancementEditorDialog, CDialog)

CEnhancementEditorDialog::CEnhancementEditorDialog(CWnd* pParent, Character * pCharacter) :
    CDialog(CEnhancementEditorDialog::IDD, pParent),
    m_pCharacter(pCharacter),
    m_pTreeDialog(NULL)
{

}

CEnhancementEditorDialog::~CEnhancementEditorDialog()
{
    delete m_pTreeDialog;
}

void CEnhancementEditorDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_INTERNAL_NAME, m_editInternalName);
    DDX_Control(pDX, IDC_EDIT_MIN_SPENT, m_editMinSpent);
    DDX_Control(pDX, IDC_EDIT_RANKS, m_editRanks);
    DDX_Control(pDX, IDC_EDIT_FILENAME, m_editFilename);
    DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
    DDX_Control(pDX, IDC_EDIT_COST, m_editCost);
    DDX_Control(pDX, IDC_EDIT_TITLE, m_editTitle);
    DDX_Control(pDX, IDC_ENHANCEMENT, m_comboList);
    DDX_Control(pDX, IDC_CHECK_ARROWLEFT, m_buttonArrowLeft);
    DDX_Control(pDX, IDC_CHECK_ARROWRIGHT, m_buttonArrowRight);
    DDX_Control(pDX, IDC_CHECK_ARROWUP, m_buttonArrowUp);
    DDX_Control(pDX, IDC_CHECK_LONGARROWUP, m_buttonLongArrowUp);
    DDX_Control(pDX, IDC_CHECK_EXTRALONGARROWUP, m_buttonExtraLongArrowUp);
    DDX_Control(pDX, IDC_CHECK_CLICKIE, m_buttonClickie);
    DDX_Control(pDX, IDC_EDIT_ICON, m_editIcon);
    DDX_Control(pDX, IDC_EDIT_FEATNAME, m_editFeatName);
    DDX_Control(pDX, IDC_COMBO_NUM_SELECTIONS, m_comboNumSelections);
    DDX_Control(pDX, IDC_COMBO_EDIT_SELECTION, m_comboSelection);
    DDX_Control(pDX, IDC_EDIT_SELECTION_TITLE, m_editSelectionTitle);
    DDX_Control(pDX, IDC_EDIT_SELECTION_DESCRIPTION, m_editSelectionDescription);
    DDX_Control(pDX, IDC_EDIT_ICON2, m_editSelectionIcon);
    DDX_Control(pDX, IDC_EDIT_COST2, m_editSelectionCost);
    DDX_Control(pDX, IDC_EDIT_FEATNAME2, m_editFeatSelection);
    DDX_Control(pDX, IDC_BUTTON_ADD_FEATREQUIREMENT2, m_buttonAddFeatRequirementSelection);
    DDX_Control(pDX, IDC_EDIT_CLASS, m_editClass);
    DDX_Control(pDX, IDC_EDIT_CLASSLEVEL, m_editClassLevel);
}

BEGIN_MESSAGE_MAP(CEnhancementEditorDialog, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CEnhancementEditorDialog::OnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CEnhancementEditorDialog::OnClickedButtonSave)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CEnhancementEditorDialog::OnClickedButtonDelete)
    ON_BN_CLICKED(IDC_CHECK_ARROWLEFT, &CEnhancementEditorDialog::OnClickedCheckArrowleft)
    ON_BN_CLICKED(IDC_CHECK_ARROWRIGHT, &CEnhancementEditorDialog::OnClickedCheckArrowright)
    ON_BN_CLICKED(IDC_CHECK_ARROWUP, &CEnhancementEditorDialog::OnClickedCheckArrowup)
    ON_BN_CLICKED(IDC_CHECK_LONGARROWUP, &CEnhancementEditorDialog::OnClickedCheckLongarrowup)
    ON_BN_CLICKED(IDC_CHECK_EXTRALONGARROWUP, &CEnhancementEditorDialog::OnClickedCheckExtraLongarrowup)
    ON_EN_KILLFOCUS(IDC_EDIT_TITLE, &CEnhancementEditorDialog::OnKillFocusEditTitle)
    ON_EN_KILLFOCUS(IDC_EDIT_INTERNAL_NAME, &CEnhancementEditorDialog::OnKillFocusEditInternalName)
    ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, &CEnhancementEditorDialog::OnKillFocusEditDescription)
    ON_EN_KILLFOCUS(IDC_EDIT_COST, &CEnhancementEditorDialog::OnKillFocusEditCost)
    ON_EN_KILLFOCUS(IDC_EDIT_RANKS, &CEnhancementEditorDialog::OnKillFocusEditRanks)
    ON_EN_KILLFOCUS(IDC_EDIT_MIN_SPENT, &CEnhancementEditorDialog::OnKillFocusEditMinSpent)
    ON_CBN_SELENDOK(IDC_ENHANCEMENT, &CEnhancementEditorDialog::OnSelendokEnhancement)
    ON_BN_CLICKED(IDC_CHECK_CLICKIE, &CEnhancementEditorDialog::OnClickedCheckClickie)
    ON_EN_KILLFOCUS(IDC_EDIT_ICON, &CEnhancementEditorDialog::OnKillfocusEditIcon)
    ON_BN_CLICKED(IDC_BUTTON_ADD_FEATREQUIREMENT, &CEnhancementEditorDialog::OnClickedButtonAddFeatRequirement)
    ON_CBN_SELENDOK(IDC_COMBO_NUM_SELECTIONS, &CEnhancementEditorDialog::OnSelendokComboNumSelections)
    ON_CBN_SELENDOK(IDC_COMBO_EDIT_SELECTION, &CEnhancementEditorDialog::OnSelendokComboEditSelection)
    ON_WM_KILLFOCUS()
    ON_EN_KILLFOCUS(IDC_EDIT_ICON2, &CEnhancementEditorDialog::OnKillfocusEditIcon2)
    ON_EN_KILLFOCUS(IDC_EDIT_SELECTION_DESCRIPTION, &CEnhancementEditorDialog::OnKillfocusEditSelectionDescription)
    ON_EN_KILLFOCUS(IDC_EDIT_COST2, &CEnhancementEditorDialog::OnKillfocusEditCost2)
    ON_EN_KILLFOCUS(IDC_EDIT_SELECTION_TITLE, &CEnhancementEditorDialog::OnKillfocusEditSelectionTitle)
    ON_BN_CLICKED(IDC_BUTTON_ADD_FEATREQUIREMENT2, &CEnhancementEditorDialog::OnClickedButtonAddFeatRequirementSelection)
    ON_BN_CLICKED(IDC_BUTTON_ADD_CLASSREQUIREMENT, &CEnhancementEditorDialog::OnClickedButtonAddClassrequirement)
    ON_BN_CLICKED(IDC_BUTTONICON1_TRANSPARENT, &CEnhancementEditorDialog::OnClickedButtonicon1Transparent)
    ON_BN_CLICKED(IDC_BUTTONICON2_TRANSPARENT, &CEnhancementEditorDialog::OnClickedButtonicon2Transparent)
END_MESSAGE_MAP()

// CEnhancementEditorDialog message handlers
void CEnhancementEditorDialog::OnClickedButtonBrowse()
{
    CFileDialog dlg(
            TRUE,
            NULL,
            NULL,
            OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,
            "XML Files (*.xml)|*.xml|All files (*.*)|*.*||",
            this);
    if (dlg.DoModal() == IDOK)
    {
        // update the filename
        CString name = dlg.GetPathName();
        m_editFilename.SetWindowText(name);
        EnhancementsFile file((LPCSTR)name);
        file.Read();
        m_loadedTrees = file.EnhancementTrees();
        ASSERT(m_loadedTrees.size() == 1);
        UpdateTree();
        PopulateComboList();
    }
}

void CEnhancementEditorDialog::UpdateTree()
{
    CWnd *pFocusWnd = GetFocus();
    // destroy and create the enhancement tree
    if (m_pTreeDialog != NULL)
    {
        if (pFocusWnd == m_pTreeDialog)
        {
            pFocusWnd = NULL;
        }
        m_pTreeDialog->DestroyWindow();
        delete m_pTreeDialog;
        m_pTreeDialog = NULL;
    }
    const size_t c_sizeX = 300;
    const size_t c_sizeY = 526;
    CRect itemRect(425, 0, 425 + c_sizeX, 0 + c_sizeY);
    // show an enhancement dialog
    m_pTreeDialog = new CDestinyTreeDialog(
            this,
            m_pCharacter,
            m_loadedTrees.front(),
            TT_enhancement);        // type doesn't matter in editor
    m_pTreeDialog->Create(CDestinyTreeDialog::IDD, this);
    m_pTreeDialog->MoveWindow(&itemRect);
    m_pTreeDialog->ShowWindow(SW_SHOW);
    if (pFocusWnd != NULL)
    {
        pFocusWnd->SetFocus();
    }
}

void CEnhancementEditorDialog::PopulateComboList()
{
    std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
    std::list<EnhancementTreeItem>::iterator it = items.begin();
    int sel = m_comboList.GetCurSel();
    m_comboList.ResetContent();
    while (it != items.end())
    {
        CString name = (*it).InternalName().c_str();
        m_comboList.AddString(name);
        ++it;
    }
    m_comboList.SetCurSel(sel);
    OnSelendokEnhancement();
}

void CEnhancementEditorDialog::OnClickedButtonSave()
{
    CString name;
    m_editFilename.GetWindowText(name);
    EnhancementsFile file((LPCSTR)name);
    file.m_loadedTrees = m_loadedTrees;
    file.Save();
}

void CEnhancementEditorDialog::OnClickedButtonDelete()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        items.erase(it);
        m_loadedTrees.front().Set_Items(items);
        PopulateComboList();
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnClickedCheckArrowleft()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        int checked = m_buttonArrowLeft.GetCheck();
        if (checked)
        {
            (*it).Set_ArrowLeft();
            std::string name = (*it).InternalName();
            it = FindItem(items, (*it).XPosition() - 1, (*it).YPosition());
            if (it != items.end())
            {
                // add a requirement to this item
                Requirements requirements = (*it).RequirementsToTrain();
                std::list<Requirement> reqs = requirements.Requires();
                Requirement r;
                r.Set_Enhancement(name);
                reqs.push_back(r);
                requirements.Set_Requires(reqs);
                (*it).Set_RequirementsToTrain(requirements);
            }
        }
        else
        {
            (*it).Clear_ArrowLeft();
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnClickedCheckArrowright()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        int checked = m_buttonArrowRight.GetCheck();
        if (checked)
        {
            (*it).Set_ArrowRight();
            std::string name = (*it).InternalName();
            it = FindItem(items, (*it).XPosition() + 1, (*it).YPosition());
            if (it != items.end())
            {
                // add a requirement to this item
                Requirements requirements = (*it).RequirementsToTrain();
                std::list<Requirement> reqs = requirements.Requires();
                Requirement r;
                r.Set_Enhancement(name);
                reqs.push_back(r);
                requirements.Set_Requires(reqs);
                (*it).Set_RequirementsToTrain(requirements);
            }
        }
        else
        {
            (*it).Clear_ArrowRight();
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnClickedCheckArrowup()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        int checked = m_buttonArrowUp.GetCheck();
        if (checked)
        {
            (*it).Set_ArrowUp();
            std::string name = (*it).InternalName();
            it = FindItem(items, (*it).XPosition(), (*it).YPosition() + 1);
            if (it != items.end())
            {
                // add a requirement to this item
                Requirements requirements = (*it).RequirementsToTrain();
                std::list<Requirement> reqs = requirements.Requires();
                Requirement r;
                r.Set_Enhancement(name);
                reqs.push_back(r);
                requirements.Set_Requires(reqs);
                (*it).Set_RequirementsToTrain(requirements);
            }
        }
        else
        {
            (*it).Clear_ArrowUp();
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnClickedCheckLongarrowup()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        int checked = m_buttonLongArrowUp.GetCheck();
        if (checked)
        {
            (*it).Set_LongArrowUp();
            std::string name = (*it).InternalName();
            it = FindItem(items, (*it).XPosition(), (*it).YPosition() + 2);
            if (it != items.end())
            {
                // add a requirement to this item
                Requirements requirements = (*it).RequirementsToTrain();
                std::list<Requirement> reqs = requirements.Requires();
                Requirement r;
                r.Set_Enhancement(name);
                reqs.push_back(r);
                requirements.Set_Requires(reqs);
                (*it).Set_RequirementsToTrain(requirements);
            }
        }
        else
        {
            (*it).Clear_LongArrowUp();
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnClickedCheckExtraLongarrowup()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        int checked = m_buttonExtraLongArrowUp.GetCheck();
        if (checked)
        {
            (*it).Set_ExtraLongArrowUp();
            std::string name = (*it).InternalName();
            it = FindItem(items, (*it).XPosition(), (*it).YPosition() + 3);
            if (it != items.end())
            {
                // add a requirement to this item
                Requirements requirements = (*it).RequirementsToTrain();
                std::list<Requirement> reqs = requirements.Requires();
                Requirement r;
                r.Set_Enhancement(name);
                reqs.push_back(r);
                requirements.Set_Requires(reqs);
                (*it).Set_RequirementsToTrain(requirements);
            }
        }
        else
        {
            (*it).Clear_ExtraLongArrowUp();
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnKillFocusEditTitle()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        CString text;
        m_editTitle.GetWindowText(text);
        if (text != (*it).Name().c_str())
        {
            (*it).Set_Name((LPCTSTR)text);
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnKillFocusEditInternalName()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        CString text;
        m_editInternalName.GetWindowText(text);
        if (text != (*it).InternalName().c_str())
        {
            (*it).Set_InternalName((LPCTSTR)text);
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
            PopulateComboList();
        }
    }
}

void CEnhancementEditorDialog::OnKillFocusEditDescription()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        CString text;
        m_editDescription.GetWindowText(text);
        if (text != (*it).Description().c_str())
        {
            (*it).Set_Description((LPCTSTR)text);
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnKillFocusEditCost()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        CString text;
        m_editCost.GetWindowText(text);
        if (atoi(text) != (*it).Cost())
        {
            (*it).Set_Cost(atoi(text));
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnKillFocusEditRanks()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        CString text;
        m_editRanks.GetWindowText(text);
        if (atoi(text) != (*it).Ranks())
        {
            (*it).Set_Ranks(atoi(text));
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnKillFocusEditMinSpent()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        CString text;
        m_editMinSpent.GetWindowText(text);
        if (atoi(text) != (*it).MinSpent())
        {
            (*it).Set_MinSpent(atoi(text));
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

BOOL CEnhancementEditorDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  Add extra initialization here

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CEnhancementEditorDialog::OnSelendokEnhancement()
{
    // populate controls with the selections entries
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        m_editInternalName.SetWindowText((*it).InternalName().c_str());
        m_editTitle.SetWindowText((*it).Name().c_str());
        m_editDescription.SetWindowText((*it).Description().c_str());
        m_editIcon.SetWindowText((*it).Icon().c_str());
        m_editFeatName.SetWindowText("");
        CString text;
        text.Format("%d", (*it).Cost());
        m_editCost.SetWindowText(text);
        text.Format("%d", (*it).Ranks());
        m_editRanks.SetWindowText(text);
        text.Format("%d", (*it).MinSpent());
        m_editMinSpent.SetWindowText(text);
        m_buttonArrowLeft.SetCheck((*it).HasArrowLeft() ? BST_CHECKED : BST_UNCHECKED);
        m_buttonArrowRight.SetCheck((*it).HasArrowRight() ? BST_CHECKED : BST_UNCHECKED);
        m_buttonArrowUp.SetCheck((*it).HasArrowUp() ? BST_CHECKED : BST_UNCHECKED);
        m_buttonLongArrowUp.SetCheck((*it).HasLongArrowUp() ? BST_CHECKED : BST_UNCHECKED);
        m_buttonExtraLongArrowUp.SetCheck((*it).HasExtraLongArrowUp() ? BST_CHECKED : BST_UNCHECKED);
        m_buttonClickie.SetCheck((*it).HasClickie() ? BST_CHECKED : BST_UNCHECKED);

        // now setup the selection controls
        SetupSelections();
    }
}

void CEnhancementEditorDialog::SetupSelections()
{
    // populate controls with the selections entries
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        m_comboNumSelections.EnableWindow(TRUE);
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        if ((*it).HasSelections())
        {
            size_t count = (*it).Selections().Selections().size();
            m_comboNumSelections.SetCurSel(count);
            m_comboSelection.EnableWindow(TRUE);
            m_editSelectionTitle.EnableWindow(TRUE);
            m_editSelectionDescription.EnableWindow(TRUE);
            m_editSelectionIcon.EnableWindow(TRUE);
            m_editSelectionCost.EnableWindow(TRUE);
            m_buttonAddFeatRequirementSelection.EnableWindow(TRUE);
            // populate the selector combo
            m_comboSelection.ResetContent();
            for (size_t i = 0; i < count; ++i)
            {
                CString text;
                text.Format("Selection %d", (i + 1));
                m_comboSelection.AddString(text);
            }
            m_comboSelection.SetCurSel(0);
            PopulateSelection();
        }
        else
        {
            m_comboNumSelections.SetCurSel(0);      // no selections, but can be changed
            m_comboSelection.EnableWindow(FALSE);
            m_editSelectionTitle.EnableWindow(FALSE);
            m_editSelectionDescription.EnableWindow(FALSE);
            m_editSelectionIcon.EnableWindow(FALSE);
            m_editSelectionCost.EnableWindow(FALSE);
            m_buttonAddFeatRequirementSelection.EnableWindow(TRUE);
        }
    }
    else
    {
        m_comboNumSelections.EnableWindow(FALSE);
        m_comboSelection.EnableWindow(FALSE);
        m_editSelectionTitle.EnableWindow(FALSE);
        m_editSelectionDescription.EnableWindow(FALSE);
        m_editSelectionIcon.EnableWindow(FALSE);
        m_editSelectionCost.EnableWindow(FALSE);
        m_buttonAddFeatRequirementSelection.EnableWindow(TRUE);
    }
}

void CEnhancementEditorDialog::OnClickedCheckClickie()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        int checked = m_buttonClickie.GetCheck();
        if (checked)
        {
            (*it).Set_Clickie();
        }
        else
        {
            (*it).Clear_Clickie();
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnKillfocusEditIcon()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        CString text;
        m_editIcon.GetWindowText(text);
        if (text != (*it).Icon().c_str())
        {
            (*it).Set_Icon((LPCTSTR)text);
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

std::list<EnhancementTreeItem>::iterator CEnhancementEditorDialog::FindItem(
        std::list<EnhancementTreeItem> & items,
        size_t x,
        size_t y)
{
    std::list<EnhancementTreeItem>::iterator it = items.begin();
    bool found = false;
    while (!found && it != items.end())
    {
        if ((*it).XPosition() == x
                && (*it).YPosition() == y)
        {
            found = true;
            break;
        }
        ++it;
    }
    return it;
}

void CEnhancementEditorDialog::OnClickedButtonAddFeatRequirement()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        CString name;
        m_editFeatName.GetWindowText(name);
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        // add a requirement to this item
        Requirements requirements = (*it).RequirementsToTrain();
        std::list<Requirement> reqs = requirements.Requires();
        Requirement r;
        r.Set_Feat((LPCSTR)name);
        reqs.push_back(r);
        requirements.Set_Requires(reqs);
        (*it).Set_RequirementsToTrain(requirements);
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::PopulateSelection()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        ASSERT((*it).HasSelections());

        size_t index = m_comboSelection.GetCurSel();
        if (index >= 0)
        {
            std::list<EnhancementSelection> selections = (*it).Selections().Selections();
            std::list<EnhancementSelection>::iterator sit = selections.begin();
            std::advance(sit, index);
            m_editSelectionTitle.SetWindowText((*sit).Name().c_str());
            m_editSelectionDescription.SetWindowText((*sit).Description().c_str());
            m_editSelectionIcon.SetWindowText((*sit).Icon().c_str());
            m_editFeatSelection.SetWindowText("");
            CString text;
            text.Format("%d", (*sit).Cost());
            m_editSelectionCost.SetWindowText(text);
        }
    }
}

void CEnhancementEditorDialog::OnSelendokComboNumSelections()
{
     // number of selections has changed
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item

        size_t count = m_comboNumSelections.GetCurSel();
        if (count > 0)
        {
            std::list<EnhancementSelection> selections;
            if ((*it).HasSelections())
            {
                // number has changed, get current and resize
                selections = (*it).Selections().Selections();
            }
            selections.resize(count);
            Selector lsel;
            lsel.Set_Selections(selections);
            (*it).Set_Selections(lsel);
        }
        else
        {
            (*it).Clear_Selections();
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
        SetupSelections();
    }
}

void CEnhancementEditorDialog::OnSelendokComboEditSelection()
{
    PopulateSelection();
}

void CEnhancementEditorDialog::OnKillfocusEditSelectionTitle()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        ASSERT((*it).HasSelections());

        bool changed = false;
        size_t index = m_comboSelection.GetCurSel();
        if (index >= 0)
        {
            CString text;
            m_editSelectionTitle.GetWindowText(text);
            std::list<EnhancementSelection> selections = (*it).Selections().Selections();
            std::list<EnhancementSelection>::iterator sit = selections.begin();
            std::advance(sit, index);
            if ((*sit).Name() != (LPCSTR)text)
            {
                (*sit).Set_Name((LPCTSTR)text);
                Selector lsel;
                lsel.Set_Selections(selections);
                (*it).Set_Selections(lsel);
                changed = true;
            }
        }
        if (changed)
        {
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnKillfocusEditSelectionDescription()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        ASSERT((*it).HasSelections());

        bool changed = false;
        size_t index = m_comboSelection.GetCurSel();
        if (index >= 0)
        {
            CString text;
            m_editSelectionDescription.GetWindowText(text);
            std::list<EnhancementSelection> selections = (*it).Selections().Selections();
            std::list<EnhancementSelection>::iterator sit = selections.begin();
            std::advance(sit, index);
            if ((*sit).Description() != (LPCSTR)text)
            {
                (*sit).Set_Description((LPCTSTR)text);
                Selector lsel;
                lsel.Set_Selections(selections);
                (*it).Set_Selections(lsel);
                changed = true;
            }
        }
        if (changed)
        {
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnKillfocusEditIcon2()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        ASSERT((*it).HasSelections());

        bool changed = false;
        size_t index = m_comboSelection.GetCurSel();
        if (index >= 0)
        {
            CString text;
            m_editSelectionIcon.GetWindowText(text);
            std::list<EnhancementSelection> selections = (*it).Selections().Selections();
            std::list<EnhancementSelection>::iterator sit = selections.begin();
            std::advance(sit, index);
            if ((*sit).Icon() != (LPCSTR)text)
            {
                (*sit).Set_Icon((LPCTSTR)text);
                Selector lsel;
                lsel.Set_Selections(selections);
                (*it).Set_Selections(lsel);
                changed = true;
            }
        }
        if (changed)
        {
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnKillfocusEditCost2()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        ASSERT((*it).HasSelections());

        bool changed = false;
        size_t index = m_comboSelection.GetCurSel();
        if (index >= 0)
        {
            CString text;
            m_editSelectionCost.GetWindowText(text);
            std::list<EnhancementSelection> selections = (*it).Selections().Selections();
            std::list<EnhancementSelection>::iterator sit = selections.begin();
            std::advance(sit, index);
            if ((*sit).Cost() != atoi(text))
            {
                (*sit).Set_Cost(atoi(text));
                Selector lsel;
                lsel.Set_Selections(selections);
                (*it).Set_Selections(lsel);
                changed = true;
            }
        }
        if (changed)
        {
            m_loadedTrees.front().Set_Items(items);
            UpdateTree();
        }
    }
}

void CEnhancementEditorDialog::OnClickedButtonAddFeatRequirementSelection()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        ASSERT((*it).HasSelections());

        size_t index = m_comboSelection.GetCurSel();
        if (index >= 0)
        {
            CString text;
            m_editFeatSelection.GetWindowText(text);
            std::list<EnhancementSelection> selections = (*it).Selections().Selections();
            std::list<EnhancementSelection>::iterator sit = selections.begin();
            std::advance(sit, index);
            // add a requirement to this item
            Requirements requirements;
            if ((*sit).HasRequirementsToTrain())
            {
                requirements = (*sit).RequirementsToTrain();
            }
            std::list<Requirement> reqs = requirements.Requires();
            Requirement r;
            r.Set_Feat((LPCTSTR)text);
            reqs.push_back(r);
            requirements.Set_Requires(reqs);
            (*sit).Set_RequirementsToTrain(requirements);
            Selector lsel;
            lsel.Set_Selections(selections);
            (*it).Set_Selections(lsel);
        }
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnClickedButtonAddClassrequirement()
{
    int sel = m_comboList.GetCurSel();
    if (sel >= 0)
    {
        CString name;
        m_editClass.GetWindowText(name);
        CString text;
        m_editClassLevel.GetWindowText(text);
        std::list<EnhancementTreeItem> items = m_loadedTrees.front().Items();
        std::list<EnhancementTreeItem>::iterator it = items.begin();
        std::advance(it, sel);  // get correct item
        // add a requirement to this item
        Requirements requirements = (*it).RequirementsToTrain();
        std::list<Requirement> reqs = requirements.Requires();
        Requirement r;
        r.Set_Class(TextToEnumEntry((LPCTSTR)name, classTypeMap));
        r.Set_Level(atoi(text));
        reqs.push_back(r);
        requirements.Set_Requires(reqs);
        (*it).Set_RequirementsToTrain(requirements);
        m_loadedTrees.front().Set_Items(items);
        UpdateTree();
    }
}

void CEnhancementEditorDialog::OnClickedButtonicon1Transparent()
{
    CString text;
    m_editIcon.GetWindowText(text);
    MakeIconTransparent(text);
}

void CEnhancementEditorDialog::OnClickedButtonicon2Transparent()
{
    CString text;
    m_editSelectionIcon.GetWindowText(text);
    MakeIconTransparent(text);
}

void CEnhancementEditorDialog::MakeIconTransparent(CString name)
{
    // load all the images for the skills and add to the image list
    char fullPath[MAX_PATH];
    ::GetModuleFileName(
            NULL,
            fullPath,
            MAX_PATH);

    char drive[_MAX_DRIVE];
    char folder[_MAX_PATH];
    _splitpath_s(fullPath,
            drive, _MAX_DRIVE,
            folder, _MAX_PATH,
            NULL, 0,        // filename
            NULL, 0);       // extension

    char path[_MAX_PATH];
    _makepath_s(path, _MAX_PATH, drive, folder, NULL, NULL);

    std::string location = path;
    location += "EnhancementImages\\";
    std::string filename = location;
    filename += name;
    filename += ".png";

    CImage image;
    HRESULT result = image.Load(filename.c_str());
    if (result == S_OK)
    {
        // set the pixel background
        for (size_t y = 0; y < 9; y++)
        {
            for (size_t x = 0; x < (9 - y); x++)
            {
                image.SetPixel(x, y, c_transparentColour);
            }
            for (size_t x = 0; x < (9 - y); x++)
            {
                image.SetPixel(x, 31 - y, c_transparentColour);
            }
            for (size_t x = 0; x < (9 - y); x++)
            {
                image.SetPixel(31 - x, y, c_transparentColour);
            }
            for (size_t x = 0; x < (9 - y); x++)
            {
                image.SetPixel(31 - x, 31 - y, c_transparentColour);
            }
        }
        image.Save(filename.c_str(), Gdiplus::ImageFormatPNG);
        UpdateTree();
    }
    else
    {
        AfxMessageBox("Failed, image failed to load", MB_ICONERROR);
    }
}
