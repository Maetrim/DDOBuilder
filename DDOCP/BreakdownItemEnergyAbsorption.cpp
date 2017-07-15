// BreakdownItemEnergyAbsorption.cpp
//
#include "stdafx.h"
#include "BreakdownItemEnergyAbsorption.h"

BreakdownItemEnergyAbsorption::BreakdownItemEnergyAbsorption(
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
}

BreakdownItemEnergyAbsorption::~BreakdownItemEnergyAbsorption()
{
}

// required overrides
CString BreakdownItemEnergyAbsorption::Title() const
{
    return m_title;
}

CString BreakdownItemEnergyAbsorption::Value() const
{
    CString value;
    value.Format(
            "%3d %%",
            (int)Total());
    return value;
}

void BreakdownItemEnergyAbsorption::CreateOtherEffects()
{
    m_otherEffects.clear();
}

bool BreakdownItemEnergyAbsorption::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect
            && (effect.Energy() == m_energyType
                || effect.Energy() == Energy_All))
    {
        isUs = true;
    }
    return isUs;
}
