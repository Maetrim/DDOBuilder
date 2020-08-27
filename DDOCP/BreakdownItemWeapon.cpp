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
        InventorySlotType slot,
        const Dice & damageDice,
        size_t weaponCriticalMultiplier) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_weaponType(weaponType),
    m_damageDice(damageDice),
    m_baseDamage(Breakdown_WeaponBaseDamage, Effect_WeaponBaseDamageBonus, "Base Damage", treeList, NULL),
    m_attackBonus(Breakdown_WeaponAttackBonus, Effect_AttackBonus, "Attack Bonus", treeList, NULL, this, slot),
    m_damageBonus(Breakdown_WeaponDamageBonus, Effect_DamageBonus, "Damage Bonus", treeList, NULL),
    m_otherDamageEffects(Breakdown_WeaponOtherDamageEffects, treeList, NULL),
    m_criticalAttackBonus(Breakdown_WeaponCriticalAttackBonus, Effect_CriticalAttackBonus, "Critical Attack Bonus", treeList, NULL, this, slot),
    m_criticalDamageBonus(Breakdown_WeaponCriticalDamageBonus, Effect_Seeker, "Critical Damage Bonus", treeList, NULL),
    m_otherCriticalDamageEffects(Breakdown_WeaponCriticalOtherDamageEffects, treeList, NULL),
    m_criticalThreatRange(Breakdown_WeaponCriticalThreatRange, treeList, NULL),
    m_criticalMultiplier(Breakdown_WeaponCriticalMultiplier,  treeList, NULL, NULL),
    m_criticalMultiplier19To20(Breakdown_WeaponCriticalMultiplier19To20, treeList, NULL, &m_criticalMultiplier),
    m_attackSpeed(Breakdown_WeaponAttackSpeed, Effect_Alacrity, "Attack Speed", treeList, NULL),
    m_drBypass(Breakdown_DRBypass, treeList, NULL),
    m_centeredCount(0),    // assume not centered with this weapon
    m_weaponCriticalMuliplier(weaponCriticalMultiplier)
{
    m_baseDamage.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_attackBonus.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_damageBonus.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_otherDamageEffects.SetWeapon(weaponType, weaponCriticalMultiplier);
    //m_vorpalRange.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_criticalThreatRange.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_criticalAttackBonus.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_criticalDamageBonus.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_otherCriticalDamageEffects.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_criticalMultiplier.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_criticalMultiplier19To20.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_attackSpeed.SetWeapon(weaponType, weaponCriticalMultiplier);
    m_drBypass.SetWeapon(weaponType, weaponCriticalMultiplier);

    if (slot == Inventory_Weapon2)
    {
        // track ability bonus at 50% for off hand attacks
        m_damageBonus.SetIsOffHand(true);
    }

    // we need to update if any of our sub-items update also
    m_baseDamage.AttachObserver(this);
    m_attackBonus.AttachObserver(this);
    m_damageBonus.AttachObserver(this);
    m_otherDamageEffects.AttachObserver(this);
    //m_vorpalRange.AttachObserver(this);
    m_criticalThreatRange.AttachObserver(this);
    m_criticalAttackBonus.AttachObserver(this);
    m_criticalDamageBonus.AttachObserver(this);
    m_otherCriticalDamageEffects.AttachObserver(this);
    m_criticalMultiplier.AttachObserver(this);
    m_criticalMultiplier19To20.AttachObserver(this);
    m_attackSpeed.AttachObserver(this);
    m_drBypass.AttachObserver(this);

    std::string strDamageDice = m_damageDice.Description(1);

    AddTreeItem("Weapon Dice", strDamageDice, NULL);           // no breakdown, just a dice number
    AddTreeItem("Base Damage", "", &m_baseDamage);
    AddTreeItem("Attack Bonus", "", &m_attackBonus);
    AddTreeItem("Damage Bonus", "", &m_damageBonus);
    AddTreeItem("Other Damage Effects", "", &m_otherDamageEffects);
    AddTreeItem("Critical Attack Bonus", "", &m_criticalAttackBonus);
    AddTreeItem("Critical Damage Bonus", "", &m_criticalDamageBonus);
    AddTreeItem("Other Critical Damage Effects", "", &m_otherCriticalDamageEffects);
    AddTreeItem("Critical Threat Range", "", &m_criticalThreatRange);
    AddTreeItem("Critical Multiplier", "", &m_criticalMultiplier);
    AddTreeItem("Critical Multiplier (19-20)", "", &m_criticalMultiplier19To20);
    AddTreeItem("Attack Speed", "", &m_attackSpeed);
    AddTreeItem("DR Bypass", "", &m_drBypass);
}

BreakdownItemWeapon::~BreakdownItemWeapon()
{
}

void BreakdownItemWeapon::AddTreeItem(
        const std::string & entry,
        const std::string & total,
        BreakdownItem * pBreakdown)
{
    HTREEITEM hItem = m_pTreeList->InsertItem(
            entry.c_str(),
            m_hItem,
            TVI_LAST);
    m_pTreeList->SetItemText(hItem, 1, total.c_str());
    m_pTreeList->SetItemData(hItem, (DWORD)(void*)pBreakdown);
    if (pBreakdown != NULL)
    {
        pBreakdown->SetHTreeItem(hItem);
    }
}

bool BreakdownItemWeapon::IsCentering() const
{
    bool isCentering = false;
    if (m_pCharacter != NULL)
    {
        isCentering = (m_centeredCount > 0)
            || (m_pCharacter->IsFocusWeapon(m_weaponType)
                && m_pCharacter->IsEnhancementTrained("KenseiOneWithTheBlade", "", TT_enhancement));
    }
    return isCentering;
}

void BreakdownItemWeapon::SetCharacter(Character * charData, bool observe)
{
    BreakdownItem::SetCharacter(charData, observe);
    m_baseDamage.SetCharacter(charData, false);        // we handle this for them
    m_attackBonus.SetCharacter(charData, false);        // we handle this for them
    m_damageBonus.SetCharacter(charData, false);        // we handle this for them
    m_otherDamageEffects.SetCharacter(charData, false);
    //m_vorpalRange.SetCharacter(charData, false);        // we handle this for them
    m_criticalThreatRange.SetCharacter(charData, false);        // we handle this for them
    m_criticalAttackBonus.SetCharacter(charData, false);        // we handle this for them
    m_criticalDamageBonus.SetCharacter(charData, false);        // we handle this for them
    m_otherCriticalDamageEffects.SetCharacter(charData, false);        // we handle this for them
    m_criticalMultiplier.SetCharacter(charData, false);        // we handle this for them
    m_criticalMultiplier19To20.SetCharacter(charData, false);        // we handle this for them
    m_attackSpeed.SetCharacter(charData, false);
    m_drBypass.SetCharacter(charData, false);
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
    CString valueRegular;
    // regular hit value
    valueRegular.Format(
            "%.2f[%s]+%d %s",
            m_baseDamage.Total(),
            m_damageDice.Description(1).c_str(),
            (int)m_damageBonus.Total(),
            m_otherDamageEffects.Value());
    // critical hit value
    CString valueCrit;
    valueCrit.Format(
            "%s (%.2f[%s]+%d) * %d %s",
            m_criticalThreatRange.Value(),
            m_baseDamage.Total(),
            m_damageDice.Description(1).c_str(),
            (int)m_criticalDamageBonus.Total(),
            (int)m_criticalMultiplier.Total(),
            m_otherCriticalDamageEffects.Value());
    valueCrit.Replace("-20", "-18");
    CString valueCrit1920;
    valueCrit1920.Format(
            "19-20 (%.2f[%s]+%d) * %d %s",
            m_baseDamage.Total(),
            m_damageDice.Description(1).c_str(),
            (int)m_criticalDamageBonus.Total(),
            (int)m_criticalMultiplier19To20.Total(),
            m_otherCriticalDamageEffects.Value());
    CString value;
    value.Format("%s; %s; %s", valueRegular, valueCrit, valueCrit1920);
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
    if (effect.IncludesWeapon(m_weaponType))
    {
        isUs = true;
    }
    if (effect.HasWeaponClass())
    {
        isUs = IsInWeaponClass(effect.WeaponClass(), m_weaponType);
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
        m_otherDamageEffects.UpdateFeatEffect(pCharacter, featName, effect);
        //m_vorpalRange.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalThreatRange.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalAttackBonus.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalDamageBonus.UpdateFeatEffect(pCharacter, featName, effect);
        m_otherCriticalDamageEffects.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalMultiplier.UpdateFeatEffect(pCharacter, featName, effect);
        m_criticalMultiplier19To20.UpdateFeatEffect(pCharacter, featName, effect);
        m_attackSpeed.UpdateFeatEffect(pCharacter, featName, effect);
        m_drBypass.UpdateFeatEffect(pCharacter, featName, effect);

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
            else if (effect.IncludesWeapon(m_weaponType))
            {
                ++m_centeredCount;
                Populate();
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
        m_otherDamageEffects.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        //m_vorpalRange.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalThreatRange.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalAttackBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalDamageBonus.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_otherCriticalDamageEffects.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalMultiplier.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_criticalMultiplier19To20.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_attackSpeed.UpdateFeatEffectRevoked(pCharacter, featName, effect);
        m_drBypass.UpdateFeatEffectRevoked(pCharacter, featName, effect);

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
            else if (effect.IncludesWeapon(m_weaponType))
            {
                --m_centeredCount;
                Populate();
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
        m_otherDamageEffects.UpdateItemEffect(pCharacter, itemName, effect);
        //m_vorpalRange.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalThreatRange.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalAttackBonus.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalDamageBonus.UpdateItemEffect(pCharacter, itemName, effect);
        m_otherCriticalDamageEffects.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalMultiplier.UpdateItemEffect(pCharacter, itemName, effect);
        m_criticalMultiplier19To20.UpdateItemEffect(pCharacter, itemName, effect);
        m_attackSpeed.UpdateItemEffect(pCharacter, itemName, effect);
        m_drBypass.UpdateItemEffect(pCharacter, itemName, effect);

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
            else if (effect.IncludesWeapon(m_weaponType))
            {
                ++m_centeredCount;
                Populate();
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
        m_otherDamageEffects.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        //m_vorpalRange.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalThreatRange.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalAttackBonus.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalDamageBonus.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_otherCriticalDamageEffects.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalMultiplier.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_criticalMultiplier19To20.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_attackSpeed.UpdateItemEffectRevoked(pCharacter, itemName, effect);
        m_drBypass.UpdateItemEffectRevoked(pCharacter, itemName, effect);

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
            else if (effect.IncludesWeapon(m_weaponType))
            {
                --m_centeredCount;
                Populate();
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
        m_otherDamageEffects.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        //m_vorpalRange.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalThreatRange.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalAttackBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalDamageBonus.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_otherCriticalDamageEffects.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalMultiplier.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_criticalMultiplier19To20.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_attackSpeed.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        m_drBypass.UpdateEnhancementEffect(pCharacter, enhancementName, effect);

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
            else if (effect.m_effect.IncludesWeapon(m_weaponType))
            {
                ++m_centeredCount;
                Populate();
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
        m_otherDamageEffects.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        //m_vorpalRange.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalThreatRange.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalAttackBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalDamageBonus.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_otherCriticalDamageEffects.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalMultiplier.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_criticalMultiplier19To20.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_attackSpeed.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        m_drBypass.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);

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
            else if (effect.m_effect.IncludesWeapon(m_weaponType))
            {
                --m_centeredCount;
                Populate();
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
    Populate();
}

bool BreakdownItemWeapon::IsDamageType(WeaponDamageType type) const
{
    bool isUs = false;
    switch (type)
    {
    case WeaponDamage_Bludgeoning:
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
            case Weapon_Warhammer:
                isUs = true;
                break;
        }
        break;
    case WeaponDamage_Slashing:
        switch (m_weaponType)
        {
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
                isUs = true;
                break;
        }
        break;
    case WeaponDamage_Piercing:
        switch (m_weaponType)
        {
            // piercing weapons
            case Weapon_Dagger:
            case Weapon_HeavyPick:
            case Weapon_LightPick:
            case Weapon_Rapier:
            case Weapon_Shortsword:
                isUs = true;
                break;
        }
        break;
    case WeaponDamage_Ranged:
        switch (m_weaponType)
        {
            // ranged weapons
            case Weapon_GreatCrossbow:
            case Weapon_HeavyCrossbow:
            case Weapon_LightCrossbow:
            case Weapon_Longbow:
            case Weapon_RepeatingHeavyCrossbow:
            case Weapon_RepeatingLightCrossbow:
            case Weapon_Shortbow:
                isUs = true;
                break;
        }
        break;
    case WeaponDamage_Thrown:
        switch (m_weaponType)
        {
            // thrown weapons
            case Weapon_Dart:
            case Weapon_ThrowingAxe:
            case Weapon_ThrowingDagger:
            case Weapon_Shuriken:
            case Weapon_ThrowingHammer:
                isUs = true;
                break;
        }
        break;
    }
    // special types that are not specific weapons but work the same
    //Weapon_EldritchBlast,
    //Weapon_Shield,
    return isUs;
}

void BreakdownItemWeapon::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateClassChanged(charData, classFrom, classTo, level);
    m_attackBonus.UpdateClassChanged(charData, classFrom, classTo, level);
    m_damageBonus.UpdateClassChanged(charData, classFrom, classTo, level);
    m_otherDamageEffects.UpdateClassChanged(charData, classFrom, classTo, level);
    //m_vorpalRange.UpdateClassChanged(charData, classFrom, classTo, level);
    m_criticalThreatRange.UpdateClassChanged(charData, classFrom, classTo, level);
    m_criticalAttackBonus.UpdateClassChanged(charData, classFrom, classTo, level);
    m_criticalDamageBonus.UpdateClassChanged(charData, classFrom, classTo, level);
    m_otherCriticalDamageEffects.UpdateClassChanged(charData, classFrom, classTo, level);
    m_criticalMultiplier.UpdateClassChanged(charData, classFrom, classTo, level);
    m_criticalMultiplier19To20.UpdateClassChanged(charData, classFrom, classTo, level);
    m_attackSpeed.UpdateClassChanged(charData, classFrom, classTo, level);
    m_drBypass.UpdateClassChanged(charData, classFrom, classTo, level);
}

void BreakdownItemWeapon::UpdateAPSpentInTreeChanged(
        Character * charData,
        const std::string & treeName)
{
    BreakdownItem::UpdateAPSpentInTreeChanged(charData, treeName);
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateAPSpentInTreeChanged(charData, treeName);
    m_attackBonus.UpdateAPSpentInTreeChanged(charData, treeName);
    m_damageBonus.UpdateAPSpentInTreeChanged(charData, treeName);
    m_otherDamageEffects.UpdateAPSpentInTreeChanged(charData, treeName);
    //m_vorpalRange.UpdateAPSpentInTreeChanged(charData, treeName);
    m_criticalThreatRange.UpdateAPSpentInTreeChanged(charData, treeName);
    m_criticalAttackBonus.UpdateAPSpentInTreeChanged(charData, treeName);
    m_criticalDamageBonus.UpdateAPSpentInTreeChanged(charData, treeName);
    m_otherCriticalDamageEffects.UpdateAPSpentInTreeChanged(charData, treeName);
    m_criticalMultiplier.UpdateAPSpentInTreeChanged(charData, treeName);
    m_criticalMultiplier19To20.UpdateAPSpentInTreeChanged(charData, treeName);
    m_attackSpeed.UpdateAPSpentInTreeChanged(charData, treeName);
    m_drBypass.UpdateAPSpentInTreeChanged(charData, treeName);
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
    m_otherDamageEffects.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    //m_vorpalRange.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalThreatRange.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalAttackBonus.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalDamageBonus.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_otherCriticalDamageEffects.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalMultiplier.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_criticalMultiplier19To20.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_attackSpeed.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
    m_drBypass.UpdateEnhancementTrained(charData, enhancementName, selection, isTier5);
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
    m_otherDamageEffects.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    //m_vorpalRange.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalThreatRange.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalAttackBonus.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalDamageBonus.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_otherCriticalDamageEffects.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalMultiplier.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_criticalMultiplier19To20.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_attackSpeed.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
    m_drBypass.UpdateEnhancementRevoked(charData, enhancementName, selection, isTier5);
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
    m_otherDamageEffects.UpdateFeatTrained(charData, featName);
    //m_vorpalRange.UpdateFeatTrained(charData, featName);
    m_criticalThreatRange.UpdateFeatTrained(charData, featName);
    m_criticalAttackBonus.UpdateFeatTrained(charData, featName);
    m_criticalDamageBonus.UpdateFeatTrained(charData, featName);
    m_otherCriticalDamageEffects.UpdateFeatTrained(charData, featName);
    m_criticalMultiplier.UpdateFeatTrained(charData, featName);
    m_criticalMultiplier19To20.UpdateFeatTrained(charData, featName);
    m_attackSpeed.UpdateFeatTrained(charData, featName);
    m_drBypass.UpdateFeatTrained(charData, featName);
}

void BreakdownItemWeapon::UpdateFeatRevoked(
        Character * charData,
        const std::string & featName)
{
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateFeatRevoked(charData, featName);
    m_attackBonus.UpdateFeatRevoked(charData, featName);
    m_damageBonus.UpdateFeatRevoked(charData, featName);
    m_otherDamageEffects.UpdateFeatRevoked(charData, featName);
    //m_vorpalRange.UpdateFeatRevoked(charData, featName);
    m_criticalThreatRange.UpdateFeatRevoked(charData, featName);
    m_criticalAttackBonus.UpdateFeatRevoked(charData, featName);
    m_criticalDamageBonus.UpdateFeatRevoked(charData, featName);
    m_otherCriticalDamageEffects.UpdateFeatRevoked(charData, featName);
    m_criticalMultiplier.UpdateFeatRevoked(charData, featName);
    m_criticalMultiplier19To20.UpdateFeatRevoked(charData, featName);
    m_attackSpeed.UpdateFeatRevoked(charData, featName);
    m_drBypass.UpdateFeatRevoked(charData, featName);
}

void BreakdownItemWeapon::UpdateStanceActivated(
        Character * charData,
        const std::string & stanceName)
{
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateStanceActivated(charData, stanceName);
    m_attackBonus.UpdateStanceActivated(charData, stanceName);
    m_damageBonus.UpdateStanceActivated(charData, stanceName);
    m_otherDamageEffects.UpdateStanceActivated(charData, stanceName);
    //m_vorpalRange.UpdateStanceActivated(charData, stanceName);
    m_criticalThreatRange.UpdateStanceActivated(charData, stanceName);
    m_criticalAttackBonus.UpdateStanceActivated(charData, stanceName);
    m_criticalDamageBonus.UpdateStanceActivated(charData, stanceName);
    m_otherCriticalDamageEffects.UpdateStanceActivated(charData, stanceName);
    m_criticalMultiplier.UpdateStanceActivated(charData, stanceName);
    m_criticalMultiplier19To20.UpdateStanceActivated(charData, stanceName);
    m_attackSpeed.UpdateStanceActivated(charData, stanceName);
    m_drBypass.UpdateStanceActivated(charData, stanceName);
}

void BreakdownItemWeapon::UpdateStanceDeactivated(
        Character * charData,
        const std::string & stanceName)
{
    // pass through to all our sub breakdowns
    m_baseDamage.UpdateStanceDeactivated(charData, stanceName);
    m_attackBonus.UpdateStanceDeactivated(charData, stanceName);
    m_damageBonus.UpdateStanceDeactivated(charData, stanceName);
    m_otherDamageEffects.UpdateStanceDeactivated(charData, stanceName);
    //m_vorpalRange.UpdateStanceDeactivated(charData, stanceName);
    m_criticalThreatRange.UpdateStanceDeactivated(charData, stanceName);
    m_criticalAttackBonus.UpdateStanceDeactivated(charData, stanceName);
    m_criticalDamageBonus.UpdateStanceDeactivated(charData, stanceName);
    m_otherCriticalDamageEffects.UpdateStanceDeactivated(charData, stanceName);
    m_criticalMultiplier.UpdateStanceDeactivated(charData, stanceName);
    m_criticalMultiplier19To20.UpdateStanceDeactivated(charData, stanceName);
    m_attackSpeed.UpdateStanceDeactivated(charData, stanceName);
    m_drBypass.UpdateStanceDeactivated(charData, stanceName);
}

void BreakdownItemWeapon::AddForumExportData(std::stringstream & forumExport)
{
    forumExport << Title() << "\r\n";
    CString valueRegular;
    // regular hit value
    valueRegular.Format(
            "On Hit         %.2f[%s]+%d %s\r\n",
            m_baseDamage.Total(),
            m_damageDice.Description(1).c_str(),
            (int)m_damageBonus.Total(),
            m_otherDamageEffects.Value());
    forumExport << valueRegular;
    // critical hit value
    CString valueCrit;
    valueCrit.Format(
            "Critical %s (%.2f[%s]+%d) * %d %s\r\n",
            m_criticalThreatRange.Value(),
            m_baseDamage.Total(),
            m_damageDice.Description(1).c_str(),
            (int)m_criticalDamageBonus.Total(),
            (int)m_criticalMultiplier.Total(),
            m_otherCriticalDamageEffects.Value());
    valueCrit.Replace("-20", "-18");
    forumExport << valueCrit;
    CString valueCrit1920;
    valueCrit1920.Format(
            "Critical 19-20 (%.2f[%s]+%d) * %d %s\r\n",
            m_baseDamage.Total(),
            m_damageDice.Description(1).c_str(),
            (int)m_criticalDamageBonus.Total(),
            (int)m_criticalMultiplier19To20.Total(),
            m_otherCriticalDamageEffects.Value());
    forumExport << valueCrit1920;
    forumExport << "DR Bypass: " << m_drBypass.Value();
    forumExport << "\r\n\r\n";
}

BreakdownItem * BreakdownItemWeapon::GetWeaponBreakdown(BreakdownType bt)
{
    BreakdownItem * pBI = NULL;
    switch (bt)
    {
    case Breakdown_WeaponBaseDamage:
        pBI = &m_baseDamage;
        break;
    case Breakdown_WeaponAttackBonus:
        pBI = &m_attackBonus;
        break;
    case Breakdown_WeaponDamageBonus:
        pBI = &m_damageBonus;
        break;
    case Breakdown_WeaponOtherDamageEffects:
        pBI = &m_otherDamageEffects;
        break;
    case Breakdown_WeaponCriticalAttackBonus:
        pBI = &m_criticalAttackBonus;
        break;
    case Breakdown_WeaponCriticalDamageBonus:
        pBI = &m_criticalDamageBonus;
        break;
    case Breakdown_WeaponCriticalOtherDamageEffects:
        pBI = &m_otherCriticalDamageEffects;
        break;
    case Breakdown_WeaponCriticalThreatRange:
        pBI = &m_criticalThreatRange;
        break;
    case Breakdown_WeaponCriticalMultiplier:
        pBI = &m_criticalMultiplier;
        break;
    case Breakdown_WeaponCriticalMultiplier19To20:
        pBI = &m_criticalMultiplier19To20;
        break;
    case Breakdown_WeaponAttackSpeed:
        pBI = &m_attackSpeed;
        break;
    case Breakdown_DRBypass:
        pBI = &m_drBypass;
        break;
    }
    return pBI;
}

