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
    m_nextStanceId(IDC_SPECIALFEAT_0),
    m_nextSliderId(50)
{
}

CStancesView::~CStancesView()
{
}

void CStancesView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_HIDDEN_SIZER, m_staticHiddenSizer);
    DDX_Control(pDX, IDC_STATIC_USERCONTROLLED, m_userStances);
    DDX_Control(pDX, IDC_STATIC_AUTOCONTROLLED, m_autoStances);
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
    ON_WM_HSCROLL()
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
    if (IsWindow(m_userStances.GetSafeHwnd()))
    {
        // [Slider Label][Slider Control.........]
        // [User] [][][][][][][][][][][][][][][][]
        // [Auto] [][][][][][][][][][][][][][][][]
        CRect wndClient;
        GetClientRect(&wndClient);
        int sliderBottom = c_controlSpacing;
        // do all the sliders
        std::list<SliderItem>::iterator si = m_sliders.begin();
        CRect rctSizer;
        m_staticHiddenSizer.GetWindowRect(rctSizer);
        rctSizer -= rctSizer.TopLeft();
        while (si != m_sliders.end())
        {
            CRect rctLabel(c_controlSpacing, sliderBottom, c_controlSpacing + 100, sliderBottom + rctSizer.Height());
            CRect rctSlider(rctLabel.right + c_controlSpacing, sliderBottom, cx - c_controlSpacing, sliderBottom + rctSizer.Height());
            // now position the controls
            (*si).m_label->MoveWindow(rctLabel, TRUE);
            (*si).m_slider->MoveWindow(rctSlider, TRUE);
            (*si).m_label->ShowWindow(SW_SHOW);
            (*si).m_slider->ShowWindow(SW_SHOW);
            sliderBottom = rctSlider.bottom + c_controlSpacing;
            ++si;
        }
        CRect itemRect(
                c_controlSpacing,
                sliderBottom,
                c_windowSize + c_controlSpacing,
                sliderBottom + c_windowSize);
        // user stance header first
        m_userStances.MoveWindow(itemRect, TRUE);
        // move rectangle across for next set of controls
        itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
        if (itemRect.right > (wndClient.right - c_controlSpacing))
        {
            // oops, not enough space in client area here
            // move down and start the next row of controls
            itemRect -= CPoint(itemRect.left, 0);
            itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
        }
        // move each stance button
        for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
        {
            m_userStancebuttons[i]->MoveWindow(itemRect, TRUE);
            // move rectangle across for next set of controls
            itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
            if (itemRect.right > (wndClient.right - c_controlSpacing)
                    && i != m_userStancebuttons.size() - 1) // don't move for last one
            {
                // oops, not enough space in client area here
                // move down and start the next row of controls
                itemRect -= CPoint(itemRect.left, 0);
                itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
            }
        }
        // Now auto stances, which always start on a new line
        itemRect -= CPoint(itemRect.left, 0);
        itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
        m_autoStances.MoveWindow(itemRect, TRUE);
        // auto-controlled stances are only shown when active
        // move rectangle across for next set of controls
        itemRect += CPoint(itemRect.Width() + c_controlSpacing, 0);
        if (itemRect.right > (wndClient.right - c_controlSpacing))
        {
            // oops, not enough space in client area here
            // move down and start the next row of controls
            itemRect -= CPoint(itemRect.left, 0);
            itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
        }
        // move each stance button
        for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
        {
            if (m_autoStancebuttons[i]->IsSelected())
            {
                m_autoStancebuttons[i]->MoveWindow(itemRect, TRUE);
                m_autoStancebuttons[i]->ShowWindow(SW_SHOW);
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
            else
            {
                m_autoStancebuttons[i]->ShowWindow(SW_HIDE);
                CRect notVisibleRect(-100, -100, -68, -68);
                m_autoStancebuttons[i]->MoveWindow(notVisibleRect, FALSE);
            }
        }
        // ensure stances redraw correctly
        m_userStances.Invalidate(TRUE);
        m_autoStances.Invalidate(TRUE);
    }
    for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
    {
        m_userStancebuttons[i]->Invalidate(TRUE);
    }
    for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
    {
        m_autoStancebuttons[i]->Invalidate(TRUE);
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
    // Standard automatic Stances which are always present
    Stance twf("Two Weapon Fighting", "TwoWeaponFighting", "You are fighting with a weapon in each hand");
    twf.Set_AutoControlled();
    AddStance(twf);
    Stance thf("Two Handed Fighting", "TwoHandedFighting", "You are fighting with a single weapon in both hands");
    thf.Set_AutoControlled();
    AddStance(thf);
    Stance swf("Single Weapon Fighting", "SingleWeaponFighting", "You are fighting with a single weapon");
    swf.Set_AutoControlled();
    AddStance(swf);
    Stance unarmed("Unarmed", "ADanceOfFlowers", "You are fighting in Animal form or with handwraps / fists");
    unarmed.Set_AutoControlled();
    AddStance(unarmed);
    Stance axe("Axe", "BEReconstructedEdges", "You are fighting with an Axe");
    axe.Set_AutoControlled();
    AddStance(axe);
    Stance sab("Sword and Board", "SDLastStand", "You are fighting with a weapon and shield");
    sab.Set_AutoControlled();
    AddStance(sab);
    Stance staff("Staff", "HMSoundingStaff", "You are fighting with a staff");
    staff.Set_AutoControlled();
    AddStance(staff);
    Stance orb("Orb", "EfficientMetamagic", "You are fighting with an Orb in your off hand");
    orb.Set_AutoControlled();
    AddStance(orb);
    Stance runearm("Rune Arm", "BEBattleEngineer", "You are fighting with a Rune-Arm in your off hand");
    runearm.Set_AutoControlled();
    AddStance(runearm);
    Stance Swashbuckling("Swashbuckling", "SBEvasiveManeuvers", "You are Swashbuckling");
    Swashbuckling.Set_AutoControlled();
    AddStance(Swashbuckling);
}

void CStancesView::AddStance(const Stance & stance)
{
    // only add the stance if it is not already present
    bool found = false;
    for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
    {
        if (m_userStancebuttons[i]->IsYou(stance))
        {
            found = true;
            m_userStancebuttons[i]->AddStack();
        }
    }
    for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
    {
        if (m_autoStancebuttons[i]->IsYou(stance))
        {
            found = true;
            m_autoStancebuttons[i]->AddStack();
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

        if (stance.HasAutoControlled())
        {
            // now create the new auto stance control
            m_autoStancebuttons.push_back(new CStanceButton(m_pCharacter, stance));
            // create a parent window that is c_windowSize by c_windowSize pixels in size
            m_autoStancebuttons.back()->Create(
                    "",
                    WS_CHILD,       // default is not visible
                    itemRect,
                    this,
                    m_nextStanceId++);
            m_autoStancebuttons.back()->AddStack();
        }
        else
        {
            // now create the new user stance control
            m_userStancebuttons.push_back(new CStanceButton(m_pCharacter, stance));
            // create a parent window that is c_windowSize by c_windowSize pixels in size
            m_userStancebuttons.back()->Create(
                    "",
                    WS_CHILD | WS_VISIBLE,
                    itemRect,
                    this,
                    m_nextStanceId++);
            m_userStancebuttons.back()->AddStack();
        }
        if (IsWindow(GetSafeHwnd()))
        {
            // now force an on size event to position everything
            CRect rctWnd;
            GetClientRect(&rctWnd);
            OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
        }
    }
}

void CStancesView::RevokeStance(const Stance & stance)
{
    // only revoke the stance if it is not already present and its the last stack of it
    bool found = false;
    size_t i;
    for (i = 0; i < m_userStancebuttons.size(); ++i)
    {
        if (m_userStancebuttons[i]->IsYou(stance))
        {
            found = true;
            m_userStancebuttons[i]->RevokeStack();
            break;      // keep the index
        }
    }
    if (found
            && m_userStancebuttons[i]->NumStacks() == 0)
    {
        // all instances of this stance are gone, remove the button
        m_userStancebuttons[i]->DestroyWindow();
        delete m_userStancebuttons[i];
        m_userStancebuttons[i] = NULL;
        // clear entries from the array
        std::vector<CStanceButton *>::iterator it = m_userStancebuttons.begin() + i;
        m_userStancebuttons.erase(it);
        // now force an on size event
        CRect rctWnd;
        GetClientRect(&rctWnd);
        OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
    }
    if (!found)
    {
        for (i = 0; i < m_autoStancebuttons.size(); ++i)
        {
            if (m_autoStancebuttons[i]->IsYou(stance))
            {
                found = true;
                m_autoStancebuttons[i]->RevokeStack();
                break;      // keep the index
            }
        }
        if (found
                && m_autoStancebuttons[i]->NumStacks() == 0)
        {
            // all instances of this stance are gone, remove the button
            m_autoStancebuttons[i]->DestroyWindow();
            delete m_autoStancebuttons[i];
            m_autoStancebuttons[i] = NULL;
            // clear entries from the array
            std::vector<CStanceButton *>::iterator it = m_autoStancebuttons.begin() + i;
            m_autoStancebuttons.erase(it);
            // now force an on size event
            CRect rctWnd;
            GetClientRect(&rctWnd);
            OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
        }
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
    bool resize = false;
    // update the state of the required stance buttons
    for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
    {
        if (m_userStancebuttons[i]->GetStance().Name() == stanceName)
        {
            m_userStancebuttons[i]->SetSelected(true);
        }
    }
    // update the state of the required stance buttons
    for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
    {
        if (m_autoStancebuttons[i]->GetStance().Name() == stanceName)
        {
            m_autoStancebuttons[i]->SetSelected(true);
            resize = true;
        }
    }
    // if an auto-controlled stance changed state, we need to show/hide its button
    if (resize)
    {
        CRect rctWnd;
        GetClientRect(&rctWnd);
        OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
    }
}

void CStancesView::UpdateStanceDeactivated(Character * charData, const std::string & stanceName)
{
    bool resize = false;
    // update the state of the required stance buttons
    for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
    {
        if (m_userStancebuttons[i]->GetStance().Name() == stanceName)
        {
            m_userStancebuttons[i]->SetSelected(false);
        }
    }
    // update the state of the required stance buttons
    for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
    {
        if (m_autoStancebuttons[i]->GetStance().Name() == stanceName)
        {
            m_autoStancebuttons[i]->SetSelected(false);
            resize = true;
        }
    }
    // if an auto-controlled stance changed state, we need to show/hide its button
    if (resize)
    {
        CRect rctWnd;
        GetClientRect(&rctWnd);
        OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
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

void CStancesView::UpdateFeatEffectRevoked(
        Character * charData,
        const std::string & featName,
        const Effect & effect)
{
    UpdateSliders(effect, false);
}

void CStancesView::UpdateEnhancementEffect(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // all handled the same way
    UpdateItemEffect(charData, enhancementName, effect.m_effect);
}

void CStancesView::UpdateEnhancementEffectRevoked(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    UpdateSliders(effect.m_effect, false);
}

void CStancesView::UpdateItemEffect(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    // see if this is an activate stance effect
    if (effect.Type() == Effect_ActivateStance)
    {
        bool resize = false;
        ASSERT(effect.Stance().size() == 1);
        for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
        {
            if (m_userStancebuttons[i]->GetStance().Name() == effect.Stance()[0])
            {
                m_pCharacter->ActivateStance(m_userStancebuttons[i]->GetStance());
            }
        }
        for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
        {
            if (m_autoStancebuttons[i]->GetStance().Name() == effect.Stance()[0])
            {
                m_pCharacter->ActivateStance(m_autoStancebuttons[i]->GetStance());
                resize = true;
            }
        }
        // if an auto-controlled stance changed state, we need to show/hide its button
        if (resize)
        {
            CRect rctWnd;
            GetClientRect(&rctWnd);
            OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
        }
    }
    UpdateSliders(effect, true);
}

void CStancesView::UpdateItemEffectRevoked(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    UpdateSliders(effect, false);
}

void CStancesView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // determine which stance, if any was clicked on.
    // if they did click on a stance either activate or deactivate it
    CFormView::OnLButtonDown(nFlags, point);
    if (m_userStancebuttons.size() > 0)
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
    for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
    {
        m_userStancebuttons[i]->DestroyWindow();
        delete m_userStancebuttons[i];
        m_userStancebuttons[i] = NULL;
    }
    m_userStancebuttons.clear();
    for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
    {
        m_autoStancebuttons[i]->DestroyWindow();
        delete m_autoStancebuttons[i];
        m_autoStancebuttons[i] = NULL;
    }
    m_autoStancebuttons.clear();

    std::list<SliderItem>::iterator it = m_sliders.begin();
    while (it != m_sliders.end())
    {
        // time for this slider to disappear
        (*it).m_label->DestroyWindow();
        (*it).m_slider->DestroyWindow();
        it = m_sliders.erase(it);
    }
}

const std::vector<CStanceButton *> & CStancesView::UserStances() const
{
    return m_userStancebuttons;
}

const std::vector<CStanceButton *> & CStancesView::AutoStances() const
{
    return m_autoStancebuttons;
}

const CStanceButton * CStancesView::GetStance(const std::string & stanceName) const
{
    const CStanceButton * pButton = NULL;
    for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
    {
        if (m_userStancebuttons[i]->GetStance().Name() == stanceName)
        {
            pButton = m_userStancebuttons[i];
        }
    }
    for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
    {
        if (m_autoStancebuttons[i]->GetStance().Name() == stanceName)
        {
            pButton = m_autoStancebuttons[i];
        }
    }
    return pButton;
}

void CStancesView::OnHScroll(UINT sbCode, UINT nPos, CScrollBar * pScrollbar)
{
    // find which control has changed and update if required
    UINT id = pScrollbar->GetDlgCtrlID();
    std::list<SliderItem>::iterator it = GetSlider(id);
    if (it != m_sliders.end())
    {
        nPos = (*it).m_slider->GetPos();
        if ((*it).m_position != nPos)
        {
            (*it).m_position = nPos;
            CString windowText;
            windowText.Format("%s: %d", (*it).m_name.c_str(), (*it).m_position);
            (*it).m_label->SetWindowText(windowText);
            m_pCharacter->StanceSliderChanged();
        }
    }
}

void CStancesView::UpdateSliders(const Effect & effect, bool bApply)
{
    if (effect.Type() == Effect_CreateSlider)
    {
        if (bApply)
        {
            // need to create or increment slider
            std::list<SliderItem>::iterator it = GetSlider(effect, true);
            (*it).m_creationCount++;    // count a reference
        }
        else
        {
            // need to decrement or removal slider
            std::list<SliderItem>::iterator it = GetSlider(effect, false);
            if (it != m_sliders.end())
            {
                // does exist, we can revoke a layer
                (*it).m_creationCount--;    // lose a reference
                if ((*it).m_creationCount == 0)
                {
                    // time for this slider to disappear
                    (*it).m_label->DestroyWindow();
                    (*it).m_slider->DestroyWindow();
                    m_sliders.erase(it);
                    // make sure controls are positioned
                    CRect rctWindow;
                    GetWindowRect(&rctWindow);
                    OnSize(SIZE_RESTORED, rctWindow.Width(), rctWindow.Height());
                }
            }
        }
    }
}

std::list<SliderItem>::iterator CStancesView::GetSlider(
        const Effect & effect,
        bool bCreateIfMissing)
{
    bool bFound = false;
    std::list<SliderItem>::iterator it = m_sliders.begin();
    while (!bFound && it != m_sliders.end())
    {
        if ((*it).m_name == effect.Slider())
        {
            bFound = true;  // this is the droid we're looking for
        }
        else
        {
            ++it;
        }
    }
    if (!bFound && bCreateIfMissing)
    {
        // add a new entry to the end
        SliderItem slider;
        m_sliders.push_front(slider);
        it = m_sliders.begin();
        (*it).CreateControls();
        // now initialise it fully
        (*it).m_sliderControlId = m_nextSliderId;
        (*it).m_name = effect.Slider();
        (*it).m_creationCount = 0;  // increment outside
        (*it).m_sliderMin = (int)effect.AmountVector().at(0);
        (*it).m_sliderMax = (int)effect.AmountVector().at(1);
        (*it).m_position = (*it).m_sliderMax;
        // and create the windows controls
        CString windowText;
        windowText.Format("%s: %d", effect.Slider().c_str(), (*it).m_position);
        (*it).m_label->Create(
                windowText,
                SS_CENTERIMAGE | WS_VISIBLE | WS_CHILD,
                CRect(0, 0, 10, 10),        // updates on OnSize
                this,
                (*it).m_sliderControlId+1);
        (*it).m_slider->Create(
                WS_VISIBLE | WS_CHILD | TBS_AUTOTICKS | TBS_HORZ | TBS_BOTTOM,
                CRect(0, 0, 10, 10),        // updates on OnSize
                this,
                (*it).m_sliderControlId);
        CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
        (*it).m_label->SetFont(pDefaultGUIFont, TRUE);
        (*it).m_slider->SetFont(pDefaultGUIFont, TRUE);
        (*it).m_slider->SetRange((*it).m_sliderMin, (*it).m_sliderMax);
        (*it).m_slider->SetPos((*it).m_position);
        m_nextSliderId += 2;    // skip 2 control ids (label, slider)
        // make sure controls are positioned
        CRect rctWindow;
        GetWindowRect(&rctWindow);
        OnSize(SIZE_RESTORED, rctWindow.Width(), rctWindow.Height());
    }
    return it;
}

std::list<SliderItem>::iterator CStancesView::GetSlider(UINT controlId)
{
    bool bFound = false;
    std::list<SliderItem>::iterator it = m_sliders.begin();
    while (!bFound && it != m_sliders.end())
    {
        if ((*it).m_sliderControlId == controlId)
        {
            bFound = true;  // this is the droid we're looking for
        }
        else
        {
            ++it;
        }
    }
    ASSERT(bFound);
    return it;
}

std::list<SliderItem>::const_iterator CStancesView::GetSlider(
        const std::string & name) const
{
    bool bFound = false;
    std::list<SliderItem>::const_iterator it = m_sliders.begin();
    while (!bFound && it != m_sliders.end())
    {
        if ((*it).m_name == name)
        {
            bFound = true;  // this is the droid we're looking for
        }
        else
        {
            ++it;
        }
    }
    ASSERT(bFound);
    return it;
}

bool CStancesView::IsStanceActive(const std::string & name) const
{
    bool bEnabled = false;
    // some special stances are based on a slider position
    // all these stances start with a numeric with the format of
    // "<number>% <stance name>"
    if (name.find("%") != std::string::npos)
    {
        // this is a special stance
        int value = atoi(name.c_str());
        // identify the slider stance in question
        std::string stanceName = name.substr(name.find('%') + 2, 50);
        std::list<SliderItem>::const_iterator it = GetSlider(stanceName);
        if (it != m_sliders.end())
        {
            // if the value is negative its an under comparison else its an over
            if (value < 0)
            {
                value = -value;
                bEnabled = ((*it).m_position < value);
            }
            else
            {
                bEnabled = ((*it).m_position >= value);
            }
        }
    }
    return bEnabled;
}