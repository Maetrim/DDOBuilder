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
        InventorySlotType slot) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_weaponType(weaponType),
    m_baseDamage(Breakdown_WeaponBaseDamage, Effect_WeaponBaseDamageBonus, "Base Damage", treeList, NULL),
    m_attackBonus(Breakdown_WeaponAttackBonus, Effect_AttackBonus, "Attack Bonus", treeList, NULL, this, slot),
    m_damageBonus(Breakdown_WeaponDamageBonus, Effect_DamageBonus, "Damage Bonus", treeList, NULL),
    m_criticalAttackBonus(Breakdown_WeaponAttackBonus, Effect_CriticalAttackBonus, "Critical Attack Bonus", treeList, NULL, this, slot),
    m_criticalDamageBonus(Breakdown_WeaponDamageBonus, Effect_Seeker, "Critical Damage Bonus", treeList, NULL),
    m_criticalThreatRange(Breakdown_WeaponCriticalThreatRange, treeList, NULL),
    //m_attackSpeed(Breakdown_WeaponAttackSpeed, Effect_AttackSpeed, "Attack Speed", treeList, NULL),
    m_centeredCount(0)     // assume not centered with this weapon
{
    m_baseDamage.SetWeapon(weaponType);
    m_attackBonus.SetWeapon(weaponType);
    m_damageBonus.SetWeapon(weaponType);
    //m_vorpalRange.SetWeapon(weaponType);
    m_criticalThreatRange.SetWeapon(weaponType);
    m_criticalAttackBonus.SetWeapon(weaponType);
    m_criticalDamageBonus.SetWeapon(weaponType);
    //m_criticalMultiplier.SetWeapon(weaponType);
    //m_specialMultiplier.SetWeapon(weaponType);
    //m_attackSpeed.SetWeapon(weaponType);

    //m_baseDamage.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));
    m_damageBonus.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));
    //m_vorpalRange.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));
    //m_criticalAttackBonus.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));
    m_criticalDamageBonus.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));
    //m_criticalMultiplier.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));
    //m_specialMultiplier.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));
    //m_attackSpeed.SetIsMeleeWeapon(IsMeleeWeapon(weaponType));      // not set for attack speed

    // we need to update if any of our sub-items update also
    m_baseDamage.AttachObserver(this);
    m_attackBonus.AttachObserver(this);
    m_damageBonus.AttachObserver(this);
    //m_vorpalRange.AttachObserver(this);
    m_criticalThreatRange.AttachObserver(this);
    m_criticalAttackBonus.AttachObserver(this);
    m_criticalDamageBonus.AttachObserver(this);
    //m_criticalMultiplier.AttachObserver(this);
    //m_specialMultiplier.AttachObserver(this);
    //m_attackSpeed.AttachObserver(this);

    AddTreeItem("Base Damage", &m_baseDamage);
    AddTreeItem("Attack Bonus", &m_attackBonus);
    AddTreeItem("Damage Bonus", &m_damageBonus);
    AddTreeItem("Critical Attack Bonus", &m_criticalAttackBonus);
    AddTreeItem("Critical Damage Bonus", &m_criticalDamageBonus);
    AddTreeItem("Critical Threat Range", &m_criticalThreatRange);
    //AddTreeItem("Attack Speed", &m_attackSpeed);

    // by default all melee weapons use strength as their base for attack bonus
    // additional abilities can be added for specific weapons by enhancements
    if (IsThrownWeapon(weaponType))
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
    if (!IsRangedWeapon(weaponType) && !IsThrownWeapon(weaponType))
    {
        m_damageBonus.AddAbility(Ability_Strength);
        m_criticalDamageBonus.AddAbility(Ability_Strength);
    }
}

BreakdownItemWeapon::~BreakdownItemWeapon()
{
}

void BreakdownItemWeapon::AddTreeItem(
        const std::string & entry,
        BreakdownItem * pBreakdown)
{
    HTREEITEM hItem = m_pTreeList->InsertItem(
            entry.c_str(),
            m_hItem,
            TVI_LAST);
    m_pTreeList->SetItemData(hItem, (DWORD)(void*)pBreakdown);
    pBreakdown->SetHTreeItem(hItem);
}

bool BreakdownItemWeapon::IsCentering() const
{
    bool isCentering = false;
    if (m_pCharacter != NULL)
    {
        isCentering = (m_centeredCount > 0)
            || (m_pCharacter->IsFocusWeapon(m_weaponType)
                && m_pCharacter->IsEnhancementTrained("KenseiOneWithTheBlade", ""));
    }
    return isCentering;
}

void BreakdownItemWeapon::SetCharacter(Character * charData, bool observe)
{
    BreakdownItem::SetCharacter(charData, observe);
    m_baseDamage.SetCharacter(charData, false);        // we handle this for them
    m_attackBonus.SetCharacter(charData, false);        // we handle this for them
    m_damageBonus.SetCharacter(charData, false);        // we handle this for them
    //m_vorpalRange.SetCharacter(charData, false);        // we handle this for them
    m_criticalThreatRange.SetCharacter(charData, false);        // we handle this for them
    m_criticalAttackBonus.SetCharacter(charData, false);        // we handle this for them
    m_criticalDamageBonus.SetCharacter(charData, false);        // we handle this for them
    //m_criticalMultiplier.SetCharacter(charData, false);        // we handle this for them
    //m_specialMultiplier.SetCharacter(charData, false);        // we handle this for them
    //m_attackSpeed.SetCharacter(charData, false);
}

// required overrides
CString BreakdownItemWeapon::Title() const
{
    if (IsCentering())
    {
        return m_title + " (Centering)";
    }
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
        case WeaponClass_Martial:
            isUs = IsMartialWeapon(m_weaponType);
            break;
        case WeaponClass_Simple:
            isUs = IsSimpleWeapon(m_weaponType);
            break;
        case WeaponClass_Thrown:
            isUs = IsThrownWeapon(m_weaponType);
            break;
        case WeaponClass_Unarmed:
            isUs = (m_weaponType == Weapon_Handwraps);
            break;
        case WeaponClass_OneHanded:
            isUs = IsOneHandedWeapon(m_weaponType);
            break;
        case WeaponClass_Ranged:
            isUs = IsRangedWeapon(m_weaponType);
            break;
        case WeaponClass_TwoHanded:
            isUs = IsTwoHandedWeapon(m_weaponType);
            break;
        case WeaponClass_Axe:
            isUs = IsAxe(m_weaponType);
            break;
        case WeaponClass_Bows:
            isUs = IsBow(m_weaponType);
            break;
        case WeaponClass_Crossbows:
            isUs = IsCrossbow(m_weaponType);
            break;
        case WeaponClass_ReapeatingCrossbows:
            isUs = IsRepeatingCrossbow(m_weaponType);
            break;
        case WeaponClass_Melee:
            isUs = IsMeleeWeapon(m_weaponType);
            break;
        case WeaponClass_Light:
            isUs = IsLightWeapon(m_weaponType);
            break;
        case WeaponClass_Finesseable:
            isUs = IsFinesseableWeapon(m_weaponType);
            break;
        case WeaponClass_FocusGroup:
            // always affects weapon, but disabled if weapon not part of focus group
            // done this way as weapons in a given focus group can vary with enhancements
            isUs = true;
            break;
        default:
            ASSERT(FALSE);  // not implemented this one? Do it!
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
        m_baseDamage.UpdateFeatEffect(pCharacter, featName, effect);
        m_attackBonus.UpdateFeatEffect(pCharacter, featName, effect);
        m_damageBonus.UpdateFeatEffect(pCharacter, featName, effect);
        //m_vorpalRange.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalThreatRange.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalAttackBonus.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalDamageBonus.UpdateFeatEffect(pCharacter, featName, effect);
        //m_criticalMultiplier.UpdateFeatEffect(pCharacter, featName, effect);
        //m_specialMultiplier.UpdateFeatEffect(pCharacter, featName, effect);
        //m_attackSpeed.UpdateFeatEffect(pCharacter, featName, effect);

        // we handle whether we are centered or not
        if (effect.Type() == Effect_CenteredWeapon)
        {
            if (effect.HasWeaponClass())
            {
                if (pCharacter->IsFocusWeapon(m_weaponType))
                {
                    ++m_centeredCount;
                    Populate();
                }
            }
            else if (effect.HasWeapon())
            {
                if (effect.Weapon() == m_weaponType)
                {
                    ++m_centeredCount;
                    Populate();
                }
            }
        }
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
        m_baseDamage.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_attackBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_damageBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_vorpalRange.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalThreatRange.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalAttackBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalDamageBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_criticalMultiplier.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_specialMultiplier.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_attackSpeed.UpdateFeatEffectRevoked(pCharacter, featName, effect);

        // we handle whether we are centered or not
        // we handle whether we are centered or not
        if (effect.Type() == Effect_CenteredWeapon)
        {
            if (effect.HasWeaponClass())
            {
                if (pCharacter->IsFocusWeapon(m_weaponType))
                {
                    --m_centeredCount;
                    Populate();
                }
            }
            else if (effect.HasWeapon())
            {
                if (effect.Weapon() == m_weaponType)
                {
                    --m_centeredCount;
                    Populate();
                }
            }
        }
    }
}

void BreakdownItemWeapon::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        // pass through to all our sub breakdowns
        m_baseDamage.UpdateItemEffect(pCharacter, itemName, effect);
        m_attackBonus.UpdateItemEffect(pCharacter, itemName, effect);
        m_damageBonus.UpdateItemEffect(pCharacter, itemName, effect);
        //m_vorpalRange.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalThreatRange.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalAttackBonus.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalDamageBonus.UpdateItemEffect(pCharacter, itemName, effect);
        //m_criticalMultiplier.UpdateItemEffect(pCharacter, itemName, effect);
        //m_specialMultiplier.UpdateItemEffect(pCharacter, itemName, effect);
        //m_attackSpeed.UpdateItemEffect(pCharacter, itemName, effect);

        // we handle whether we are centered or not
        // we handle whether we are centered or not
        if (effect.Type() == Effect_CenteredWeapon)
        {
            if (effect.HasWeaponClass())
            {
                if (pCharacter->IsFocusWeapon(m_weaponType))
                {
                    ++m_centeredCount;
                    Populate();
                }
            }
            else if (effect.HasWeapon())
            {
                if (effect.Weapon() == m_weaponType)
                {
                    ++m_centeredCount;
                    Populate();
                }
            }
        }
    }
}

void BreakdownItemWeapon::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        // pass through to all our sub breakdowns
        m_baseDamage.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_attackBonus.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_damageBonus.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        //m_vorpalRange.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalThreatRange.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalAttackBonus.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalDamageBonus.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        //m_criticalMultiplier.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        //m_specialMultiplier.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        //m_attackSpeed.UpdateItemEffectRevoked(pCharacter, itemName, effect);

        // we handle whether we are centered or not
        // we handle whether we are centered or not
        if (effect.Type() == Effect_CenteredWeapon)
        {
            if (effect.HasWeaponClass())
            {
                if (pCharacter->IsFocusWeapon(m_weaponType))
                {
                    --m_centeredCount;
                    Populate();
                }
            }
            else if (effect.HasWeapon())
            {
                if (effect.Weapon() == m_weaponType)
                {
                    --m_centeredCount;
                    Populate();
                }
            }
        }
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
        m_baseDamage.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_attackBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_damageBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_vorpalRange.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalThreatRange.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalAttackBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalDamageBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_criticalMultiplier.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_specialMultiplier.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_attackSpeed.UpdateEnhancementEffect(pCharacter, enhancementName, effect);

        // we handle whether we are centered or not
        if (effect.m_effect.Type() == Effect_CenteredWeapon)
        {
            if (effect.m_effect.HasWeaponClass())
            {
                if (pCharacter->IsFocusWeapon(m_weaponType))
                {
                    ++m_centeredCount;
                    Populate();
                }
            }
            else if (effect.m_effect.HasWeapon())
            {
                if (effect.m_effect.Weapon() == m_weaponType)
                {
                    ++m_centeredCount;
                    Populate();
                }
            }
        }
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
        m_baseDamage.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_attackBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_damageBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_vorpalRange.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalThreatRange.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalAttackBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalDamageBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_criticalMultiplier.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_specialMultiplier.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_attackSpeed.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);

        // we handle whether we are centered or not
        if (effect.m_effect.Type() == Effect_CenteredWeapon)
        {
            if (effect.m_effect.HasWeaponClass())
            {
                if (pCharacter->IsFocusWeapon(m_weaponType))
                {
                    --m_centeredCount;
                    Populate();
                }
            }
            else if (effect.m_effect.HasWeapon())
            {
                if (effect.m_effect.Weapon() == m_weaponType)
                {
                    --m_centeredCount;
                    Populate();
                }
            }
        }
    }
}

void BreakdownItemWeapon::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}

bool BreakdownItemWeapon::IsDamageType(WeaponDamageType type) const
{
    bool isUs = false;
    switch (m_weaponType)
    {
    // bludgeoning weapons
    case Weapon_Club:
    case Weapon_GreatClub:
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

void BreakdownItemWeapon::UpdateEnhancementTrained(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    BreakdownItem::UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_attackBonus.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_damageBonus.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    //m_vorpalRange.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalThreatRange.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalAttackBonus.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalDamageBonus.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    //m_criticalMultiplier.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    //m_specialMultiplier.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    //m_attackSpeed.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    if (enhancementName == "KenseiOneWithTheBlade"
            || enhancementName == "KenseiExoticWeaponMastery")
    {
        // these two enhancements can affect the centering state
        Populate();
    }
}

void BreakdownItemWeapon::UpdateEnhancementRevoked(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    BreakdownItem::UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_attackBonus.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_damageBonus.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    //m_vorpalRange.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalThreatRange.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalAttackBonus.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalDamageBonus.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    //m_criticalMultiplier.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    //m_specialMultiplier.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    //m_attackSpeed.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    if (enhancementName == "KenseiOneWithTheBlade"
            || enhancementName == "KenseiExoticWeaponMastery")
    {
        // these two enhancements can affect the centering state
        Populate();
    }
}

void BreakdownItemWeapon::UpdateFeatTrained(
        Character * charData,
        const std::string & featName)
{
    BreakdownItem::UpdateFeatTrained(charData, featName);
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateFeatTrained(charData, featName);
    m_attackBonus.UpdateFeatTrained(charData, featName);
    m_damageBonus.UpdateFeatTrained(charData, featName);
    //m_vorpalRange.UpdateFeatTrained(charData, featName);
    m_criticalThreatRange.UpdateFeatTrained(charData, featName);
    m_criticalAttackBonus.UpdateFeatTrained(charData, featName);
    m_criticalDamageBonus.UpdateFeatTrained(charData, featName);
    //m_criticalMultiplier.UpdateFeatTrained(charData, featName);
    //m_specialMultiplier.UpdateFeatTrained(charData, featName);
    //m_attackSpeed.UpdateFeatTrained(charData, featName);
}

void BreakdownItemWeapon::UpdateFeatRevoked(
        Character * charData,
        const std::string & featName)
{
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateFeatRevoked(charData, featName);
    m_attackBonus.UpdateFeatRevoked(charData, featName);
    m_damageBonus.UpdateFeatRevoked(charData, featName);
    //m_vorpalRange.UpdateFeatRevoked(charData, featName);
    m_criticalThreatRange.UpdateFeatRevoked(charData, featName);
    m_criticalAttackBonus.UpdateFeatRevoked(charData, featName);
    m_criticalDamageBonus.UpdateFeatRevoked(charData, featName);
    //m_criticalMultiplier.UpdateFeatRevoked(charData, featName);
    //m_specialMultiplier.UpdateFeatRevoked(charData, featName);
    //m_attackSpeed.UpdateFeatRevoked(charData, featName);
}
