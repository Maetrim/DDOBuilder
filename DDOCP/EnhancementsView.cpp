// EnhancementsView.cpp
//
#include "stdafx.h"
#include "EnhancementsView.h"

#include "DDOCP.h"
#include "EnhancementTreeDialog.h"
#include "GlobalSupportFunctions.h"
#include "CustomDockablePane.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    const UINT UWM_UPDATE_TREES = ::RegisterWindowMessage(_T("UpdateTrees"));
    // enhancement window size
    const size_t c_sizeX = 300;
    const size_t c_sizeY = 466;
    const std::string c_noSelection = "No selection";
}

IMPLEMENT_DYNCREATE(CEnhancementsView, CFormView)

CEnhancementsView::CEnhancementsView() :
    CFormView(CEnhancementsView::IDD),
    m_pDocument(NULL),
    m_pCharacter(NULL),
    m_numUniversalButtons(0),
    m_tipCreated(false),
    m_pTooltipItem(NULL)
{
}

CEnhancementsView::~CEnhancementsView()
{
    for (size_t vi = 0; vi < m_treeViews.size(); ++vi)
    {
        delete m_treeViews[vi];
    }
}

void CEnhancementsView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    // no IDC_TREE_SELECT1 as that is the racial tree which cannot be swapped out
    DDX_Control(pDX, IDC_TREE_SELECT2, m_comboTreeSelect[0]);
    DDX_Control(pDX, IDC_TREE_SELECT3, m_comboTreeSelect[1]);
    DDX_Control(pDX, IDC_TREE_SELECT4, m_comboTreeSelect[2]);
    DDX_Control(pDX, IDC_TREE_SELECT5, m_comboTreeSelect[3]);
    DDX_Control(pDX, IDC_TREE_SELECT6, m_comboTreeSelect[4]);
    DDX_Control(pDX, IDC_TREE_SELECT7, m_comboTreeSelect[5]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE1, m_universalTrees[0]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE2, m_universalTrees[1]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE3, m_universalTrees[2]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE4, m_universalTrees[3]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE5, m_universalTrees[4]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE6, m_universalTrees[5]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE7, m_universalTrees[6]);
    DDX_Control(pDX, IDC_BUTTON_UNIVERSAL_TREE8, m_universalTrees[7]);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CEnhancementsView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_REGISTERED_MESSAGE(UWM_UPDATE_TREES, OnUpdateTrees)
    ON_CONTROL_RANGE(CBN_SELENDOK, IDC_TREE_SELECT2, IDC_TREE_SELECT7, OnTreeSelect)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_UNIVERSAL_TREE1, IDC_BUTTON_UNIVERSAL_TREE8, OnUniversalTree)
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CEnhancementsView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEnhancementsView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CEnhancementsView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    UpdateWindowTitle();
    AddCustomButtons();
    m_tooltip.Create(this);
    m_tipCreated = true;
}

void CEnhancementsView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (m_treeViews.size() > 0
            && IsWindow(m_treeViews[0]->GetSafeHwnd()))
    {
        // we can position and show all the visible enhancement windows
        // with scrolls bars as required

        // default location of first enhancement tree
        CRect itemRect(
                c_controlSpacing,
                c_controlSpacing,
                c_sizeX + c_controlSpacing,
                c_sizeY + c_controlSpacing);
        int scrollX = GetScrollPos(SB_HORZ);
        int scrollY = GetScrollPos(SB_VERT);
        itemRect -= CPoint(scrollX, scrollY);

        ASSERT(m_treeViews.size() == MST_Number);
        for (size_t ti = 0; ti < m_visibleTrees.size(); ++ti)
        {
            size_t index = m_visibleTrees[ti];
            ASSERT(index >= 0 && index < MST_Number);
            // move the window to the correct location
            m_treeViews[index]->MoveWindow(itemRect, false);
            m_treeViews[index]->ShowWindow(SW_SHOW);        // ensure visible
            if (ti > 0 && ti < MST_Number)
            {
                CRect rctCombo(itemRect.left, itemRect.bottom, itemRect.right, itemRect.bottom + 300);
                m_comboTreeSelect[ti-1].MoveWindow(rctCombo);
                m_comboTreeSelect[ti-1].ShowWindow(SW_SHOW);
            }
            // now move the rectangle to the next tree location
            itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
        }
        // position the universal tree buttons under the racial tree
        for (size_t i = 0; i < m_numUniversalButtons; ++i)
        {
            CRect rect;
            m_universalTrees[i].GetWindowRect(&rect);
            rect -= rect.TopLeft();
            rect.right = rect.left + 36;
            rect.bottom = rect.top + 36;
            rect += CPoint((rect.Width() + c_controlSpacing) * i + c_controlSpacing, itemRect.bottom + c_controlSpacing);
            m_universalTrees[i].MoveWindow(rect, true);
            m_universalTrees[i].ShowWindow(SW_SHOW);
        }
        SetScrollSizes(
                MM_TEXT,
                CSize(
                        itemRect.left + scrollX,
                        itemRect.bottom + 36 + scrollY + c_controlSpacing * 2));
    }
}

LRESULT CEnhancementsView::OnNewDocument(WPARAM wParam, LPARAM lParam)
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
        m_availableTrees = DetermineTrees();
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
    }
    else
    {
        DestroyEnhancementWindows();
    }
    EnableDisableComboboxes();
    UpdateWindowTitle();
    return 0L;
}

BOOL CEnhancementsView::OnEraseBkgnd(CDC* pDC)
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

std::list<EnhancementTree> CEnhancementsView::DetermineTrees()
{
    std::list<EnhancementTree> trees;
    // see which classes we have and then make our race and class trees available
    if (m_pCharacter != NULL)
    {
        // to determine which tree's we need to know how many class levels we have in each class
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
        const std::list<EnhancementTree> & allTrees = EnhancementTrees();
        std::list<EnhancementTree>::const_iterator it = allTrees.begin();
        while (it != allTrees.end())
        {
            // get all the trees that are compatible with the race/class setup
            if ((*it).MeetRequirements(*m_pCharacter)
                    && !(*it).HasIsReaperTree()         // no reaper trees in enhancements please!
                    && !(*it).HasIsEpicDestiny())       // no epic destiny trees in enhancements please!
            {
                // yes this is one of our tree's add it
                trees.push_back((*it));
            }
            ++it;
        }
        // first remove any trees present which are no longer compatible
        // we also revoke those trees AP spends if required
        SelectedEnhancementTrees selTrees = m_pCharacter->SelectedTrees(); // take a copy
        for (size_t ti = 0; ti < MST_Number; ++ti)
        {
            std::string treeName = selTrees.Tree(ti);
            if (!SelectedEnhancementTrees::IsNotSelected(treeName))
            {
                // we have a tree selected here, is it in the new list of trees available?
                bool found = false;
                std::list<EnhancementTree>::iterator tit = trees.begin();
                while (!found && tit != trees.end())
                {
                    if ((*tit).Name() == treeName)
                    {
                        // ok, it's still in the list
                        found = true;
                    }
                    ++tit;
                }
                if (!found)
                {
                    // the tree is no longer valid for this race/class setup
                    // revoke any points spent in it
                    if (m_pCharacter->APSpentInTree(treeName) > 0)
                    {
                        // no user confirmation for this as they have already changed
                        // the base requirement that included the tree. All
                        // APs spent in this tree have to be returned to the pool of
                        // those available.
                        m_pCharacter->Enhancement_ResetEnhancementTree(treeName);   // revokes any trained enhancements also
                    }
                    // now remove it from the selected list
                    selTrees.SetNotSelected(ti);
                }
            }
        }

        // now that we have the tree list, assign them to unused tree selections
        // if there are any left
        std::list<EnhancementTree>::iterator tit = trees.begin();
        while (tit != trees.end())
        {
            if (!selTrees.IsTreePresent((*tit).Name()))
            {
                // tree not present, see if it can be assigned
                for (size_t ti = 0; ti < MST_Number; ++ti)
                {
                    std::string treeName = selTrees.Tree(ti);
                    if (SelectedEnhancementTrees::IsNotSelected(treeName)
                            || ((ti == 0) && treeName == c_noSelection))    // special case for racial tree
                    {
                        // no assignment yet for this tree, assign this tree to it
                        selTrees.SetTree(ti, (*tit).Name());
                        break;  // done
                    }
                }
            }
            // try the next tree
            ++tit;
        }
        m_pCharacter->Enhancement_SetSelectedTrees(selTrees);
    }
    return trees;
}

void CEnhancementsView::CreateEnhancementWindows()
{
    LockWindowUpdate();
    // all enhancement windows are a set size and max of MST_Number available to the user
    CRect itemRect(
            c_controlSpacing,
            c_controlSpacing,
            c_sizeX + c_controlSpacing,
            c_sizeY + c_controlSpacing);
    // show the trees selected by the user
    const SelectedEnhancementTrees & selTrees = m_pCharacter->SelectedTrees(); // take a copy
    for (size_t i = 0; i < MST_Number; ++i)
    {
        std::string treeName = selTrees.Tree(i);
        if (!SelectedEnhancementTrees::IsNotSelected(treeName))
        {
            // we have a selected tree here
            if (i > 0)
            {
                // no combo box selector for racial tree
                PopulateTreeCombo(&m_comboTreeSelect[i-1], treeName);
            }
            // create the tree dialog
            TreeType tt = TT_racial;
            if (i > 0)  // first tree is always racial
            {
                tt = GetEnhancementTree(treeName).HasIsUniversalTree()
                        ? TT_universal
                        : TT_enhancement;
            }
            // show an enhancement dialog
            CEnhancementTreeDialog * dlg = new CEnhancementTreeDialog(
                    this,
                    m_pCharacter,
                    GetEnhancementTree(treeName),
                    tt);
            dlg->Create(CEnhancementTreeDialog::IDD, this);
            dlg->MoveWindow(&itemRect);
            dlg->ShowWindow(SW_SHOW);
            m_treeViews.push_back(dlg);
        }
        else
        {
            // show a blank tree and populate the selection combo
            PopulateTreeCombo(&m_comboTreeSelect[i-1], "");
            // create the blank tree dialog
            CEnhancementTreeDialog * dlg = new CEnhancementTreeDialog(
                    this,
                    m_pCharacter,
                    GetEnhancementTree(c_noSelection),
                    TT_enhancement);
            dlg->Create(CEnhancementTreeDialog::IDD, this);
            dlg->MoveWindow(&itemRect);
            dlg->ShowWindow(SW_SHOW);
            m_treeViews.push_back(dlg);
        }
        // TBD this is just so they all get shown for now
        m_visibleTrees.push_back(m_visibleTrees.size());
    }
    UnlockWindowUpdate();
    // reposition and show the windows (handled in OnSize)
    CRect rctWnd;
    GetClientRect(&rctWnd);
    OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
}

void CEnhancementsView::DestroyEnhancementWindows()
{
    LockWindowUpdate();
    // the user has changed either a race or class and the available enhancement trees
    // has changed
    for (size_t i = 0; i < m_treeViews.size(); ++i)
    {
        m_treeViews[i]->DestroyWindow();
        delete m_treeViews[i];
        m_treeViews[i] = NULL;
    }
    m_treeViews.clear();
    m_visibleTrees.clear();
    UnlockWindowUpdate();
}

void CEnhancementsView::PopulateTreeCombo(
        CComboBox * combo,
        const std::string & selectedTree)
{
    combo->LockWindowUpdate();
    combo->ResetContent();
    // always add a "No selection" item
    combo->AddString(c_noSelection.c_str());
    // now add any trees which are not already selected
    int sel = 0;        // assume "No selection"
    const SelectedEnhancementTrees & selTrees = m_pCharacter->SelectedTrees();
    std::list<EnhancementTree>::iterator tit = m_availableTrees.begin();
    while (tit != m_availableTrees.end())
    {
        if (!selTrees.IsTreePresent((*tit).Name())
                || selectedTree == (*tit).Name())       // include the name of the selected tree in the combo
        {
            // tree not present, list it in the control
            int index = combo->AddString((*tit).Name().c_str());
            if ((*tit).Name() == selectedTree)
            {
                sel = index;        // this is the item that should be selected
            }
        }
        // Move to the next tree
        ++tit;
    }
    // select the active tree
    combo->SetCurSel(sel);
    // disable the control is any points are spent in this tree
    if (m_pCharacter->APSpentInTree(selectedTree) > 0)
    {
        // can't change trees if any points spent in the tree
        combo->EnableWindow(FALSE);
    }
    else
    {
        // if no points spent or "No selection" then enable the control
        combo->EnableWindow(TRUE);
    }
    // done
    combo->UnlockWindowUpdate();
}

// CharacterObserver overrides
void CEnhancementsView::UpdateAlignmentChanged(Character * charData, AlignmentType alignmen)
{
    // if alignment has changed, then classes may have changed also
    // we need to update our windows
    // (Same trees may still be available)
    std::list<EnhancementTree> trees = DetermineTrees();
    if (trees != m_availableTrees)
    {
        // yup, they have changed
        m_availableTrees = trees;
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
    }
}

void CEnhancementsView::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    // if a class has changed so whether we need to update our windows
    // (Same trees may still be available)
    std::list<EnhancementTree> trees = DetermineTrees();
    if (trees != m_availableTrees)
    {
        // yup, they have changed
        m_availableTrees = trees;
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
    }
}

void CEnhancementsView::UpdateRaceChanged(
        Character * charData,
        RaceType race)
{
    // if the race has changed, we definitely need to update the available
    // enhancement trees
    m_availableTrees = DetermineTrees();
    DestroyEnhancementWindows();
    CreateEnhancementWindows();
}

void CEnhancementsView::UpdateFeatEffect(Character * charData, const std::string & featName,  const Effect & effect)
{
     if (effect.Type() == Effect_EnhancementTree)
     {
         // do a delayed tree update as we do not want a current tree object
         // to be destroyed while a const reference to it is being used during
         // the notify
        PostMessage(UWM_UPDATE_TREES, 0, 0L);
     }
}

void CEnhancementsView::UpdateFeatEffectRevoked(Character * charData, const std::string & featName, const Effect & effect)
{
     if (effect.Type() == Effect_EnhancementTree)
     {
         // do a delayed tree update as we do not want a current tree object
         // to be destroyed while a const reference to it is being used during
         // the notify
        PostMessage(UWM_UPDATE_TREES, 0, 0L);
     }
}

void CEnhancementsView::UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect)
{
     if (effect.m_effect.Type() == Effect_EnhancementTree)
     {
         // do a delayed tree update as we do not want a current tree object
         // to be destroyed while a const reference to it is being used during
         // the notify
        PostMessage(UWM_UPDATE_TREES, 0, 0L);
     }
}

void CEnhancementsView::UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect)
{
     if (effect.m_effect.Type() == Effect_EnhancementTree)
     {
         // do a delayed tree update as we do not want a current tree object
         // to be destroyed while a const reference to it is being used during
         // the notify
        PostMessage(UWM_UPDATE_TREES, 0, 0L);
     }
}

void CEnhancementsView::UpdateEnhancementTreeOrderChanged(Character * charData)
{
    DestroyEnhancementWindows();
    CreateEnhancementWindows();
}

LRESULT CEnhancementsView::OnUpdateTrees(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    // received a delayed tree update message, do the work
    UpdateTrees();
    UpdateWindowTitle();
    EnableDisableComboboxes();
    return 0;
}

void CEnhancementsView::UpdateTrees()
{
    // only update if there is a change in actual trees
    std::list<EnhancementTree> trees = DetermineTrees();
    if (trees != m_availableTrees)
    {
        // yup, they have changed
        m_availableTrees = trees;
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
    }
}

void CEnhancementsView::UpdateEnhancementTreeReset(Character * charData)
{
    UpdateWindowTitle();
    EnableDisableComboboxes();
}

void CEnhancementsView::UpdateActionPointsChanged(Character * charData)
{
    UpdateWindowTitle();
    EnableDisableComboboxes();
}

void CEnhancementsView::UpdateWindowTitle()
{
    if (m_pCharacter != NULL)
    {
        CString text;
        text.Format("Enhancements - %d points available to spend",
                m_pCharacter->AvailableActionPoints());
        size_t bonusRacial = m_pCharacter->BonusRacialActionPoints();
        if (bonusRacial > 0)
        {
            CString additional;
            additional.Format(", Racial %d", bonusRacial);
            text += additional;
        }
        size_t bonusUniversal = m_pCharacter->BonusUniversalActionPoints();
        if (bonusUniversal > 0)
        {
            CString additional;
            additional.Format(", Universal %d", bonusUniversal);
            text += additional;
        }
        GetParent()->SetWindowText(text);
    }
    else
    {
        GetParent()->SetWindowText("Enhancements");
    }
}

void CEnhancementsView::OnTreeSelect(UINT nID)
{
    // the user has selected a new enhancement tree from a drop list under one of
    // the enhancement view windows. Switch the selected tree to the one wanted
    size_t treeIndex = (nID - IDC_TREE_SELECT2) + 1;    // tree 0 is the racial tree and cant be changed (no control for it)
    ASSERT(treeIndex > 0 && treeIndex < MST_Number);
    int sel = m_comboTreeSelect[treeIndex-1].GetCurSel();
    if (sel != CB_ERR)
    {
        CString entry;
        m_comboTreeSelect[treeIndex-1].GetLBText(sel, entry);
        std::string treeName = (LPCSTR)entry;
        // select the tree wanted!
        SelectedEnhancementTrees selTrees = m_pCharacter->SelectedTrees(); // take a copy
        selTrees.SetTree(treeIndex, treeName);      // modify
        m_pCharacter->Enhancement_SetSelectedTrees(selTrees);   // update
        // update our state
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
    }
}

void CEnhancementsView::EnableDisableComboboxes()
{
    if (m_pCharacter != NULL)
    {
        const SelectedEnhancementTrees & selTrees = m_pCharacter->SelectedTrees();
        for (size_t i = 1; i < MST_Number; ++i)
        {
            std::string treeName = selTrees.Tree(i);
            // can only select a different tree if no points spent in this one
            bool enable = (m_pCharacter->APSpentInTree(treeName) == 0);
            m_comboTreeSelect[i-1].EnableWindow(enable);
        }
        // also set the states of the universal tree buttons
        for (size_t i = 0; i < m_numUniversalButtons; ++i)
        {
            m_universalTrees[i].EnableWindow(true);
            m_universalTrees[i].ShowWindow(SW_SHOW);
            // show the correct checked state
            CString name;
            m_universalTrees[i].GetWindowText(name);
            bool trained = (m_pCharacter->GetSpecialFeatTrainedCount((LPCTSTR)name) > 0);
            m_universalTrees[i].SetSelected(trained);
        }
    }
    else
    {
        // all combo boxes should be hidden and disabled
        for (size_t i = 1; i < MST_Number; ++i)
        {
            m_comboTreeSelect[i-1].EnableWindow(false);
            m_comboTreeSelect[i-1].ShowWindow(SW_HIDE);
        }
        // also set the states of the universal tree buttons
        for (size_t i = 0; i < m_numUniversalButtons; ++i)
        {
            m_universalTrees[i].EnableWindow(false);
            m_universalTrees[i].ShowWindow(SW_HIDE);
        }
    }

}

void CEnhancementsView::AddCustomButtons()
{
    // add buttons under the Racial tree to allow quick access to the
    // universal enhancement trees
    CWinApp * pApp = AfxGetApp();
    CDDOCPApp * pDDOApp = dynamic_cast<CDDOCPApp*>(pApp);
    if (pDDOApp != NULL)
    {
        const std::list<Feat> & universalTreeFeats = pDDOApp->UniversalTreeFeats();
        std::list<Feat>::const_iterator it = universalTreeFeats.begin();
        size_t buttonIndex = 0;
        while (it != universalTreeFeats.end())
        {
            m_universalTrees[buttonIndex].SetWindowText((*it).Name().c_str());
            CImage image;
            HRESULT result = LoadImageFile(
                    IT_feat,
                    (*it).Icon(),
                    &m_universalTrees[buttonIndex].m_image,
                    false);
            m_universalTrees[buttonIndex].m_image.SetTransparentColor(c_transparentColour);
            ++buttonIndex;
            ++m_numUniversalButtons;
            ++it;
        }
    }
}

void CEnhancementsView::OnUniversalTree(UINT nID)
{
    UINT id = nID - IDC_BUTTON_UNIVERSAL_TREE1;
    CString name;
    m_universalTrees[id].GetWindowText(name);
    bool trained = (m_pCharacter->GetSpecialFeatTrainedCount((LPCTSTR)name) > 0);
    if (trained)
    {
        m_pCharacter->RevokeSpecialFeat((LPCTSTR)name, TFT_SpecialFeat);
    }
    else
    {
        m_pCharacter->TrainSpecialFeat((LPCTSTR)name, TFT_SpecialFeat);
    }
    m_universalTrees[id].SetSelected(!trained);
    // ensure displayed trees are shown
    std::list<EnhancementTree> trees = DetermineTrees();
    if (trees != m_availableTrees)
    {
        // yup, they have changed
        m_availableTrees = trees;
        DestroyEnhancementWindows();
        CreateEnhancementWindows();
    }
}

void CEnhancementsView::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which universal tree selector the mouse may be over
    CWnd * pWnd = ChildWindowFromPoint(point);
    CIconButton * pIconButton = dynamic_cast<CIconButton*>(pWnd);
    if (m_pCharacter == NULL)
    {
        // ensure buttons do not produce tooltips while no character active
        pIconButton = NULL;
    }
    if (pIconButton != NULL
            && pIconButton != m_pTooltipItem)
    {
        CRect itemRect;
        pIconButton->GetWindowRect(&itemRect);
        ScreenToClient(itemRect);
        // over a new item or a different item
        m_pTooltipItem = pIconButton;
        ShowTip(*pIconButton, itemRect);
    }
    else
    {
        if (m_showingTip
                && pIconButton != m_pTooltipItem)
        {
            // no longer over the same item
            HideTip();
        }
    }
}

void CEnhancementsView::ShowTip(const CIconButton & item, CRect itemRect)
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
    // track the mouse so we know when it leaves our window
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
}

void CEnhancementsView::HideTip()
{
    // tip not shown if not over an assay
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
        m_pTooltipItem = NULL;
    }
}

void CEnhancementsView::SetTooltipText(
        const CIconButton & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    CString text;
    item.GetWindowText(text);
    const Feat & feat = FindFeat((LPCTSTR)text);
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetFeatItem(*m_pCharacter, &feat, false, MAX_LEVEL, false);
    m_tooltip.Show();
}
