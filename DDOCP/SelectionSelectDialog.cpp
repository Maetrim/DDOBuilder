// SelectionSelectDialog.cpp
//
#include "stdafx.h"
#include "SelectionSelectDialog.h"
#include "afxdialogex.h"
#include "Character.h"
#include "EnhancementTreeItem.h"
#include "GlobalSupportFunctions.h"


// CSelectionSelectDialog dialog

IMPLEMENT_DYNAMIC(CSelectionSelectDialog, CDialog)

CSelectionSelectDialog::CSelectionSelectDialog(
        CWnd* pParent,
        const Character & charData,
        const EnhancementTreeItem & item,
        const std::string & treeName,
        TreeType type) :
    CDialog(CSelectionSelectDialog::IDD, pParent),
    m_charData(charData),
    m_item(item),
    m_treeName(treeName),
    m_type(type),
    m_cost(0),
    m_showingTip(false),
    m_tipCreated(false),
    m_pTooltipItem(NULL)
{
}

CSelectionSelectDialog::~CSelectionSelectDialog()
{
}

void CSelectionSelectDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    for (size_t i = 0; i < c_maxSelections; ++i)
    {
        DDX_Control(pDX, IDC_OPTION1 + i, m_buttonOption[i]);
    }
}

BEGIN_MESSAGE_MAP(CSelectionSelectDialog, CDialog)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

std::string CSelectionSelectDialog::Selection() const
{
    return m_selection;
}

size_t CSelectionSelectDialog::Cost() const
{
    return m_cost;
}

// CSelectionSelectDialog message handlers
BOOL CSelectionSelectDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_tooltip.Create(this);
    m_tipCreated = true;

     // set the button icons and text and enable states
    const Selector & selector = m_item.Selections();
    const std::list<EnhancementSelection> & selections = selector.Selections();
    ASSERT(selections.size() <= c_maxSelections);
    std::list<EnhancementSelection>::const_iterator it = selections.begin();
    size_t index = 0;
    while (it != selections.end())
    {
        // ensure buttons are correct size
        CRect rctButton;
        m_buttonOption[index].GetWindowRect(&rctButton);
        ScreenToClient(&rctButton);
        rctButton.right = rctButton.left + 38;
        rctButton.bottom = rctButton.top + 38;
        m_buttonOption[index].MoveWindow(&rctButton);
        m_buttonOption[index].SetImage((*it).Icon());
        m_buttonOption[index].SetWindowText((*it).Name().c_str());
        m_costs[index] = (*it).Cost();
        m_selections[index] = (*it).Name();
        bool excluded = false;
        if (selector.HasExclusions())
        {
            const std::list<SelectorExclusion> & exclusions = selector.Exclusions().Exclude();
            // check all the exclusions
            std::list<SelectorExclusion>::const_iterator eit = exclusions.begin();
            while (eit != exclusions.end())
            {
                const TrainedEnhancement * te = m_charData.IsTrained((*eit).InternalName(), "");
                if (te != NULL)
                {
                    // this previous enhancement is trained, see what was selected
                    if (te->HasSelection()
                            && te->Selection() == (*it).Name())
                    {
                        // previously trained
                        excluded = true;
                    }
                }
                ++eit;
            }
        }
        bool canTrain = true;
        if ((*it).HasRequirementsToTrain())
        {
            std::vector<size_t> classLevels = m_charData.ClassLevels(MAX_LEVEL);
            std::list<TrainedFeat> trainedFeats = m_charData.CurrentFeats(MAX_LEVEL);
            canTrain = (*it).RequirementsToTrain().CanTrainFeat(
                    m_charData,
                    classLevels,
                    MAX_LEVEL,
                    trainedFeats);
        }
        bool enoughAP = (m_charData.AvailableActionPoints(m_treeName, m_type) >= (*it).Cost());
         m_buttonOption[index].EnableWindow(!excluded && canTrain && enoughAP);
        ++it;
        ++index;
    }
    // hide any unused selection buttons
    while (index < c_maxSelections)
    {
        m_buttonOption[index].EnableWindow(false);
        m_buttonOption[index].ShowWindow(SW_HIDE);
        ++index;
    }
    GetDlgItem(IDOK)->EnableWindow(FALSE);       // must select an item to click ok

    return TRUE;
}

void CSelectionSelectDialog::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which selection button the mouse may be over
    CRect itemRect;
    CWnd * pWnd = ChildWindowFromPoint(point);
    if (pWnd != NULL
            && pWnd != m_pTooltipItem)
    {
        if (pWnd->IsKindOf(RUNTIME_CLASS(CStatic))
                && pWnd->IsWindowVisible())
        {
            m_pTooltipItem = pWnd;
            size_t index = pWnd->GetDlgCtrlID() - IDC_OPTION1;
            pWnd->GetWindowRect(&itemRect);
            ShowTip(index, itemRect);
        }
    }
    else
    {
        if (m_showingTip
                && pWnd != m_pTooltipItem)
        {
            HideTip();
        }
    }
}

LRESULT CSelectionSelectDialog::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // we erroneously get a mouse leave due to the mouse being over a child window,
    // exclude the mouse leave for these
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);
    CWnd * pWnd = ChildWindowFromPoint(point);
    if (pWnd != m_pTooltipItem)
    {
        // hide any tooltip when the mouse leave the area its being shown for
        HideTip();
    }
    else
    {
        // restart another mouse leave as its a 1 time event
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = 1;
        _TrackMouseEvent(&tme);
    }
    return 0;
}

void CSelectionSelectDialog::ShowTip(size_t index, CRect itemRect)
{
    const Selector & selector = m_item.Selections();
    const std::list<EnhancementSelection> & selections = selector.Selections();
    ASSERT(selections.size() <= c_maxSelections);
    std::list<EnhancementSelection>::const_iterator it = selections.begin();
    std::advance(it, index);

    if (m_showingTip)
    {
        m_tooltip.Hide();
    }
    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
    SetTooltipText((*it), tipTopLeft);
    m_showingTip = true;
    // track the mouse so we know when it leaves our window
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
}

void CSelectionSelectDialog::HideTip()
{
    // tip not shown if not over an assay
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
        m_pTooltipItem = NULL;
    }
}

void CSelectionSelectDialog::SetTooltipText(
        const EnhancementSelection & item,
        CPoint tipTopLeft)
{
    m_tooltip.SetOrigin(tipTopLeft);
    m_tooltip.SetEnhancementSelectionItem(m_charData, &item, m_item.Ranks());
    m_tooltip.Show();
}

void CSelectionSelectDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
    // identify the clicked on option if any
    GetCursorPos(&point);
    ScreenToClient(&point);
    CWnd * pWnd = ChildWindowFromPoint(point);
    for (size_t i = 0; i < c_maxSelections; ++i)
    {
        if (pWnd == &m_buttonOption[i]
                && pWnd->IsWindowVisible()
                && pWnd->IsWindowEnabled())
        {
            // this is the item clicked on
            m_cost = m_costs[i];
            m_selection = m_selections[i];
            GetDlgItem(IDOK)->EnableWindow(TRUE);       // can now click ok
            for (size_t j = 0; j < c_maxSelections; ++j)
            {
                m_buttonOption[j].SetSelected(j == i);
            }
            break;
        }
    }
}
