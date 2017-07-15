// BreakdownItemWeapon.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeapon.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemWeapon::BreakdownItemWeapon(
        BreakdownType type,
        WeaponType weaponType,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem,
        const std::vector<HTREEITEM> & hSubItems) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_weaponType(weaponType),
    m_attackBonus(Breakdown_WeaponAttackBonus, Effect_AttackBonus, "Attack Bonus", treeList, hSubItems[0]),
    m_damageBonus(Breakdown_WeaponDamageBonus, Effect_DamageBonus, "Damage Bonus", treeList, hSubItems[1]),
    m_criticalAttackBonus(Breakdown_WeaponAttackBonus, Effect_CriticalAttackBonus, "Critical Attack Bonus", treeList, hSubItems[2]),
    m_criticalDamageBonus(Breakdown_WeaponDamageBonus, Effect_CriticalDamageBonus, "Critical Damage Bonus", treeList, hSubItems[3]),
    m_attackSpeed(Breakdown_WeaponAttackSpeed, Effect_AttackSpeed, "Attack Speed", treeList, hSubItems[4])
{
    // we need to update if any of our sub-items update also
    //m_baseDamage.AttachObserver(this);
    m_attackBonus.AttachObserver(this);
    m_damageBonus.AttachObserver(this);
    //m_vorpalRange.AttachObserver(this);
    //m_criticalThreatRange.AttachObserver(this);
    m_criticalAttackBonus.AttachObserver(this);
    m_criticalDamageBonus.AttachObserver(this);
    //m_criticalMultiplier.AttachObserver(this);
    //m_specialMultiplier.AttachObserver(this);
    m_attackSpeed.AttachObserver(this);

    treeList->SetItemData(hSubItems[0], (DWORD)(void*)&m_attackBonus);
    treeList->SetItemData(hSubItems[1], (DWORD)(void*)&m_damageBonus);
    //treeList->SetItemData(hSubItems[4], (DWORD)(void*)&m_vorpalRange);
    //treeList->SetItemData(hSubItems[4], (DWORD)(void*)&m_criticalThreatRange);
    treeList->SetItemData(hSubItems[2], (DWORD)(void*)&m_criticalAttackBonus);
    treeList->SetItemData(hSubItems[3], (DWORD)(void*)&m_criticalDamageBonus);
    //treeList->SetItemData(hSubItems[4], (DWORD)(void*)&m_criticalMultiplier);
    //treeList->SetItemData(hSubItems[4], (DWORD)(void*)&m_specialMultiplier);
    treeList->SetItemData(hSubItems[4], (DWORD)(void*)&m_attackSpeed);


    // by default all melee weapons use strength as their base for attack bonus
    // additional abilities can be added for specific weapons by enhancements
    if (IsThrownWeapon())
    {
        // thrown weapons use dexterity
        m_attackBonus.AddAbility(Ability_Dexterity);
        m_criticalAttackBonus.AddAbility(Ability_Dexterity);
    }
    else
    {
        m_attackBonus.AddAbility(Ability_Strength);
        m_criticalAttackBonus.AddAbility(Ability_Strength);
    }

    // by default all weapons except ranged/thrown weapons use strength as their base for bonus damage
    // additional abilities can be added for specific weapons by enhancements/feats
    // such as Bow Strength gives strength for ranged weapons
    if (!IsRangedWeapon() && !IsThrownWeapon())
    {
        m_damageBonus.AddAbility(Ability_Strength);
        m_criticalDamageBonus.AddAbility(Ability_Strength);
    }
}

BreakdownItemWeapon::~BreakdownItemWeapon()
{
}

void BreakdownItemWeapon::SetCharacter(Character * charData, bool observe)
{
    BreakdownItem::SetCharacter(charData, observe);
    m_attackBonus.SetCharacter(charData, false);        // we handle this for them
    m_damageBonus.SetCharacter(charData, false);        // we handle this for them
    //m_vorpalRange.SetCharacter(charData, false);        // we handle this for them
    //m_criticalThreatRange.SetCharacter(charData, false);        // we handle this for them
    m_criticalAttackBonus.SetCharacter(charData, false);        // we handle this for them
    m_criticalDamageBonus.SetCharacter(charData, false);        // we handle this for them
    //m_criticalMultiplier.SetCharacter(charData, false);        // we handle this for them
    //m_specialMultiplier.SetCharacter(charData, false);        // we handle this for them
    m_attackSpeed.SetCharacter(charData, false);
}

// required overrides
CString BreakdownItemWeapon::Title() const
{
    return m_title;
}

CString BreakdownItemWeapon::Value() const
{
    CString value;
    // this string should be a format such as
    // "2.5[2D6]+57, 2.5[2D6]+64(16-18), 4.0[2D6]+46(19-20)
    value = "TBD";
    return value;
}

void BreakdownItemWeapon::CreateOtherEffects()
{
    // does nothing in this holder class
}

bool BreakdownItemWeapon::AffectsUs(const Effect & effect) const
{
    // determine whether this effect is applied to this weapon
    bool isUs = false;
    if (effect.HasWeapon())
    {
        // weapon type specified explicitly
        if (effect.Weapon() == m_weaponType || effect.Weapon() == Weapon_All)
        {
            isUs = true;
        }
    }
    if (effect.HasWeaponClass())
    {
        // a class of weapons are affected, determine whether we fall into it
        switch (effect.WeaponClass())
        {
        case WeaponClass_Thrown:
            isUs = IsThrownWeapon();
            break;
        case WeaponClass_Unarmed:
            isUs = (m_weaponType == Weapon_Handwraps);
            break;
        case WeaponClass_OneHanded:
            isUs = IsOneHandedWeapon();
            break;
        case WeaponClass_Ranged:
            isUs = IsRangedWeapon();
            break;
        case WeaponClass_TwoHanded:
            isUs = IsTwoHandedWeapon();
            break;
        case WeaponClass_Bows:
            isUs = IsBow();
            break;
        case WeaponClass_Crossbows:
            isUs = IsCrossbow();
            break;
        case WeaponClass_ReapeatingCrossbows:
            isUs = IsRepeatingCrossbow();
            break;
        case WeaponClass_Melee:
            isUs = IsMeleeWeapon();
            break;
        case WeaponClass_Druidic:
            isUs = IsDruidicWeapon();
            break;
        case WeaponClass_HeavyBlades:
            isUs = IsHeavyBladeWeapon();
            break;
        case WeaponClass_LightBlades:
            isUs = IsLightBladeWeapon();
            break;
        case WeaponClass_PicksAndHammers:
            isUs = IsPickOrHammerWeapon();
            break;
        case WeaponClass_MacesAndClubs:
            isUs = IsMaceOrClubWeapon();
            break;
        case WeaponClass_MartialArts:
            isUs = IsMartialArtsWeapon();
            break;
        case WeaponClass_Axe:
            isUs = IsAxe();
            break;
        case WeaponClass_Light:
            isUs = IsLightWeapon();
            break;
        default:
            ASSERT(FALSE);  // no implemented this one? Do it!
        }
    }
    if (effect.HasDamageType())
    {
        isUs = IsDamageType(effect.DamageType());
    }
    return isUs;
}

void BreakdownItemWeapon::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        // pass through to all our sub breakdowns
        //m_baseDamage.UpdateFeatEffect(pCharacter, featName, effect);
        m_attackBonus.UpdateFeatEffect(pCharacter, featName, effect);
        m_damageBonus.UpdateFeatEffect(pCharacter, featName, effect);
        //m_vorpalRange.UpdateFeatEffect(pCharacter, featName, effect);
        //m_criticalThreatRange.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalAttackBonus.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalDamageBonus.UpdateFeatEffect(pCharacter, featName, effect);
        //m_criticalMultiplier.UpdateFeatEffect(pCharacter, featName, effect);
        //m_specialMultiplier.UpdateFeatEffect(pCharacter, featName, effect);
        m_attackSpeed.UpdateFeatEffect(pCharacter, featName, effect);
    }
}

void BreakdownItemWeapon::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        // pass through to all our sub breakdowns
        //m_baseDamage.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_attackBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_damageBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_vorpalRange.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_criticalThreatRange.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalAttackBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalDamageBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_criticalMultiplier.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_specialMultiplier.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_attackSpeed.UpdateFeatEffectRevoked(pCharacter, featName, effect);
    }
}

void BreakdownItemWeapon::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        // pass through to all our sub breakdowns
        //m_baseDamage.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_attackBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_damageBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_vorpalRange.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_criticalThreatRange.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalAttackBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalDamageBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_criticalMultiplier.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_specialMultiplier.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_attackSpeed.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
    }
}

void BreakdownItemWeapon::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        // pass through to all our sub breakdowns
        //m_baseDamage.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_attackBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_damageBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_vorpalRange.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_criticalThreatRange.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalAttackBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalDamageBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_criticalMultiplier.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_specialMultiplier.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_attackSpeed.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
    }
}

void BreakdownItemWeapon::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}

// Weapon class filters
bool BreakdownItemWeapon::IsThrownWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Dart:
    case Weapon_Shuriken:
    case Weapon_ThrowingAxe:
    case Weapon_ThrowingDagger:
    case Weapon_ThrowingHammer:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsOneHandedWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_BastardSword:
    case Weapon_BattleAxe:
    case Weapon_Club:
    case Weapon_Dagger:
    case Weapon_DwarvenAxe:
    case Weapon_HandAxe:
    case Weapon_HeavyMace:
    case Weapon_HeavyPick:
    case Weapon_Kama:
    case Weapon_Khopesh:
    case Weapon_Kukri:
    case Weapon_LightHammer:
    case Weapon_LightMace:
    case Weapon_LightPick:
    case Weapon_Longsword:
    case Weapon_Morningstar:
    case Weapon_Rapier:
    case Weapon_Scimitar:
    case Weapon_Shortsword:
    case Weapon_Sickle:
    case Weapon_Warhammer:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsRangedWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Dart:
    case Weapon_GreatCrossbow:
    case Weapon_HeavyCrossbow:
    case Weapon_LightCrossbow:
    case Weapon_Longbow:
    case Weapon_RepeatingHeavyCrossbow:
    case Weapon_RepeatingLightCrossbow:
    case Weapon_Shortbow:
    case Weapon_Shuriken:
    case Weapon_ThrowingAxe:
    case Weapon_ThrowingDagger:
    case Weapon_ThrowingHammer:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsTwoHandedWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Falchion:
    case Weapon_GreatAxe:
    case Weapon_GreateClub:
    case Weapon_GreatSword:
    case Weapon_Maul:
    case Weapon_Quarterstaff:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsBow() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Longbow:
    case Weapon_Shortbow:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsCrossbow() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_GreatCrossbow:
    case Weapon_RepeatingHeavyCrossbow:
    case Weapon_RepeatingLightCrossbow:
        // TBD: special case for great/repeating
    case Weapon_HeavyCrossbow:
    case Weapon_LightCrossbow:
       isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsRepeatingCrossbow() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_RepeatingHeavyCrossbow:
    case Weapon_RepeatingLightCrossbow:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsMeleeWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_BastardSword:
    case Weapon_BattleAxe:
    case Weapon_Club:
    case Weapon_Dagger:
    case Weapon_Dart:
    case Weapon_DwarvenAxe:
    case Weapon_Falchion:
    case Weapon_GreatAxe:
    case Weapon_GreateClub:
    case Weapon_GreatSword:
    case Weapon_HandAxe:
    case Weapon_Handwraps:
    case Weapon_HeavyMace:
    case Weapon_HeavyPick:
    case Weapon_Kama:
    case Weapon_Khopesh:
    case Weapon_Kukri:
    case Weapon_LightHammer:
    case Weapon_LightMace:
    case Weapon_LightPick:
    case Weapon_Longsword:
    case Weapon_Maul:
    case Weapon_Morningstar:
    case Weapon_Quarterstaff:
    case Weapon_Rapier:
    case Weapon_Scimitar:
    case Weapon_Shortsword:
    case Weapon_Sickle:
    case Weapon_Warhammer:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsDruidicWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Club:
    case Weapon_Dagger:
    case Weapon_Dart:
    case Weapon_Handwraps:
    case Weapon_Quarterstaff:
    case Weapon_Scimitar:
    case Weapon_Sickle:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsHeavyBladeWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_BastardSword:
    case Weapon_Khopesh:
        // TBD: these are only available if relevant exotic weapon enhancement taken
    case Weapon_Falchion:
    case Weapon_GreatSword:
    case Weapon_Longsword:
    case Weapon_Scimitar:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsLightBladeWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Dagger:
    case Weapon_Kukri:
    case Weapon_Rapier:
    case Weapon_Shortsword:
    case Weapon_ThrowingDagger:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsPickOrHammerWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_HeavyPick:
    case Weapon_LightHammer:
    case Weapon_LightPick:
    case Weapon_Maul:
    case Weapon_ThrowingHammer:
    case Weapon_Warhammer:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsMaceOrClubWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Club:
    case Weapon_GreateClub:
    case Weapon_HeavyMace:
    case Weapon_LightMace:
    case Weapon_Morningstar:
    case Weapon_Quarterstaff:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsMartialArtsWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_Handwraps:
    case Weapon_Kama:
    case Weapon_Quarterstaff:
    case Weapon_Shuriken:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsAxe() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    case Weapon_DwarvenAxe:
        // TBD: special case for Dwarven axe
    case Weapon_BattleAxe:
    case Weapon_GreatAxe:
    case Weapon_HandAxe:
    case Weapon_ThrowingAxe:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsLightWeapon() const
{
    bool isUs = false;
    switch (m_weaponType)
    {
        // TBD: this list not complete
    case Weapon_LightHammer:
    case Weapon_LightMace:
    case Weapon_LightPick:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeapon::IsDamageType(WeaponDamageType type) const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    // bludgeoning weapons
    case Weapon_Club:
    case Weapon_GreateClub:
    case Weapon_Handwraps:
    case Weapon_HeavyMace:
    case Weapon_LightHammer:
    case Weapon_LightMace:
    case Weapon_Maul:
    case Weapon_Morningstar:
    case Weapon_Quarterstaff:
    case Weapon_Shuriken:
    case Weapon_ThrowingHammer:
    case Weapon_Warhammer:
        isUs = (type == WeaponDamage_Bludgeoning);
        break;
    // slashing weapons
    case Weapon_BastardSword:
    case Weapon_BattleAxe:
    case Weapon_DwarvenAxe:
    case Weapon_Falchion:
    case Weapon_GreatAxe:
    case Weapon_GreatSword:
    case Weapon_HandAxe:
    case Weapon_Kama:
    case Weapon_Khopesh:
    case Weapon_Kukri:
    case Weapon_Longsword:
    case Weapon_Scimitar:
    case Weapon_Sickle:
    case Weapon_ThrowingAxe:
        isUs = (type == WeaponDamage_Slashing);
        break;
    // piercing weapons
    case Weapon_Dagger:
    case Weapon_Dart:
    case Weapon_GreatCrossbow:
    case Weapon_HeavyCrossbow:
    case Weapon_HeavyPick:
    case Weapon_LightCrossbow:
    case Weapon_LightPick:
    case Weapon_Longbow:
    case Weapon_Rapier:
    case Weapon_RepeatingHeavyCrossbow:
    case Weapon_RepeatingLightCrossbow:
    case Weapon_Shortbow:
    case Weapon_Shortsword:
    case Weapon_ThrowingDagger:
        isUs = (type == WeaponDamage_Piercing);
        break;
    }
    // special types that are not specific weapons but work the same
    //Weapon_EldritchBlast,
    //Weapon_Shield,
    return isUs;
}

