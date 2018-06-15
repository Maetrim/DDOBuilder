// TwistOfFateDialog.cpp
//

#include "stdafx.h"
#include "TwistOfFateDialog.h"
#include "DDOCPDoc.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"

const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CTwistOfFateDialog, CDialog)
    //{{AFX_MSG_MAP(CTwistOfFateDialog)
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_BN_CLICKED(IDC_MINUS, OnButtonMinus)
    ON_BN_CLICKED(IDC_PLUS, OnButtonPlus)
    ON_WM_ERASEBKGND()
    ON_CBN_SELENDOK(IDC_COMBO_TWISTSELECT, OnComboTwistSelect)
    ON_CBN_SELENDCANCEL(IDC_COMBO_TWISTSELECT, OnComboTwistCancel)
    ON_MESSAGE(WM_MOUSEHOVER, OnHoverComboBox)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma warning(pop)

CTwistOfFateDialog::CTwistOfFateDialog(CWnd* pParent, size_t twistIndex) :
    CDialog(CTwistOfFateDialog::IDD, pParent),
    m_twistIndex(twistIndex),
    m_pCharacter(NULL),
    m_bShowingTip(false),
    m_bIgnoreNextMessage(false)
{
    //{{AFX_DATA_INIT(CTwistOfFateDialog)
    //}}AFX_DATA_INIT
}

void CTwistOfFateDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTwistOfFateDialog)
    DDX_Control(pDX, IDC_MINUS, m_buttonMinus);
    DDX_Control(pDX, IDC_COMBO_TWISTSELECT, m_comboTwistSelect);
    DDX_Control(pDX, IDC_PLUS, m_buttonPlus);
    DDX_Control(pDX, IDC_TIER, m_staticTier);
    //}}AFX_DATA_MAP
}

BOOL CTwistOfFateDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_tooltip.Create(this);
    SetupControls();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CTwistOfFateDialog::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    //m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    if (pCharacter != m_pCharacter)
    {
        if (m_pCharacter != NULL)
        {
            m_pCharacter->DetachObserver(this);
        }
        m_pCharacter = pCharacter;
        if (m_pCharacter != NULL)
        {
            m_pCharacter->AttachObserver(this);
            PopulateTwistCombobox();
        }
        SetupControls();
    }
    return 0L;
}

void CTwistOfFateDialog::SetupControls()
{
    if (m_pCharacter != NULL)
    {
        if (m_pCharacter->IsTwistActive(m_twistIndex))
        {
            // can only decrement this twist as long as it is a higher tier
            // than any following twist
            bool canDecrement = true;
            if (m_twistIndex < MAX_TWISTS - 1)
            {
                // check the tier of the following twist
                canDecrement = m_pCharacter->TwistTier(m_twistIndex)
                        > m_pCharacter->TwistTier(m_twistIndex+1);
            }
            m_buttonMinus.EnableWindow(
                    m_pCharacter->TwistTier(m_twistIndex) > 0
                    && canDecrement);
            m_comboTwistSelect.EnableWindow(m_pCharacter->TwistTier(m_twistIndex) > 0);
            m_buttonPlus.EnableWindow(m_pCharacter->CanUpgradeTwist(m_twistIndex));
            // show current tier and next upgrade cost
            CString text;
            if (m_pCharacter->TwistTier(m_twistIndex) < MAX_TWIST_LEVEL)
            {
                size_t nextCost = 1 + m_twistIndex + m_pCharacter->TwistTier(m_twistIndex);
                text.Format("Tier %d (Cost %d)",
                        m_pCharacter->TwistTier(m_twistIndex),
                        nextCost);
            }
            else
            {
                // just show the current tier
                text.Format("Tier %d (MAX)", m_pCharacter->TwistTier(m_twistIndex));
            }
            m_staticTier.SetWindowText(text);
        }
        else
        {
            m_buttonMinus.EnableWindow(FALSE);
            m_comboTwistSelect.EnableWindow(FALSE);
            m_buttonPlus.EnableWindow(FALSE);
            m_staticTier.SetWindowText("");
        }
    }
    else
    {
        // controls disabled
        m_buttonMinus.EnableWindow(FALSE);
        m_comboTwistSelect.EnableWindow(FALSE);
        m_buttonPlus.EnableWindow(FALSE);
        m_staticTier.SetWindowText("");
    }
}

void CTwistOfFateDialog::PopulateTwistCombobox()
{
    // need to know the current twist so that we start with that selected
    int sel = 0;            // no selection entry

    // get the list of available twists that can be trained based on which
    // enhancements in which destinies have been trained.
    m_availableTwists = m_pCharacter->AvailableTwists(m_twistIndex);
    const TrainedEnhancement * trainedTwist = m_pCharacter->TrainedTwist(m_twistIndex);
    bool foundTwist = false;

    // add an option of no twist to the front of the list
    TrainedEnhancement noSelection;
    m_availableTwists.push_front(noSelection);

    // now populate the combo box to allow selection of the twist
    m_comboTwistSelect.LockWindowUpdate();
    m_comboTwistSelect.SetImageList(NULL);
    m_comboTwistSelect.ResetContent();

    m_twistImages.DeleteImageList();
    m_twistImages.Create(
            32,             // all icons are 32x32 pixels
            32,
            ILC_COLOR32,
            0,
            m_availableTwists.size());
    std::list<TrainedEnhancement>::const_iterator it = m_availableTwists.begin();
    std::list<const EnhancementTreeItem *> cachedPointers;
    size_t twistIndex = 0;
    while (it != m_availableTwists.end())
    {
        // note that "item" can be NULL for "no selection"
        const EnhancementTreeItem * item = FindEnhancement((*it).EnhancementName());
        cachedPointers.push_back(item); // save for later use
        CImage image;
        HRESULT result = LoadImageFile(
                IT_enhancement,
                item != NULL ? item->ActiveIcon(*m_pCharacter) : "NoTwist",
                &image);
        if (result == S_OK)
        {
            CBitmap bitmap;
            bitmap.Attach(image.Detach());
            m_twistImages.Add(&bitmap, c_transparentColour);  // standard mask color (purple)
        }
        // is this the selected twist?
        if (trainedTwist != NULL
                && trainedTwist->EnhancementName() == (*it).EnhancementName())
        {
            // this is the one that needs to be selected
            sel = twistIndex;
            foundTwist = true;
        }
        ++twistIndex;
        ++it;
    }
    m_comboTwistSelect.SetImageList(&m_twistImages);
    ASSERT(cachedPointers.size() == m_availableTwists.size());

    // see if the selected twist is still available
    if (trainedTwist != NULL && !foundTwist)
    {
        // if the twist is not available, remove it
        m_pCharacter->SetTwist(m_twistIndex, NULL);
        // let the user know
        AfxMessageBox("Selected twist no longer available. It was revoked.\n"
                "This can happen when:\n"
                "The twist came from a destiny which has just been made active,\n"
                "The twist in question was revoked from the destiny it is selected from.", MB_ICONWARNING);
    }

    // now add the twist names to the combo control
    twistIndex = 0;
    std::list<const EnhancementTreeItem *>::const_iterator pit = cachedPointers.begin();
    while (pit != cachedPointers.end())
    {
        // format of:
        // T?: <Name>
        // ? = Tier of selected twist
        char buffer[_MAX_PATH];
        if ((*pit) != NULL)
        {
            sprintf_s(
                    buffer,
                    _MAX_PATH,
                    "T%d: %s",
                    (*pit)->YPosition(),
                    (*pit)->Name().c_str());
        }
        else
        {
            strcpy_s(buffer, _MAX_PATH, "No Selection");
        }
        size_t pos = m_comboTwistSelect.AddString(buffer);
        m_comboTwistSelect.SetItemData(pos, twistIndex);
        ++twistIndex;
        ++pit;
    }
    m_comboTwistSelect.SetCurSel(sel);
    m_comboTwistSelect.SetDroppedWidth(350); // wider to show extra text
    m_comboTwistSelect.UnlockWindowUpdate();
}

BOOL CTwistOfFateDialog::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        IDC_MINUS,
        IDC_COMBO_TWISTSELECT,
        IDC_PLUS,
        IDC_TIER,
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CTwistOfFateDialog::UpdateFatePointsChanged(Character * charData)
{
    SetupControls();
}

void CTwistOfFateDialog::UpdateAvailableTwistsChanged(Character * charData)
{
    // update the combo box selections available
    PopulateTwistCombobox();
    SetupControls();
}

void CTwistOfFateDialog::OnButtonMinus()
{
    m_pCharacter->DowngradeTwist(m_twistIndex);
}

void CTwistOfFateDialog::OnButtonPlus()
{
    m_pCharacter->UpgradeTwist(m_twistIndex);
}

void CTwistOfFateDialog::OnComboTwistSelect()
{
    if (m_bShowingTip)
    {
        m_tooltip.Hide();
        m_bShowingTip = false;
    }
    // train the selected twist
    int sel = m_comboTwistSelect.GetCurSel();
    if (sel != CB_ERR)
    {
        ASSERT(sel >= 0 && sel < (int)m_availableTwists.size());
        std::list<TrainedEnhancement>::iterator it = m_availableTwists.begin();
        std::advance(it, sel);
        m_pCharacter->SetTwist(m_twistIndex, &(*it));
    }
}

LRESULT CTwistOfFateDialog::OnHoverComboBox(WPARAM wParam, LPARAM lParam)
{
    if (!m_bIgnoreNextMessage)
    {
        // wParam = selected index
        // lParam = control ID (only 1 control in this dialog)
        if (m_bShowingTip)
        {
            m_tooltip.Hide();
            m_bShowingTip = false;
        }
        if (wParam >= 0)
        {
            // we have a selection, get the twist item
            std::list<TrainedEnhancement>::iterator it = m_availableTwists.begin();
            std::advance(it, wParam);
            const EnhancementTreeItem * item = FindEnhancement(
                    (*it).EnhancementName());
            if (item != NULL)
            {
                CRect rctWindow;
                m_comboTwistSelect.GetWindowRect(&rctWindow);
                rctWindow.right = rctWindow.left + m_comboTwistSelect.GetDroppedWidth();
                ShowTip(*item, rctWindow);
                m_bShowingTip = true;
            }
        }
    }
    m_bIgnoreNextMessage = false;
    return 0;
}

void CTwistOfFateDialog::OnComboTwistCancel()
{
    // hide any tooltip being shown
    if (m_bShowingTip)
    {
        m_tooltip.Hide();
        m_bShowingTip = false;
    }
    m_bIgnoreNextMessage = true;
}

void CTwistOfFateDialog::ShowTip(const EnhancementTreeItem & item, CRect itemRect)
{
    if (m_bShowingTip)
    {
        m_tooltip.Hide();
    }
    CPoint tipTopLeft(itemRect.right, itemRect.bottom + 2);
    CPoint tipAlternate(itemRect.right, itemRect.top - 2);
    SetTooltipText(item, tipTopLeft, tipAlternate);
    m_bShowingTip = true;
}

void CTwistOfFateDialog::SetTooltipText(
        const EnhancementTreeItem & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    const TrainedEnhancement * te = m_pCharacter->IsTrained(item.InternalName(), "");
    const EnhancementSelection * es = NULL;
    std::string selection;
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    if (te != NULL)
    {
        // this item is trained, we may need to show the selected sub-item tooltip text
        if (te->HasSelection())
        {
            selection = te->Selection();
            const Selector & selector = item.Selections();
            const std::list<EnhancementSelection> & selections = selector.Selections();
            // find the selected item
            std::list<EnhancementSelection>::const_iterator it = selections.begin();
            while (it != selections.end())
            {
                if ((*it).Name() == te->Selection())
                {
                    es = &(*it);
                    break;
                }
                ++it;
            }
        }
    }
    if (es != NULL)
    {
        m_tooltip.SetEnhancementSelectionItem(
                *m_pCharacter,
                es,
                te->Ranks());
    }
    else
    {
        // its a top level item without sub-options
        std::string treeName;
        FindEnhancement(te->EnhancementName(), &treeName);
        m_tooltip.SetEnhancementTreeItem(
                *m_pCharacter,
                &item,
                selection,
                m_pCharacter->APSpentInTree(treeName));
    }
    m_tooltip.Show();
}

void CTwistOfFateDialog::OnCancel()
{
    // do nothing to stop dialog being dismissed
}

