// BreakdownItemWeaponDRBypass.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeaponDRBypass.h"
#include <algorithm>

BreakdownItemWeaponDRBypass::BreakdownItemWeaponDRBypass(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem)
{
}

BreakdownItemWeaponDRBypass::~BreakdownItemWeaponDRBypass()
{
}

// required overrides
CString BreakdownItemWeaponDRBypass::Title() const
{
    return "DR Bypass";
}

CString BreakdownItemWeaponDRBypass::Value() const
{
    std::vector<std::string> effects;

    // just append all the items together
    std::list<ActiveEffect>::const_iterator it = m_otherEffects.begin();
    while (it != m_otherEffects.end())
    {
        AddEffectToVector(&effects, (*it));
        ++it;
    }
    it = m_effects.begin();
    while (it != m_effects.end())
    {
        AddEffectToVector(&effects, (*it));
        ++it;
    }
    it = m_itemEffects.begin();
    while (it != m_itemEffects.end())
    {
        AddEffectToVector(&effects, (*it));
        ++it;
    }
    // now sort the effects alphabetically
    std::sort(effects.begin(), effects.end());
    // now remove duplicates
    for (size_t i = 1; i < effects.size(); ++i)
    {
        if (effects[i-1] == effects[i])
        {
            // this is a duplicate
            effects.erase(effects.begin() + i);
            i--;        // keep index correct
        }
    }
    // value is the concatenation of all the individual effects
    CString value;
    for (size_t i = 0; i < effects.size(); ++i)
    {
        if (i > 0)
        {
            value += ", ";
        }
        value += effects[i].c_str();
    }
    return value;
}

void BreakdownItemWeaponDRBypass::CreateOtherEffects()
{
    // no other effects for this simple item
    m_otherEffects.clear();
}

bool BreakdownItemWeaponDRBypass::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == Effect_DRBypass)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemWeaponDRBypass::AddEffectToVector(
        std::vector<std::string> * value,
        const ActiveEffect & effect) const
{
    value->push_back(effect.Description());
}
