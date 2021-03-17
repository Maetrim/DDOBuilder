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
    m_showingTip(false),
    m_nextStanceId(IDC_SPECIALFEAT_0),
    m_nextSliderId(50)
{
}

CStancesView::~CStancesView()
{
    DestroyAllStances();
}

void CStancesView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_HIDDEN_SIZER, m_staticHiddenSizer);
    DDX_Control(pDX, IDC_STATIC_USERCONTROLLED, m_userStances);
    DDX_Control(pDX, IDC_STATIC_USER_ARCANE, m_userStancesArcane);
    DDX_Control(pDX, IDC_STATIC_USER_DIVINE, m_userStancesDivine);
    DDX_Control(pDX, IDC_STATIC_USER_MARTIAL, m_userStancesMartial);
    DDX_Control(pDX, IDC_STATIC_USER_PRIMAL, m_userStancesPrimal);
    DDX_Control(pDX, IDC_STATIC_USER_ICONIC, m_userStancesIconic);
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

void CStancesView::PositionWindow(
        CWnd * pWnd,
        CRect * itemRect,
        int maxX)
{
    pWnd->MoveWindow(itemRect, TRUE);
    *itemRect += CPoint(itemRect->Width() + c_controlSpacing, 0);
    if (itemRect->right > maxX)
    {
        // oops, not enough space in client area here
        // move down and start the next row of controls
        *itemRect -= CPoint(itemRect->left, 0);
        *itemRect += CPoint(c_controlSpacing, itemRect->Height() + c_controlSpacing);
    }
    pWnd->Invalidate(TRUE);
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
        // work out the right hand end of the slider controls
        int sliderEnd = c_windowSize + c_controlSpacing;
        while (sliderEnd < cx - (c_windowSize + c_controlSpacing))
        {
            sliderEnd += (c_windowSize + c_controlSpacing);
        }
        // do all the sliders
        std::list<SliderItem>::iterator si = m_sliders.begin();
        CRect rctSizer;
        m_staticHiddenSizer.GetWindowRect(rctSizer);
        rctSizer -= rctSizer.TopLeft();
        while (si != m_sliders.end())
        {
            CRect rctLabel(c_controlSpacing, sliderBottom, c_controlSpacing + 100, sliderBottom + rctSizer.Height());
            CRect rctSlider(rctLabel.right + c_controlSpacing, sliderBottom, sliderEnd, sliderBottom + rctSizer.Height());
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
        int maxX = wndClient.right - c_controlSpacing;
        PositionWindow(&m_userStances, &itemRect, maxX);
        // move each stance button
        for (size_t i = 0; i < m_userStancebuttons.size(); ++i)
        {
            PositionWindow(m_userStancebuttons[i], &itemRect, maxX);
        }
        // arcane, divine, martial and primal have their own user sections
        if (m_userStancebuttonsArcane.size() > 0)
        {
            m_userStancesArcane.ShowWindow(SW_SHOW);
            PositionWindow(&m_userStancesArcane, &itemRect, maxX);
            for (size_t i = 0; i < m_userStancebuttonsArcane.size(); ++i)
            {
                PositionWindow(m_userStancebuttonsArcane[i], &itemRect, maxX);
            }
        }
        else
        {
            m_userStancesArcane.ShowWindow(SW_HIDE);
            CRect notVisibleRect(-100, -100, -68, -68);
            m_userStancesArcane.MoveWindow(notVisibleRect, FALSE);
        }
        if (m_userStancebuttonsDivine.size() > 0)
        {
            m_userStancesDivine.ShowWindow(SW_SHOW);
            PositionWindow(&m_userStancesDivine, &itemRect, maxX);
            for (size_t i = 0; i < m_userStancebuttonsDivine.size(); ++i)
            {
                PositionWindow(m_userStancebuttonsDivine[i], &itemRect, maxX);
            }
        }
        else
        {
            m_userStancesDivine.ShowWindow(SW_HIDE);
            CRect notVisibleRect(-100, -100, -68, -68);
            m_userStancesDivine.MoveWindow(notVisibleRect, FALSE);
        }
        if (m_userStancebuttonsMartial.size() > 0)
        {
            m_userStancesMartial.ShowWindow(SW_SHOW);
            PositionWindow(&m_userStancesMartial, &itemRect, maxX);
            for (size_t i = 0; i < m_userStancebuttonsMartial.size(); ++i)
            {
                PositionWindow(m_userStancebuttonsMartial[i], &itemRect, maxX);
            }
        }
        else
        {
            m_userStancesMartial.ShowWindow(SW_HIDE);
            CRect notVisibleRect(-100, -100, -68, -68);
            m_userStancesMartial.MoveWindow(notVisibleRect, FALSE);
        }
        if (m_userStancebuttonsPrimal.size() > 0)
        {
            m_userStancesPrimal.ShowWindow(SW_SHOW);
            PositionWindow(&m_userStancesPrimal, &itemRect, maxX);
            for (size_t i = 0; i < m_userStancebuttonsPrimal.size(); ++i)
            {
                PositionWindow(m_userStancebuttonsPrimal[i], &itemRect, maxX);
            }
        }
        else
        {
            m_userStancesPrimal.ShowWindow(SW_HIDE);
            CRect notVisibleRect(-100, -100, -68, -68);
            m_userStancesPrimal.MoveWindow(notVisibleRect, FALSE);
        }
        if (m_userStancebuttonsIconic.size() > 0)
        {
            m_userStancesIconic.ShowWindow(SW_SHOW);
            PositionWindow(&m_userStancesIconic, &itemRect, maxX);
            for (size_t i = 0; i < m_userStancebuttonsIconic.size(); ++i)
            {
                PositionWindow(m_userStancebuttonsIconic[i], &itemRect, maxX);
            }
        }
        else
        {
            m_userStancesIconic.ShowWindow(SW_HIDE);
            CRect notVisibleRect(-100, -100, -68, -68);
            m_userStancesIconic.MoveWindow(notVisibleRect, FALSE);
        }
        // Now auto stances, which always start on a new line (unless we are at a new line)
        if (itemRect.left != c_controlSpacing)
        {
            itemRect -= CPoint(itemRect.left, 0);
            itemRect += CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
        }
        PositionWindow(&m_autoStances, &itemRect, maxX);
        // move each stance button
        for (size_t i = 0; i < m_autoStancebuttons.size(); ++i)
        {
            if (m_autoStancebuttons[i]->IsSelected())
            {
                m_autoStancebuttons[i]->ShowWindow(SW_SHOW);
                PositionWindow(m_autoStancebuttons[i], &itemRect, maxX);
            }
            else
            {
                m_autoStancebuttons[i]->ShowWindow(SW_HIDE);
                CRect notVisibleRect(-100, -100, -68, -68);
                m_autoStancebuttons[i]->MoveWindow(notVisibleRect, FALSE);
            }
        }
        if (itemRect.left == c_controlSpacing)
        {
            // avoid a blank row at the bottom
            itemRect -= CPoint(itemRect.left, 0);
            itemRect -= CPoint(c_controlSpacing, itemRect.Height() + c_controlSpacing);
        }

        // show scroll bars if required
        SetScrollSizes(
                MM_TEXT,
                CSize(cx, itemRect.bottom + c_controlSpacing));
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
        m_weaponGroups.clear();
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
    Stance thrown("Thrown Weapon", "AMEvocationIMagicMissile", "You are wielding a thrown weapon");
    thrown.Set_AutoControlled();
    AddStance(thrown);
    // add the auto controlled stances for each weapon type
    for (size_t wt = Weapon_Unknown; wt < Weapon_Count; ++wt)
    {
        CString name = (LPCTSTR)EnumEntryText((WeaponType)wt, weaponTypeMap);
        CString prompt;
        prompt.Format("You are wielding a %s", name);
        Stance weapon(
                (LPCTSTR)name,
                (LPCTSTR)name,
                (LPCTSTR)prompt);
        weapon.Set_AutoControlled();
        AddStance(weapon);
    }
    // add the auto controlled stances for each race type
    for (size_t rt = Race_Unknown; rt < Race_Count; ++rt)
    {
        CString name = (LPCTSTR)EnumEntryText((RaceType)rt, raceTypeMap);
        CString prompt;
        prompt.Format("You are %s", name);
        Stance race(
                (LPCTSTR)name,
                (LPCTSTR)name,
                (LPCTSTR)prompt);
        race.Set_AutoControlled();
        AddStance(race);
    }
}

bool CStancesView::AddStance(
        const Stance & stance,
        std::vector<CStanceButton *> & items)
{
    bool found = false;
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (items[i]->IsYou(stance))
        {
            found = true;
            items[i]->AddStack();
        }
    }
    return found;
}

void CStancesView::AddStance(const Stance & stance)
{
    // only add the stance if it is not already present
    bool found = false;
    found |= AddStance(stance, m_userStancebuttons);
    found |= AddStance(stance, m_userStancebuttonsArcane);
    found |= AddStance(stance, m_userStancebuttonsDivine);
    found |= AddStance(stance, m_userStancebuttonsMartial);
    found |= AddStance(stance, m_userStancebuttonsPrimal);
    found |= AddStance(stance, m_userStancebuttonsIconic);
    found |= AddStance(stance, m_autoStancebuttons);
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
            CStanceButton * pStance = new CStanceButton(m_pCharacter, stance);
            // create a parent window that is c_windowSize by c_windowSize pixels in size
            pStance->Create(
                    "",
                    WS_CHILD | WS_VISIBLE,
                    itemRect,
                    this,
                    m_nextStanceId++);
            pStance->AddStack();
            // add it to the correct user stance grouping
            if (stance.HasGroup())
            {
                if (stance.Group() == "Arcane")
                {
                    m_userStancebuttonsArcane.push_back(pStance);
                }
                else if (stance.Group() == "Divine")
                {
                    m_userStancebuttonsDivine.push_back(pStance);
                }
                else if (stance.Group() == "Martial")
                {
                    m_userStancebuttonsMartial.push_back(pStance);
                }
                else if (stance.Group() == "Primal")
                {
                    m_userStancebuttonsPrimal.push_back(pStance);
                }
                else if (stance.Group() == "Iconic")
                {
                    m_userStancebuttonsIconic.push_back(pStance);
                }
                else
                {
                    m_userStancebuttons.push_back(pStance);
                }
            }
            else
            {
                m_userStancebuttons.push_back(pStance);
            }
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

void CStancesView::RevokeStance(
        const Stance & stance,
        std::vector<CStanceButton *> & items)
{
    // only revoke the stance if it is not already present and its the last stack of it
    bool found = false;
    size_t i;
    for (i = 0; i < items.size(); ++i)
    {
        if (items[i]->IsYou(stance))
        {
            found = true;
            items[i]->RevokeStack();
            break;      // keep the index
        }
    }
    if (found
            && items[i]->NumStacks() == 0)
    {
        // all instances of this stance are gone, remove the button
        items[i]->DestroyWindow();
        delete items[i];
        items[i] = NULL;
        // clear entries from the array
        std::vector<CStanceButton *>::iterator it = items.begin() + i;
        items.erase(it);
        // now force an on size event
        CRect rctWnd;
        GetClientRect(&rctWnd);
        OnSize(SIZE_RESTORED, rctWnd.Width(), rctWnd.Height());
    }
}

void CStancesView::RevokeStance(const Stance & stance)
{
    RevokeStance(stance, m_userStancebuttons);
    RevokeStance(stance, m_userStancebuttonsArcane);
    RevokeStance(stance, m_userStancebuttonsDivine);
    RevokeStance(stance, m_userStancebuttonsMartial);
    RevokeStance(stance, m_userStancebuttonsPrimal);
    RevokeStance(stance, m_userStancebuttonsIconic);
    RevokeStance(stance, m_autoStancebuttons);
}

void CStancesView::UpdateNewStance(Character * charData, const Stance & stance)
{
    if (!stance.HasSetBonus())
    {
        AddStance(stance);
    }
}

void CStancesView::UpdateRevokeStance(Character * charData, const Stance & stance)
{
    if (!stance.HasSetBonus())
    {
        RevokeStance(stance);
    }
}

void CStancesView::StanceActivated(
        std::vector<CStanceButton *> & items,
        const std::string & stanceName)
{
    // update the state of the required stance buttons
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (items[i]->GetStance().Name() == stanceName)
        {
            items[i]->SetSelected(true);
        }
    }
}

void CStancesView::UpdateStanceActivated(Character * charData, const std::string & stanceName)
{
    bool resize = false;
    StanceActivated(m_userStancebuttons, stanceName);
    StanceActivated(m_userStancebuttonsArcane, stanceName);
    StanceActivated(m_userStancebuttonsDivine, stanceName);
    StanceActivated(m_userStancebuttonsMartial, stanceName);
    StanceActivated(m_userStancebuttonsPrimal, stanceName);
    StanceActivated(m_userStancebuttonsIconic, stanceName);
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

void CStancesView::StanceDeactivated(
        std::vector<CStanceButton *> & items,
        const std::string & stanceName)
{
    // update the state of the required stance buttons
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (items[i]->GetStance().Name() == stanceName)
        {
            items[i]->SetSelected(false);
        }
    }
}

void CStancesView::UpdateStanceDeactivated(Character * charData, const std::string & stanceName)
{
    bool resize = false;
    StanceDeactivated(m_userStancebuttons, stanceName);
    StanceDeactivated(m_userStancebuttonsArcane, stanceName);
    StanceDeactivated(m_userStancebuttonsDivine, stanceName);
    StanceDeactivated(m_userStancebuttonsMartial, stanceName);
    StanceDeactivated(m_userStancebuttonsPrimal, stanceName);
    StanceDeactivated(m_userStancebuttonsIconic, stanceName);
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
    RemoveFromWeaponGroup(effect);
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
    RemoveFromWeaponGroup(effect.m_effect);
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
    AddToWeaponGroup(effect);
}

void CStancesView::UpdateItemEffectRevoked(
        Character * charData,
        const std::string & itemName,
        const Effect & effect)
{
    UpdateSliders(effect, false);
    RemoveFromWeaponGroup(effect);
}

void CStancesView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // determine which stance, if any was clicked on.
    // if they did click on a stance either activate or deactivate it
    CFormView::OnLButtonDown(nFlags, point);
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
            pStance->Invalidate(true);
        }
        else
        {
            // show action is not available
            ::MessageBeep(0xFFFFFFFF);
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
    m_tooltip.SetStanceItem(*m_pCharacter, &item.GetStance(), item.NumStacks());
    m_tooltip.Show();
}

void CStancesView::DestroyStances(std::vector<CStanceButton *> & items)
{
    for (size_t i = 0; i < items.size(); ++i)
    {
        items[i]->DestroyWindow();
        delete items[i];
        items[i] = NULL;
    }
    items.clear();
}

void CStancesView::DestroyAllStances()
{
    DestroyStances(m_userStancebuttons);
    DestroyStances(m_userStancebuttonsArcane);
    DestroyStances(m_userStancebuttonsDivine);
    DestroyStances(m_userStancebuttonsMartial);
    DestroyStances(m_userStancebuttonsPrimal);
    DestroyStances(m_userStancebuttonsIconic);
    DestroyStances(m_autoStancebuttons);
    m_nextStanceId = IDC_SPECIALFEAT_0;

    std::list<SliderItem>::iterator it = m_sliders.begin();
    while (it != m_sliders.end())
    {
        // time for this slider to disappear
        (*it).m_label->DestroyWindow();
        (*it).m_slider->DestroyWindow();
        it = m_sliders.erase(it);
    }
}

void CStancesView::AddActiveStances(
        const std::vector<CStanceButton *> & items,
        std::vector<CStanceButton *> * stances) const
{
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (items[i]->IsSelected())
        {
            stances->push_back(items[i]);
        }
    }
}

const std::vector<CStanceButton *> & CStancesView::ActiveUserStances() const
{
    static std::vector<CStanceButton *> activeStances;
    activeStances.clear();
    AddActiveStances(m_userStancebuttons, &activeStances);
    AddActiveStances(m_userStancebuttonsArcane, &activeStances);
    AddActiveStances(m_userStancebuttonsDivine, &activeStances);
    AddActiveStances(m_userStancebuttonsMartial, &activeStances);
    AddActiveStances(m_userStancebuttonsPrimal, &activeStances);
    AddActiveStances(m_userStancebuttonsIconic, &activeStances);
    return activeStances;
}

const std::vector<CStanceButton *> & CStancesView::ActiveAutoStances() const
{
    static std::vector<CStanceButton *> activeStances;
    activeStances.clear();
    AddActiveStances(m_autoStancebuttons, &activeStances);
    return activeStances;
}

void CStancesView::GetStance(
        const std::string & stanceName,
        const std::vector<CStanceButton *> & items,
        const CStanceButton ** pButton) const
{
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (items[i]->GetStance().Name() == stanceName)
        {
            *pButton = items[i];
        }
    }
}

const CStanceButton * CStancesView::GetStance(const std::string & stanceName) const
{
    const CStanceButton * pButton = NULL;
    GetStance(stanceName, m_userStancebuttons, &pButton);
    GetStance(stanceName, m_userStancebuttonsArcane, &pButton);
    GetStance(stanceName, m_userStancebuttonsDivine, &pButton);
    GetStance(stanceName, m_userStancebuttonsMartial, &pButton);
    GetStance(stanceName, m_userStancebuttonsPrimal, &pButton);
    GetStance(stanceName, m_userStancebuttonsIconic, &pButton);
    GetStance(stanceName, m_autoStancebuttons, &pButton);
    return pButton;
}

void CStancesView::OnHScroll(UINT sbCode, UINT nPos, CScrollBar * pScrollbar)
{
    if (m_pCharacter != NULL
            && pScrollbar != NULL)  // must not be the window scroll bar
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
        (*it).m_position = (int)effect.Amount();
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

const SliderItem * CStancesView::GetSlider(
        const std::string & name) const
{
    const SliderItem * pSlider = NULL;
    bool bFound = false;
    std::list<SliderItem>::const_iterator it = m_sliders.begin();
    while (!bFound && it != m_sliders.end())
    {
        if ((*it).m_name == name)
        {
            bFound = true;  // this is the droid we're looking for
            pSlider = &(*it);
        }
        else
        {
            ++it;
        }
    }
    return pSlider;
}

bool CStancesView::IsStanceActive(const std::string & name, WeaponType wt) const
{
    bool bEnabled = false;
    // some special stances are based on a slider position
    // all these stances start with a numeric with the format of
    // "<number>% <stance name>"
    size_t percentPos = name.find("%");
    if (percentPos != std::string::npos
            && percentPos + 2 <= name.length()) // don't crash on badly formatted stance name
    {
        // this is a special stance
        int value = atoi(name.c_str());
        // identify the slider stance in question
        std::string stanceName = name.substr(name.find('%') + 2, 50);
        const SliderItem * pSlider = GetSlider(stanceName);
        if (pSlider != NULL)
        {
            // if the value is negative its an under comparison else its an over
            if (value < 0)
            {
                value = -value;
                bEnabled = (pSlider->m_position < value);
            }
            else
            {
                bEnabled = (pSlider->m_position >= value);
            }
        }
    }
    // see if a weapon group exists for this stance
    std::list<WeaponGroup>::const_iterator wgit = m_weaponGroups.begin();
    while (wgit != m_weaponGroups.end())
    {
        if ((*wgit).Name() == name)
        {
            // yes, this weapon group exists, see if the weapon
            // is part of it.
            if ((*wgit).HasWeapon(wt))
            {
                bEnabled = true;
            }
        }
        ++wgit;
    }
    return bEnabled;
}

void CStancesView::AddToWeaponGroup(const Effect & effect)
{
    if (effect.Type() == Effect_AddGroupWeapon)
    {
        // see if the weapon group already exists. If so, add the list of weapons to it
        // else create the weapon group and add the weapons to it
        std::list<WeaponGroup>::iterator wgit = m_weaponGroups.begin();
        while (wgit != m_weaponGroups.end()
                && (*wgit).Name() != effect.WeaponGroup())
        {
            ++wgit;
        }
        std::list<WeaponType> weaponsToAdd = effect.Weapon();
        if (wgit != m_weaponGroups.end())
        {
            // already exists, add weapons to it
            std::list<WeaponType>::iterator wit = weaponsToAdd.begin();
            while (wit != weaponsToAdd.end())
            {
                (*wgit).AddWeapon((*wit));
                ++wit;
            }
        }
        else
        {
            // need to create new weapon group
            WeaponGroup wg(effect.WeaponGroup());
            std::list<WeaponType>::iterator wit = weaponsToAdd.begin();
            while (wit != weaponsToAdd.end())
            {
                wg.AddWeapon((*wit));
                ++wit;
            }
            m_weaponGroups.push_back(wg);
        }
        m_pCharacter->NotifyStanceActivated(effect.WeaponGroup());
    }
}

void CStancesView::RemoveFromWeaponGroup(const Effect & effect)
{
    if (effect.Type() == Effect_AddGroupWeapon)
    {
        std::list<WeaponGroup>::iterator wgit = m_weaponGroups.begin();
        while (wgit != m_weaponGroups.end()
                && (*wgit).Name() != effect.WeaponGroup())
        {
            ++wgit;
        }
        std::list<WeaponType> weaponsToRemove = effect.Weapon();
        if (wgit != m_weaponGroups.end())
        {
            // already exists, remove weapons from it
            std::list<WeaponType>::iterator wit = weaponsToRemove.begin();
            while (wit != weaponsToRemove.end())
            {
                (*wgit).RemoveWeapon((*wit));
                ++wit;
            }
            // its ok if the group is now empty
        }
        else
        {
            // can't remove from a weapon group that does not exist
        }
    }
}
