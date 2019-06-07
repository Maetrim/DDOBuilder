// EpicDestiniesView.cpp
//
#include "stdafx.h"
#include "EpicDestiniesView.h"

#include "DDOCP.h"
#include "DestinyTreeDialog.h"
#include "TwistOfFateDialog.h"
#include "GlobalSupportFunctions.h"
#include "MouseHook.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    // enhancement window size
    const size_t c_sizeX = 300;
    const size_t c_sizeY = 526;
    COLORREF f_fatePointsOverspendColour = RGB(0xE9, 0x96, 0x7A); // dark salmon
}

IMPLEMENT_DYNCREATE(CEpicDestiniesView, CFormView)

CEpicDestiniesView::CEpicDestiniesView() :
    CFormView(CEpicDestiniesView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_showingTip(false),
    m_tipCreated(false)
{

}

CEpicDestiniesView::~CEpicDestiniesView()
{
    for (size_t vi = 0; vi < m_treeViews.size(); ++vi)
    {
        delete m_treeViews[vi];
    }
    for (size_t vi = 0; vi < m_twistsOfFate.size(); ++vi)
    {
        delete m_twistsOfFate[vi];
    }
}

void CEpicDestiniesView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_DESTINY_SELECT, m_comboEpicDestinySelect);
    DDX_Control(pDX, IDC_CHECK_MAKE_ACTIVE_DESTINY, m_buttonMakeActive);
    DDX_Control(pDX, IDC_TWISTS_OF_FATE_LABEL, m_labelTwistsOfFate);
    DDX_Control(pDX, IDC_FATE_POINTS, m_fatePointsSpent);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CEpicDestiniesView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_CBN_SELENDOK(IDC_COMBO_DESTINY_SELECT, OnDestinySelect)
    ON_BN_CLICKED(IDC_CHECK_MAKE_ACTIVE_DESTINY, OnButtonMakeActiveDestiny)
    ON_WM_CTLCOLOR()
    ON_MESSAGE(WM_MOUSEENTER, OnMouseEnter)
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CEpicDestiniesView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEpicDestiniesView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CEpicDestiniesView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    m_tooltip.Create(this);
    m_tipCreated = true;

    // create the twists of fate sub views
    CRect rctTwistsLabel;
    m_labelTwistsOfFate.GetWindowRect(&rctTwistsLabel);
    ScreenToClient(&rctTwistsLabel);
    CRect itemRect(
            rctTwistsLabel.left,
            rctTwistsLabel.bottom + c_controlSpacing,
            rctTwistsLabel.left + rctTwistsLabel.Width(),
            rctTwistsLabel.bottom + rctTwistsLabel.Height() * 4);
    for (size_t twist = 0; twist < MAX_TWISTS; ++twist)
    {
        // show a feat selection dialog
        CTwistOfFateDialog * dlg = new CTwistOfFateDialog(this, twist);
        dlg->Create(CTwistOfFateDialog::IDD, this);
        dlg->MoveWindow(&itemRect);
        dlg->ShowWindow(SW_HIDE);   // start hidden
        m_twistsOfFate.push_back(dlg);
        m_hookTwistHandles[twist] = GetMouseHook()->AddRectangleToMonitor(
                this->GetSafeHwnd(),
                itemRect,           // screen coordinates,
                WM_MOUSEENTER,
                WM_MOUSELEAVE,
                false);
        // move rectangle down for next twist of fate
        itemRect += CPoint(0, itemRect.Height() + c_controlSpacing);
    }
    MoveFatePointControls();
    PopulateCombobox();
}

void CEpicDestiniesView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);
    if (m_treeViews.size() > 0
            && IsWindow(m_treeViews[0]->GetSafeHwnd()))
    {
        int scrollX = GetScrollPos(SB_HORZ);
        int scrollY = GetScrollPos(SB_VERT);

        CRect rctCombo;
        m_comboEpicDestinySelect.GetWindowRect(&rctCombo);
        rctCombo -= rctCombo.TopLeft();
        rctCombo += CPoint(c_controlSpacing, c_controlSpacing);
        rctCombo.right = rctCombo.left + c_sizeX;
        m_comboEpicDestinySelect.MoveWindow(rctCombo, TRUE);
        rctCombo -= CPoint(scrollX, scrollY);

        CRect rctMakeActive;
        CRect rctLabel;
        m_buttonMakeActive.GetWindowRect(&rctMakeActive);
        m_labelTwistsOfFate.GetWindowRect(&rctLabel);
        ScreenToClient(&rctMakeActive);
        ScreenToClient(&rctLabel);
        rctMakeActive -= rctMakeActive.TopLeft();
        rctMakeActive += CPoint(rctCombo.right + c_controlSpacing, rctCombo.top);
        rctLabel -= rctLabel.TopLeft();
        rctLabel += CPoint(rctCombo.right + c_controlSpacing, rctMakeActive.bottom + c_controlSpacing);
        m_buttonMakeActive.MoveWindow(rctMakeActive, TRUE);
        m_labelTwistsOfFate.MoveWindow(rctLabel, TRUE);

        // we only show the selected destiny tree
        int requiredWidth = c_sizeX         // window
                + (c_controlSpacing * 2);   // spacers

        // location of destiny tree just below the combo box
        CRect rctButton;
        m_buttonMakeActive.GetWindowRect(rctButton);
        ScreenToClient(rctButton);
        CRect itemRect(
                c_controlSpacing - scrollX,
                rctButton.bottom + c_controlSpacing,
                c_sizeX + c_controlSpacing - scrollX,
                rctButton.bottom + c_sizeY + c_controlSpacing);

        for (size_t ti = 0; ti < m_treeViews.size(); ++ti)
        {
            // move all the destiny windows to the same location
            // only the selected tree is visible
            m_treeViews[ti]->MoveWindow(itemRect, false);
        }
        // fate point controls move depending on number of available twists
        CSize twistBottom = MoveFatePointControls();
        // set scale based on area used by the windows.
        // This will introduce scroll bars if required
        int maxx = twistBottom.cx + c_controlSpacing + scrollX;
        int maxy = max(itemRect.bottom, twistBottom.cy) + c_controlSpacing + scrollY;
        SetScrollSizes(MM_TEXT, CSize(maxx, maxy));
        UpdateMouseHooks();
    }
}

void CEpicDestiniesView::UpdateMouseHooks()
{
    CRect rect;
    GetWindowRect(&rect);
    int cx = rect.Width();
    // limit to visible area
    for (size_t i = 0; i < MAX_TWISTS; ++i)
    {
        CRect rect;
        m_twistsOfFate[i]->GetWindowRect(&rect);
        ScreenToClient(&rect);
        if (rect.right > cx)
        {
            rect.right = cx;
        }
        ClientToScreen(&rect);
        GetMouseHook()->UpdateRectangle(
                m_hookTwistHandles[i],
                rect);          // screen coordinates,
    }
}

CSize CEpicDestiniesView::MoveFatePointControls()
{
    CSize bottomRight;
    // by default there are 4 twists of fate, unless the character has
    // the epic completionist. Then they have 5
    size_t twistIndex = 3;      // 0 based
    if (m_pCharacter != NULL)
    {
        if (m_pCharacter->HasEpicCompletionist())
        {
            twistIndex = 4;
        }
    }
    int scrollX = GetScrollPos(SB_HORZ);
    int scrollY = GetScrollPos(SB_VERT);
    CRect rctLabel;
    m_labelTwistsOfFate.GetWindowRect(rctLabel);
    ScreenToClient(&rctLabel);
    // controls to be moved are: (all maintain current dimensions)
    // [CStatic m_fatePointsSpent           ]
    // [CStatic Twists of Fate]
    CRect rctFatePointsSpent;
    m_fatePointsSpent.GetWindowRect(rctFatePointsSpent);
    rctFatePointsSpent -= rctFatePointsSpent.TopLeft();
    rctFatePointsSpent += CPoint(rctLabel.left, rctLabel.bottom + c_controlSpacing);
    m_fatePointsSpent.MoveWindow(rctFatePointsSpent);
    CPoint offset(rctFatePointsSpent.left, rctFatePointsSpent.bottom + c_controlSpacing);
    // move move all the windows
    for (size_t ti = 0; ti <= twistIndex; ++ti)
    {
        CRect rctTwist;
        m_twistsOfFate[ti]->GetWindowRect(rctTwist);
        rctTwist -= rctTwist.TopLeft();
        rctTwist += offset;
        m_twistsOfFate[ti]->MoveWindow(rctTwist);
        bottomRight.cx = rctTwist.right + c_controlSpacing;
        bottomRight.cy = rctTwist.bottom + c_controlSpacing;
        offset += CPoint(0, rctTwist.Height() + c_controlSpacing);
    }
    return bottomRight;
}

LRESULT CEpicDestiniesView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }

    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
        // trees definitely change if the character has changed
        m_availableEpicDestinies = DetermineTrees();
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
        PopulateCombobox();
    }
    else
    {
        DestroyEnhancementWindows();
    }
    // keep sub windows up to date
    for (size_t vi = 0; vi < m_twistsOfFate.size(); ++vi)
    {
        m_twistsOfFate[vi]->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)m_pDocument, (LPARAM)pCharacter);
    }
    EnableControls();
    return 0L;
}

BOOL CEpicDestiniesView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        0 // end marker
    };

    // exclude any visible enhancement view windows
    for (size_t i = 0; i < m_treeViews.size(); ++i)
    {
        if (::IsWindow(m_treeViews[i]->GetSafeHwnd())
                && ::IsWindowVisible(m_treeViews[i]->GetSafeHwnd()))
        {
            CRect rctWnd;
            m_treeViews[i]->GetWindowRect(&rctWnd);
            ScreenToClient(&rctWnd);
            pDC->ExcludeClipRect(&rctWnd);
        }
    }

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

std::vector<EnhancementTree> CEpicDestiniesView::DetermineTrees()
{
    std::vector<EnhancementTree> trees;
    // just find all the available epic destiny trees
    if (m_pCharacter != NULL)
    {
        const std::list<EnhancementTree> & allTrees = EnhancementTrees();
        std::list<EnhancementTree>::const_iterator it = allTrees.begin();
        while (it != allTrees.end())
        {
            if ((*it).HasIsEpicDestiny())
            {
                // yes this is one of our tree's add it
                trees.push_back((*it));
            }
            ++it;
        }
    }
    return trees;
}

void CEpicDestiniesView::CreateEnhancementWindows()
{
    LockWindowUpdate();
    // all epic destiny windows are a set size
    // and are at the same location (only selected visible)
    CRect itemRect(
            c_controlSpacing,
            c_controlSpacing,
            c_sizeX + c_controlSpacing,
            c_sizeY + c_controlSpacing);
    // show the trees selected by the user
    for (size_t i = 0; i < m_availableEpicDestinies.size(); ++i)
    {
        std::string treeName = m_availableEpicDestinies[i].Name();
        // create the tree dialog
        // show an enhancement dialog
        CDestinyTreeDialog * dlg = new CDestinyTreeDialog(
                this,
                m_pCharacter,
                GetEnhancementTree(treeName),
                TT_epicDestiny);
        dlg->Create(CDestinyTreeDialog::IDD, this);
        dlg->MoveWindow(&itemRect);
        dlg->ShowWindow(SW_HIDE);       // created hidden, only one visible at a time
        m_treeViews.push_back(dlg);
    }
    UnlockWindowUpdate();
    // reposition and show the windows (handled in OnSize)
    CRect rctWnd;
    GetClientRect(&rctWnd);
    OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
}

void CEpicDestiniesView::DestroyEnhancementWindows()
{
    LockWindowUpdate();
    for (size_t i = 0; i < m_treeViews.size(); ++i)
    {
        m_treeViews[i]->DestroyWindow();
        delete m_treeViews[i];
        m_treeViews[i] = NULL;
    }
    m_treeViews.clear();
    UnlockWindowUpdate();
}

void CEpicDestiniesView::PopulateCombobox()
{
    m_comboEpicDestinySelect.LockWindowUpdate();
    m_comboEpicDestinySelect.ResetContent();        // ensure empty
    // add each of the available trees in order, item data is the index
    // into the destiny vector
    for (size_t i = 0; i < m_availableEpicDestinies.size(); ++i)
    {
        CString treeName = m_availableEpicDestinies[i].Name().c_str();
        int index = m_comboEpicDestinySelect.AddString(treeName);
        m_comboEpicDestinySelect.SetItemData(index, i);
    }
    // default to the active destiny if there is one
    std::string activeDestiny;
    if (m_pCharacter != NULL)
    {
        activeDestiny = m_pCharacter->ActiveEpicDestiny();
    }
    size_t activeIndex = 0;
    if (activeDestiny != "")
    {
        // there is a selection, find its index
        for (size_t i = 0; i < m_availableEpicDestinies.size(); i++)
        {
            int index = m_comboEpicDestinySelect.GetItemData(i);
            ASSERT(index >= 0 && index < (int)m_availableEpicDestinies.size());
            if (m_availableEpicDestinies[index].Name() == activeDestiny)
            {
                // this is the one we want to select
                activeIndex = i;
                break;
            }
        }
    }
    m_comboEpicDestinySelect.SetCurSel(activeIndex);  // select the first tree by default
    for (size_t i = 0; i < m_treeViews.size(); ++i)
    {
        // only show the selected tree
        m_treeViews[i]->ShowWindow(i == activeIndex ? SW_SHOW : SW_HIDE);
    }
    m_comboEpicDestinySelect.UnlockWindowUpdate();
}

void CEpicDestiniesView::EnableControls()
{
    if (m_pCharacter != NULL)
    {
        m_comboEpicDestinySelect.EnableWindow(TRUE);
        m_buttonMakeActive.EnableWindow(TRUE);
        std::string destinyName;
        int sel = m_comboEpicDestinySelect.GetCurSel();
        if (sel != CB_ERR)
        {
            sel = m_comboEpicDestinySelect.GetItemData(sel);
            destinyName = m_availableEpicDestinies[sel].Name();
        }
        m_buttonMakeActive.SetCheck((m_pCharacter->ActiveEpicDestiny() == destinyName)
                ? BST_CHECKED
                : BST_UNCHECKED);
        // show available twists of fate
        for (size_t vi = 0; vi < m_twistsOfFate.size(); ++vi)
        {
            if (vi == MAX_TWISTS - 1)
            {
                // must have epic completionist to see this twist

                m_twistsOfFate[vi]->ShowWindow(
                        m_pCharacter->HasEpicCompletionist()
                                ? SW_SHOW
                                : SW_HIDE);
            }
            else
            {
                m_twistsOfFate[vi]->ShowWindow(SW_SHOW);
            }
            m_twistsOfFate[vi]->EnableWindow(m_pCharacter->IsTwistActive(vi));
        }
        // show how many fate points are available
        int totalFatePoints = m_pCharacter->FatePoints();
        size_t spentFatePoints = m_pCharacter->SpentFatePoints();
        CString text;
        text.Format("Fate Points: %d of %d",
                (totalFatePoints - (int)spentFatePoints),
                totalFatePoints);
        m_fatePointsSpent.SetWindowText(text);
    }
    else
    {
        // UI controls disabled if no document
        m_comboEpicDestinySelect.EnableWindow(FALSE);
        m_buttonMakeActive.EnableWindow(FALSE);
        for (size_t vi = 0; vi < m_twistsOfFate.size(); ++vi)
        {
            m_twistsOfFate[vi]->ShowWindow(SW_HIDE);
        }
    }
}

void CEpicDestiniesView::OnDestinySelect()
{
    int sel = m_comboEpicDestinySelect.GetCurSel();
    if (sel != CB_ERR)
    {
        sel = m_comboEpicDestinySelect.GetItemData(sel);
        ASSERT(sel < (int)m_availableEpicDestinies.size());
        for (size_t i = 0; i < m_treeViews.size(); ++i)
        {
            // only show the selected tree
            m_treeViews[i]->ShowWindow(i == sel ? SW_SHOW : SW_HIDE);
        }
        EnableControls();
    }
}

void CEpicDestiniesView::OnButtonMakeActiveDestiny()
{
    bool checked = (m_buttonMakeActive.GetCheck() == BST_CHECKED);
    if (checked)
    {
        // make the displayed destiny the active one
        int sel = m_comboEpicDestinySelect.GetCurSel();
        if (sel != CB_ERR)
        {
            sel = m_comboEpicDestinySelect.GetItemData(sel);
            ASSERT(sel < (int)m_availableEpicDestinies.size());
            m_pCharacter->EpicDestiny_SetActiveDestiny(m_availableEpicDestinies[sel].Name());
        }
    }
    else
    {
        // clear the active destiny
        m_pCharacter->EpicDestiny_SetActiveDestiny("");
    }
    EnableControls();
}

// CharacterObserver overrides
void CEpicDestiniesView::UpdateEnhancementTrained(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    EnableControls();
}

void CEpicDestiniesView::UpdateEnhancementRevoked(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    EnableControls();
}

void CEpicDestiniesView::UpdateEnhancementTreeReset(
        Character * charData)
{
    EnableControls();
}

void CEpicDestiniesView::UpdateFatePointsChanged(Character * charData)
{
    EnableControls();
}

void CEpicDestiniesView::UpdateEpicCompletionistChanged(Character * charData)
{
    EnableControls();
    MoveFatePointControls();
}

HBRUSH CEpicDestiniesView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
    // colour the control based on whether the user has over spent
    // the number of fate points available. This can happen
    // if the number of available fate points changes.
    bool setWarning = false;
    if (m_pCharacter != NULL)
    {
        if (pWnd == &m_fatePointsSpent)
        {
            int totalFatePoints = m_pCharacter->FatePoints();
            size_t spentFatePoints = m_pCharacter->SpentFatePoints();
            setWarning = ((int)spentFatePoints > totalFatePoints);
        }
    }
    if (setWarning)
    {
        pDC->SetTextColor(f_fatePointsOverspendColour);
    }

    return hbr;
}

LRESULT CEpicDestiniesView::OnMouseEnter(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        // we are over a twist, show a tooltip if its visible and trained
        for (size_t i = 0; i < MAX_TWISTS; ++i)
        {
            if (wParam == m_hookTwistHandles[i]
                    && m_twistsOfFate[i]->IsWindowVisible())
            {
                const TrainedEnhancement * te = m_pCharacter->TrainedTwist(i);
                if (te != NULL)
                {
                    const EnhancementTreeItem * item = FindEnhancement(te->EnhancementName());
                    if (item != NULL)
                    {
                        CRect itemRect;
                        m_twistsOfFate[i]->GetWindowRect(&itemRect);
                        ShowTip(*item, itemRect);
                    }
                }
                break;
            }
        }
    }
    return 0;
}

LRESULT CEpicDestiniesView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leave the area its being shown for
    HideTip();
    return 0;
}

void CEpicDestiniesView::ShowTip(const EnhancementTreeItem & item, CRect itemRect)
{
    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
    CPoint tipAlternate(itemRect.left, itemRect.top - 2);
    SetTooltipText(item, tipTopLeft, tipAlternate);
    m_showingTip = true;
}

void CEpicDestiniesView::HideTip()
{
    // tip not shown if not over an assay
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
    }
}

void CEpicDestiniesView::SetTooltipText(
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
                &item,
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

void CEpicDestiniesView::OnHScroll(UINT p1, UINT p2, CScrollBar* p3)
{
    // call base class
    CFormView::OnHScroll(p1, p2, p3);
    UpdateMouseHooks();
}

void CEpicDestiniesView::OnVScroll(UINT p1, UINT p2, CScrollBar* p3)
{
    // call base class
    CFormView::OnVScroll(p1, p2, p3);
    UpdateMouseHooks();
}

