// BreakdownItemEnergyResistance.cpp
//
#include "stdafx.h"
#include "BreakdownItemEnergyResistance.h"

BreakdownItemEnergyResistance::BreakdownItemEnergyResistance(
        BreakdownType type,
        EffectType effect,
        EnergyType ssType,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect),
    m_energyType(ssType)
{
    m_bAddEnergies = false; // change base class behavior
}

BreakdownItemEnergyResistance::~BreakdownItemEnergyResistance()
{
}

// required overrides
CString BreakdownItemEnergyResistance::Title() const
{
    return m_title;
}

CString BreakdownItemEnergyResistance::Value() const
{
    CString value;
    value.Format(
            "%3d",
            (int)Total());
    return value;
}

void BreakdownItemEnergyResistance::CreateOtherEffects()
{
    m_otherEffects.clear();
}

bool BreakdownItemEnergyResistance::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect
            && effect.IncludesEnergy(m_energyType))
    {
        isUs = true;
    }
    return isUs;
}
