// BreakdownItemSpellPower.cpp
//
#include "stdafx.h"
#include "BreakdownItemSpellPower.h"

BreakdownItemSpellPower::BreakdownItemSpellPower(
        BreakdownType type,
        EffectType effect,
        SpellPowerType spType,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect),
    m_spellPowerType(spType)
{
}

BreakdownItemSpellPower::~BreakdownItemSpellPower()
{
}

// required overrides
CString BreakdownItemSpellPower::Title() const
{
    return m_title;
}

CString BreakdownItemSpellPower::Value() const
{
    CString value;

    value.Format(
            "%.2f",             // spell power can have fractional values to 2 dp
            Total());

    return value;
}

void BreakdownItemSpellPower::CreateOtherEffects()
{
    m_otherEffects.clear();
}

bool BreakdownItemSpellPower::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect
            && (effect.SpellPower() == m_spellPowerType
                || effect.SpellPower() == SpellPower_All))
    {
        isUs = true;
    }
    return isUs;
}

