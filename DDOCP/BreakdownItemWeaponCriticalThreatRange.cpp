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
            // from the wiki: This feat adds 1, 2, or 3 to critical threat
            // range based on the weapon type's unmodified threat range.
            // +3 for falchion, great crossbow, kukri, rapier, and scimitar.
            // +2 for bastard sword, dagger, greatsword, heavy crossbow, khopesh,
            // light crossbow, long sword, repeating heavy crossbow,
            // repeating light crossbow, short sword, and throwing dagger.
            // +1 to all other weapons.
            // Shields do not benefit from Improved Critical.
            double baseRange = 0 ;     // assume unknown
            switch (Weapon())
            {
            case Weapon_Falchion:
            case Weapon_GreatCrossbow:
            case Weapon_Kukri:
            case Weapon_Rapier:
            case Weapon_Scimitar:
                baseRange = +3;
                break;
            case Weapon_BastardSword:
            case Weapon_Dagger:
            case Weapon_GreatSword:
            case Weapon_HeavyCrossbow:
            case Weapon_Khopesh:
            case Weapon_LightCrossbow:
            case Weapon_Longsword:
            case Weapon_RepeatingHeavyCrossbow:
            case Weapon_RepeatingLightCrossbow:
            case Weapon_Shortsword:
            case Weapon_ThrowingDagger:
                baseRange = +2;
                break;
            case Weapon_ShieldBuckler:
            case Weapon_ShieldSmall:
            case Weapon_ShieldLarge:
            case Weapon_ShieldTower:
            case Weapon_Orb:
            case Weapon_RuneArm:
                baseRange = 0;
                break;
            default:
                // all other weapon types
                baseRange = +1;
                break;
            }
            if (baseRange > 0)
            {
                // we have an effect that doubles this weapons base critical range
                ActiveEffect keenEffect(
                        Bonus_keen,
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
    if (effect.Type() == Effect_CriticalRange
            || effect.Type() == Effect_Keen)
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
