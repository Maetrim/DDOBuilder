// BreakdownItemWeaponOtherDamageEffects.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeaponOtherDamageEffects.h"

BreakdownItemWeaponOtherDamageEffects::BreakdownItemWeaponOtherDamageEffects(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem)
{
}

BreakdownItemWeaponOtherDamageEffects::~BreakdownItemWeaponOtherDamageEffects()
{
}

// required overrides
CString BreakdownItemWeaponOtherDamageEffects::Title() const
{
    if (Type() == Breakdown_WeaponOtherDamageEffects)
    {
        return "Other Damage Effects";
    }
    else
    {
        return "Other Critical Damage Effects";
    }
}

CString BreakdownItemWeaponOtherDamageEffects::Value() const
{
    // value is the concatenation of all the individual effects
    CString value;
    // just append all the items together, each should be an effect
    // which as a dice item
    std::list<ActiveEffect>::const_iterator it = m_otherEffects.begin();
    while (it != m_otherEffects.end())
    {
        if ((*it).TotalAmount(false) != 0)
        {
            AddEffectToString(&value, (*it));
        }
        ++it;
    }
    it = m_effects.begin();
    while (it != m_effects.end())
    {
        if ((*it).TotalAmount(false) != 0)
        {
            AddEffectToString(&value, (*it));
        }
        ++it;
    }
    it = m_itemEffects.begin();
    while (it != m_itemEffects.end())
    {
        if ((*it).TotalAmount(false) != 0)
        {
            AddEffectToString(&value, (*it));
        }
        ++it;
    }
    return value;
}

void BreakdownItemWeaponOtherDamageEffects::CreateOtherEffects()
{
    // no other effects for this simple item
    m_otherEffects.clear();
}

bool BreakdownItemWeaponOtherDamageEffects::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (Type() == Breakdown_WeaponOtherDamageEffects
            && effect.Type() == Effect_WeaponOtherDamageBonus)
    {
        isUs = true;
    }
    if (Type() == Breakdown_WeaponCriticalOtherDamageEffects
            && effect.Type() == Effect_WeaponOtherCriticalDamageBonus)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemWeaponOtherDamageEffects::AddEffectToString(
        CString * value,
        const ActiveEffect & effect) const
{
    (*value) += " + ";
    (*value) += effect.Description().c_str();
}
