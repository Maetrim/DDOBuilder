// BreakdownItemDodge.cpp
//
#include "stdafx.h"
#include "BreakdownItemDodge.h"
#include "GlobalSupportFunctions.h"

BreakdownItemDodge::BreakdownItemDodge(
        BreakdownType type,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title)
{
}

BreakdownItemDodge::~BreakdownItemDodge()
{
}

// required overrides
CString BreakdownItemDodge::Title() const
{
    return m_title;
}

double BreakdownItemDodge::CappedTotal() const
{
    // dodge can be capped from 3 different sources:
    // 1. You basic Dodge Cap which is 25% plus any effects that increase it
    // 2. The MDB of any armor you are wearing
    // 3. The MDB when a Tower Shield is equipped
    BreakdownItem * pBI = FindBreakdown(Breakdown_DodgeCap);
    double dodgeCap = pBI->Total();
    pBI = FindBreakdown(Breakdown_MaxDexBonus);
    double maxDodgeBonusArmor = pBI->Total();
    pBI = FindBreakdown(Breakdown_MaxDexBonusShields);
    double maxDodgeBonusTowerShields = pBI->Total();

    double dodge = (int)Total();        // current dodge
    double displayDodge = dodge;
    if (dodge > dodgeCap)
    {
        displayDodge = min(displayDodge, dodgeCap);
    }
    // MDB cap applies when not in cloth armor
    if (m_pCharacter != NULL
            && !m_pCharacter->IsStanceActive("Cloth Armor")
            && dodge > maxDodgeBonusArmor)
    {
        displayDodge = min(displayDodge, maxDodgeBonusArmor);
    }
    // max dex tower shield applies when a tower shield is equipped
    if (m_pCharacter != NULL
            && m_pCharacter->IsStanceActive("Tower Shield")
            && dodge > maxDodgeBonusTowerShields)
    {
        displayDodge = min(displayDodge, maxDodgeBonusTowerShields);
    }
    return displayDodge;
}

CString BreakdownItemDodge::Value() const
{
    // dodge can be capped from 3 different sources:
    // 1. You basic Dodge Cap which is 25% plus any effects that increase it
    // 2. The MDB of any armor you are wearing
    // 3. The MDB when a Tower Shield is equipped
    BreakdownItem * pBI = FindBreakdown(Breakdown_DodgeCap);
    double dodgeCap = pBI->Total();
    pBI = FindBreakdown(Breakdown_MaxDexBonus);
    double maxDodgeBonusArmor = pBI->Total();
    pBI = FindBreakdown(Breakdown_MaxDexBonusShields);
    double maxDodgeBonusTowerShields = pBI->Total();

    double dodge = (int)Total();        // current dodge
    double displayDodge = dodge;
    bool capped = false;                // assume no cap
    if (dodge > dodgeCap)
    {
        displayDodge = min(displayDodge, dodgeCap);
        capped = true;
    }
    // MDB cap applies when not in cloth armor
    if (m_pCharacter != NULL
            && !m_pCharacter->IsStanceActive("Cloth Armor")
            && dodge > maxDodgeBonusArmor)
    {
        displayDodge = min(displayDodge, maxDodgeBonusArmor);
        capped = true;
    }
    // max dex tower shield applies when a tower shield is equipped
    if (m_pCharacter != NULL
            && m_pCharacter->IsStanceActive("Tower Shield")
            && dodge > maxDodgeBonusTowerShields)
    {
        displayDodge = min(displayDodge, maxDodgeBonusTowerShields);
        capped = true;
    }
    CString value;
    if (capped)
    {
        value.Format(
                "%.0f (Capped from %.0f)",
                displayDodge,
                dodge);
    }
    else
    {
        value.Format("%.0f", displayDodge);
    }
    return value;
}

void BreakdownItemDodge::CreateOtherEffects()
{
    m_otherEffects.clear();
    // we need to know when these breakdown values changes
    BreakdownItem * pBI = FindBreakdown(Breakdown_DodgeCap);
    pBI->AttachObserver(this);
    pBI = FindBreakdown(Breakdown_MaxDexBonus);
    pBI->AttachObserver(this);
    pBI = FindBreakdown(Breakdown_MaxDexBonusShields);
    pBI->AttachObserver(this);
}

bool BreakdownItemDodge::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == Effect_DodgeBonus)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemDodge::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    BreakdownItem::UpdateTotalChanged(item, type);  // call base
    Populate();     // things that may cap us have changed
}
