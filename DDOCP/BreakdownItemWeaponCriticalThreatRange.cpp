// BreakdownItemWeaponCriticalThreatRange.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeaponCriticalThreatRange.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemWeaponCriticalThreatRange::BreakdownItemWeaponCriticalThreatRange(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_keenCount(0)
{
}

BreakdownItemWeaponCriticalThreatRange::~BreakdownItemWeaponCriticalThreatRange()
{
}

// required overrides
CString BreakdownItemWeaponCriticalThreatRange::Title() const
{
    return "Critical Threat Range";
}

CString BreakdownItemWeaponCriticalThreatRange::Value() const
{
    CString value;
    int total = (int)Total();
    if (total > 1)
    {
        value.Format("%d-20", 21 - total);  // e.g. 19-20
    }
    else
    {
        value = "20";
    }
    return value;
}

void BreakdownItemWeaponCriticalThreatRange::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        if (m_keenCount > 0)
        {
            // look through the item effects list for the base critical range
            double baseRange = 0 ;     // assume unknown
            std::list<ActiveEffect>::const_iterator it = m_itemEffects.begin();
            while (it != m_itemEffects.end())
            {
                if ((*it).Bonus() == Bonus_base)
                {
                    baseRange = (*it).TotalAmount(false);
                }
                ++it;
            }
            if (baseRange > 0)
            {
                // we have an effect that doubles this weapons base critical range
                ActiveEffect keenEffect(
                        Bonus_base,
                        "Keen weapon",
                        1,
                        baseRange,
                        "");        // no tree
                AddOtherEffect(keenEffect);
            }
        }
    }
}

bool BreakdownItemWeaponCriticalThreatRange::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == Effect_CriticalRange)
    {
        // if its the right effect its for us as our holder class determines whether
        // it is the right weapon target type
        isUs = true;
    }
    return isUs;
}

void BreakdownItemWeaponCriticalThreatRange::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // pass through to the base class
    BreakdownItem::UpdateFeatEffect(pCharacter, featName, effect);
    if (effect.Type() == Effect_Keen)
    {
        ++m_keenCount;
        CreateOtherEffects();
    }
}

void BreakdownItemWeaponCriticalThreatRange::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // pass through to the base class
    BreakdownItem::UpdateFeatEffectRevoked(pCharacter, featName, effect);
    if (effect.Type() == Effect_Keen)
    {
        --m_keenCount;
        CreateOtherEffects();
    }
}

void BreakdownItemWeaponCriticalThreatRange::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // pass through to the base class
    BreakdownItem::UpdateItemEffect(pCharacter, itemName, effect);
    if (effect.Type() == Effect_Keen)
    {
        ++m_keenCount;
        CreateOtherEffects();
    }
}

void BreakdownItemWeaponCriticalThreatRange::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // pass through to the base class
    BreakdownItem::UpdateItemEffectRevoked(pCharacter, itemName, effect);
    if (effect.Type() == Effect_Keen)
    {
        --m_keenCount;
        CreateOtherEffects();
    }
}

void BreakdownItemWeaponCriticalThreatRange::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // pass through to the base class
    BreakdownItem::UpdateEnhancementEffect(pCharacter, enhancementName, effect);
    if (effect.m_effect.Type() == Effect_Keen)
    {
        ++m_keenCount;
        CreateOtherEffects();
    }
}

void BreakdownItemWeaponCriticalThreatRange::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // pass through to the base class
    BreakdownItem::UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
    if (effect.m_effect.Type() == Effect_Keen)
    {
        --m_keenCount;
        CreateOtherEffects();
    }
}
