// StancesView.cpp
//
#include "stdafx.h"
#include "StancesView.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
    const int c_windowSize = 38;
}

IMPLEMENT_DYNCREATE(CStancesView, CFormView)

CStancesView::CStancesView() :
    CFormView(CStancesView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL),
    m_tipCreated(false),
    m_pTooltipItem(NULL),
    m_nextStanceId(IDC_SPECIALFEAT_0)
{
}

CStancesView::~CStancesView()
{
}

void CStancesView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CStancesView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CStancesView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CStancesView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CStancesView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    m_tooltip.Create(this);
    m_tipCreated = true;
}

void CStancesView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    CRect wndClient;
    GetClientRect(&wndClient);
    CRect itemRect(
            c_controlSpacing,
            c_controlSpacing,
            c_windowSize + c_controlSpacing,
            c_windowSize + c_controlSpacing);
    // move each stance button
    for (size_t i = 0; i < m_stancebuttons.size(); ++i)
    {
        m_stancebuttons[i]->MoveWindow(itemRect, TRUE);
        // move rectangle across for next set of controls
        itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
        if (itemRect.right > (wndClient.right - c_controlSpacing))
        {
            // oops, not enough space in client area here
            // move down and start the next row of controls
            itemRect -= CPoint(itemRect.left, 0);
            itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
        }
    }
}

LRESULT CStancesView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
        DestroyAllStances();
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
        CreateStanceWindows();
    }
    return 0L;
}

BOOL CStancesView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CStancesView::CreateStanceWindows()
{
    m_nextStanceId = IDC_SPECIALFEAT_0;
    // always include a Reaper Stance which cannot be revoked, but allows
    // totals to be updated to show included reaper enhancements
    Stance reaper("Reaper", "ReaperTree", "Activate to view your characters abilities when running in Reaper mode");
    AddStance(reaper);

    Stance twf("TwoWeaponFighting", "TwoWeaponFighting", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with a weapon in each hand");
    AddStance(twf);
    Stance thf("TwoHandedFighting", "TwoHandedFighting", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with a single weapon in both hands");
    AddStance(thf);
    Stance swf("SingleWeaponFighting", "SingleWeaponFighting", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with a single weapon");
    AddStance(swf);
    Stance unarmed("Unarmed", "ADanceOfFlowers", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with handwraps or fists");
    AddStance(unarmed);
    Stance sab("SwordAndBoard", "SDLastStand", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with a weapon and shield");
    AddStance(sab);
    Stance staff("Staff", "HMSoundingStaff", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with a staff");
    AddStance(staff);
    Stance orb("Orb", "EfficientMetamagic", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with an Orb in your off hand");
    AddStance(orb);
    Stance runearm("RunArm", "BEBattleEngineer", "This stance is only visible due to development\nWill be hidden in final build\nYou are fighting with a Rune-Arm in your off hand");
    AddStance(runearm);
    Stance Swashbuckling("Swashbuckling", "SBEvasiveManeuvers", "This stance is only visible due to development\nWill be hidden in final build\nYou are Swashbuckling");
    AddStance(Swashbuckling);
    // look at all the trained feats and see which are stances
    // add an entry for each
    std::list<TrainedFeat> currentFeats = m_pCharacter->CurrentFeats(MAX_LEVEL);
    std::list<TrainedFeat>::iterator fit = currentFeats.begin();
    while (fit != currentFeats.end())
    {
        // look up the feat associated with this
        Feat feat = FindFeat((*fit).FeatName());
        const std::list<Stance> & stances = feat.StanceData();
        std::list<Stance>::const_iterator sit = stances.begin();
        while (sit != stances.end())
        {
            AddStance((*sit));
            ++sit;
        }
        ++fit;
    }
    // now do the same for all trained enhancements
    std::list<TrainedEnhancement> currentEnhancements = m_pCharacter->CurrentEnhancements();
    std::list<TrainedEnhancement>::const_iterator eit = currentEnhancements.begin();
    while (eit != currentEnhancements.end())
    {
        const EnhancementTreeItem * pItem = FindEnhancement((*eit).EnhancementName());
        if (pItem != NULL)
        {
            // enhancements may give multiple stances
            std::list<Stance>::const_iterator sit = pItem->Stances().begin();
            while (sit != pItem->Stances().end())
            {
                for (size_t i = 0; i < (*eit).Ranks(); ++i)
                {
                    // train once per rank of enhancement trained
                    AddStance((*sit));
                }
                ++sit;
            }
        }
        ++eit;
    }
}

void CStancesView::AddStance(const Stance & stance)
{
    // only add the stance if it is not already present
    bool found = false;
    for (size_t i = 0; i < m_stancebuttons.size(); ++i)
    {
        if (m_stancebuttons[i]->IsYou(stance))
        {
            found = true;
            m_stancebuttons[i]->AddStack();
        }
    }
    if (!found)
    {
        // position the created windows left to right until
        // they don't fit then move them down a row and start again
        // each stance window is c_windowSize * c_windowSize pixels
        CRect wndClient;
        GetClientRect(&wndClient);
        CRect itemRect(
                c_controlSpacing,
                c_controlSpacing,
                c_windowSize + c_controlSpacing,
                c_windowSize + c_controlSpacing);
        // move the rectangle by the number of existing stances
        for (size_t i = 0; i < m_stancebuttons.size(); ++i)
        {
            // move rectangle across for next set of controls
            itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
            if (itemRect.right > (wndClient.right - c_controlSpacing))
            {
                // oops, not enough space in client area here
                // move down and start the next row of controls
                itemRect -= CPoint(itemRect.left, 0);
                itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
            }
        }

        // now create the new stance control
        m_stancebuttons.push_back(new CStanceButton(m_pCharacter, stance));
        // create a parent window that is c_windowSize by c_windowSize pixels in size
        m_stancebuttons.back()->Create(
                "",
                WS_CHILD | WS_VISIBLE,
                itemRect,
                this,
                m_nextStanceId++);
    }
}

void CStancesView::RevokeStance(const Stance & stance)
{
    // only revoke the stance if it is not already present and its the last stack of it
    bool found = false;
    size_t i;
    for (i = 0; i < m_stancebuttons.size(); ++i)
    {
        if (m_stancebuttons[i]->IsYou(stance))
        {
            found = true;
            m_stancebuttons[i]->RevokeStack();
            break;      // keep the index
        }
    }
    if (found
            && m_stancebuttons[i]->NumStacks() == 0)
    {
        // all instances of this stance are gone, remove the button
        m_stancebuttons[i]->DestroyWindow();
        delete m_stancebuttons[i];
        m_stancebuttons[i] = NULL;
        // clear entries from the array
        std::vector<CStanceButton *>::iterator it = m_stancebuttons.begin() + i;
        m_stancebuttons.erase(it);
        // now force an on size event
        CRect rctWnd;
        GetClientRect(&rctWnd);
        OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
    }
}

void CStancesView::UpdateNewStance(Character * charData, const Stance & stance)
{
    AddStance(stance);
}

void CStancesView::UpdateRevokeStance(Character * charData, const Stance & stance)
{
    RevokeStance(stance);
}

void CStancesView::UpdateStanceActivated(Character * charData, const std::string & stanceName)
{
    // update the state of the required stance buttons
    for (size_t i = 0; i < m_stancebuttons.size(); ++i)
    {
        if (m_stancebuttons[i]->GetStance().Name() == stanceName)
        {
            m_stancebuttons[i]->SetSelected(true);
        }
    }
}

void CStancesView::UpdateStanceDeactivated(Character * charData, const std::string & stanceName)
{
    // update the state of the required stance buttons
    for (size_t i = 0; i < m_stancebuttons.size(); ++i)
    {
        if (m_stancebuttons[i]->GetStance().Name() == stanceName)
        {
            m_stancebuttons[i]->SetSelected(false);
        }
    }
}

void CStancesView::UpdateFeatEffect(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    // all handled the same way
    UpdateItemEffect(charData, featName, effect);
}

void CStancesView::UpdateEnhancementEffect(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // all handled the same way
    UpdateItemEffect(charData, enhancementName, effect.m_effect);
}

void CStancesView::UpdateItemEffect(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    // see if this is an activate stance effect
    if (effect.Type() == Effect_ActivateStance)
    {
        ASSERT(effect.Stance().size() == 1);
        for (size_t i = 0; i < m_stancebuttons.size(); ++i)
        {
            if (m_stancebuttons[i]->GetStance().Name() == effect.Stance()[0])
            {
                m_pCharacter->ActivateStance(m_stancebuttons[i]->GetStance());
            }
        }
    }
}

void CStancesView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // determine which stance, if any was clicked on.
    // if they did click on a stance either activate or deactivate it
    CFormView::OnLButtonDown(nFlags, point);
    if (m_stancebuttons.size() > 0)
    {
        CWnd * pWnd = ChildWindowFromPoint(point);
        CStanceButton * pStance = dynamic_cast<CStanceButton*>(pWnd);
        if (pStance != NULL)
        {
            if (!pStance->GetStance().HasAutoControlled())
            {
                // yup, they clicked on a stance, now change its activation state
                if (pStance->IsSelected())
                {
                    m_pCharacter->DeactivateStance(pStance->GetStance());
                }
                else
                {
                    m_pCharacter->ActivateStance(pStance->GetStance());
                }
                m_pDocument->SetModifiedFlag(TRUE);
            }
            else
            {
                // show action is not available
                ::MessageBeep(0xFFFFFFFF);
            }
        }
    }
}

void CStancesView::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which stance the mouse may be over
    CWnd * pWnd = ChildWindowFromPoint(point);
    CStanceButton * pStance = dynamic_cast<CStanceButton*>(pWnd);
    if (pStance != NULL
            && pStance != m_pTooltipItem)
    {
        CRect itemRect;
        pStance->GetWindowRect(&itemRect);
        ScreenToClient(itemRect);
        // over a new item or a different item
        m_pTooltipItem = pStance;
        ShowTip(*pStance, itemRect);
        if (pStance->GetStance().HasAutoControlled())
        {
            GetMainFrame()->SetStatusBarPromptText("This stances state is controlled by the software. Cannot be manually changed.");
        }
        else
        {
            GetMainFrame()->SetStatusBarPromptText("Click to toggle stance state.");
        }
    }
    else
    {
        if (m_showingTip
                && pStance != m_pTooltipItem)
        {
            // no longer over the same item
            HideTip();
        }
        // as the mouse is over the enhancement tree, ensure the status bar message prompts available actions
        GetMainFrame()->SetStatusBarPromptText("Ready.");
    }
}

LRESULT CStancesView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    GetMainFrame()->SetStatusBarPromptText("Ready.");
    return 0;
}

void CStancesView::ShowTip(const CStanceButton & item, CRect itemRect)
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

void CStancesView::HideTip()
{
    // tip not shown if not over an assay
    if (m_tipCreated && m_showingTip)
    {
        m_tooltip.Hide();
        m_showingTip = false;
        m_pTooltipItem = NULL;
    }
}

void CStancesView::SetTooltipText(
        const CStanceButton & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetStanceItem(*m_pCharacter, &item.GetStance());
    m_tooltip.Show();
}

void CStancesView::DestroyAllStances()
{
    for (size_t i = 0; i < m_stancebuttons.size(); ++i)
    {
        m_stancebuttons[i]->DestroyWindow();
        delete m_stancebuttons[i];
        m_stancebuttons[i] = NULL;
    }
    m_stancebuttons.clear();
}

