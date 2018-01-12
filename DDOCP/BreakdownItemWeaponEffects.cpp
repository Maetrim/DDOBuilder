// BreakdownItemWeaponEffects.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeaponEffects.h"

#include "BreakdownsView.h"
#include "BreakdownItemWeapon.h"
#include "GlobalSupportFunctions.h"

BreakdownItemWeaponEffects::BreakdownItemWeaponEffects(
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(Breakdown_WeaponEffectHolder, NULL, NULL),
    m_pTreeList(treeList),  // not passed to base class
    m_hItem(hItem),         // not passed to base class
    m_pMainHandWeapon(NULL),
    m_pOffHandWeapon(NULL)
{
    // ensure the vector is the correct size
    m_weaponFeatEffects.resize(Weapon_Count);
    m_weaponItemEffects.resize(Weapon_Count);
    m_weaponEnhancementEffects.resize(Weapon_Count);
}

BreakdownItemWeaponEffects::~BreakdownItemWeaponEffects()
{
    delete m_pMainHandWeapon;
    m_pMainHandWeapon = NULL;
    delete m_pOffHandWeapon;
    m_pOffHandWeapon = NULL;
}

void BreakdownItemWeaponEffects::SetCharacter(Character * charData, bool observe)
{
    BreakdownItem::SetCharacter(charData, observe);
}

// required overrides
CString BreakdownItemWeaponEffects::Title() const
{
    CString title("This item not displayed");
    return title;
}

CString BreakdownItemWeaponEffects::Value() const
{
    return "Not displayed";
}

void BreakdownItemWeaponEffects::CreateOtherEffects()
{
    // does nothing in this holder class
}

bool BreakdownItemWeaponEffects::AffectsUs(const Effect & effect) const
{
    // affects us if its a weapon effect
    bool isUs = false;
    if (effect.HasWeapon())
    {
        isUs = true;
    }
    if (effect.HasWeaponClass())
    {
        isUs = true;
    }
    if (effect.HasDamageType())
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemWeaponEffects::AddToAffectedWeapons(
        std::vector<std::list<Effect> > * list,
        const Effect & effect)
{
    // see which sub-weapons (if any) this effect applies to
    // if it add it to the list
    for (size_t i = Weapon_Unknown; i < Weapon_Count; ++i)
    {
        WeaponType wt = (WeaponType)i;
        bool affectsWeapon = AffectsThisWeapon(wt, effect);
        if (affectsWeapon)
        {
            (*list)[i].push_back(effect);
        }
    }
}

void BreakdownItemWeaponEffects::AddToAffectedWeapons(
        std::vector<std::list<EffectTier> > * list,
        const EffectTier & effect)
{
    // see which sub-weapons (if any) this effect applies to
    // if it add it to the list
    for (size_t i = Weapon_Unknown; i < Weapon_Count; ++i)
    {
        WeaponType wt = (WeaponType)i;
        bool affectsWeapon = AffectsThisWeapon(wt, effect.m_effect);
        if (affectsWeapon)
        {
            (*list)[i].push_back(effect);
        }
    }
}

void BreakdownItemWeaponEffects::RemoveFromAffectedWeapons(
        std::vector<std::list<Effect> > * list,
        const Effect & effect)
{
    // see which sub-weapons (if any) this effect applies to
    // if it affects remove it from the list
    for (size_t i = Weapon_Unknown; i < Weapon_Count; ++i)
    {
        WeaponType wt = (WeaponType)i;
        bool affectsWeapon = AffectsThisWeapon(wt, effect);
        // find in the list and remove
        if (affectsWeapon)
        {
            std::list<Effect>::iterator it = (*list)[i].begin();
            while (it != (*list)[i].end())
            {
                if ((*it) == effect)
                {
                    // this is it
                    it = (*list)[i].erase(it);
                    break; // and were done
                }
                ++it;
            }
        }
    }
}

void BreakdownItemWeaponEffects::RemoveFromAffectedWeapons(
        std::vector<std::list<EffectTier> > * list,
        const EffectTier & effect)
{
    // see which sub-weapons (if any) this effect applies to
    // if it affects remove it from the list
    for (size_t i = Weapon_Unknown; i < Weapon_Count; ++i)
    {
        WeaponType wt = (WeaponType)i;
        bool affectsWeapon = AffectsThisWeapon(wt, effect.m_effect);
        // find in the list and remove
        if (affectsWeapon)
        {
            std::list<EffectTier>::iterator it = (*list)[i].begin();
            while (it != (*list)[i].end())
            {
                if ((*it) == effect)
                {
                    // this is it
                    it = (*list)[i].erase(it);
                    break; // and were done
                }
                ++it;
            }
        }
    }
}

bool BreakdownItemWeaponEffects::AffectsThisWeapon(
        WeaponType wt,
        const Effect & effect)
{
    // determine whether this effect is applied to this weapon
    bool isUs = false;
    if (effect.HasWeapon())
    {
        // weapon type specified explicitly
        if (effect.Weapon() == wt
                || effect.Weapon() == Weapon_All)
        {
            isUs = true;
        }
    }
    if (effect.HasWeaponClass())
    {
        // a class of weapons are affected, determine whether
        // we fall into it
        switch (effect.WeaponClass())
        {
        case WeaponClass_Martial:
            isUs = IsMartialWeapon(wt);
            break;
        case WeaponClass_Simple:
            isUs = IsSimpleWeapon(wt);
            break;
        case WeaponClass_Thrown:
            isUs = IsThrownWeapon(wt);
            break;
        case WeaponClass_Unarmed:
            isUs = (wt == Weapon_Handwraps);
            break;
        case WeaponClass_OneHanded:
            isUs = IsOneHandedWeapon(wt);
            break;
        case WeaponClass_Ranged:
            isUs = IsRangedWeapon(wt);
            break;
        case WeaponClass_TwoHanded:
            isUs = IsTwoHandedWeapon(wt);
            break;
        case WeaponClass_Bows:
            isUs = IsBow(wt);
            break;
        case WeaponClass_Crossbows:
            isUs = IsCrossbow(wt);
            break;
        case WeaponClass_ReapeatingCrossbows:
            isUs = IsRepeatingCrossbow(wt);
            break;
        case WeaponClass_Melee:
            isUs = IsMeleeWeapon(wt);
            break;
        case WeaponClass_Druidic:
            isUs = IsDruidicWeapon(wt);
            break;
        case WeaponClass_HeavyBlades:
            isUs = IsHeavyBladeWeapon(wt);
            break;
        case WeaponClass_LightBlades:
            isUs = IsLightBladeWeapon(wt);
            break;
        case WeaponClass_PicksAndHammers:
            isUs = IsPickOrHammerWeapon(wt);
            break;
        case WeaponClass_MacesAndClubs:
            isUs = IsMaceOrClubWeapon(wt);
            break;
        case WeaponClass_MartialArts:
            isUs = IsMartialArtsWeapon(wt);
            break;
        case WeaponClass_Axe:
            isUs = IsAxe(wt);
            break;
        case WeaponClass_Light:
            isUs = IsLightWeapon(wt);
            break;
        default:
            ASSERT(FALSE);  // no implemented this one? Do it!
        }
    }
    if (effect.HasDamageType())
    {
        isUs = IsDamageType(wt, effect.DamageType());
    }
    return isUs;
}

void BreakdownItemWeaponEffects::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        Effect copy(effect);
        if (!copy.HasDisplayName())
        {
            copy.Set_DisplayName(featName);
        }
        AddToAffectedWeapons(&m_weaponFeatEffects, copy);
    }
}

void BreakdownItemWeaponEffects::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        Effect copy(effect);
        if (!copy.HasDisplayName())
        {
            copy.Set_DisplayName(featName);
        }
        RemoveFromAffectedWeapons(&m_weaponFeatEffects, copy);
    }
}

void BreakdownItemWeaponEffects::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        Effect copy(effect);
        if (!copy.HasDisplayName())
        {
            copy.Set_DisplayName(itemName);
        }
        AddToAffectedWeapons(&m_weaponItemEffects, copy);
    }
}

void BreakdownItemWeaponEffects::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        Effect copy(effect);
        if (!copy.HasDisplayName())
        {
            copy.Set_DisplayName(itemName);
        }
        RemoveFromAffectedWeapons(&m_weaponItemEffects, copy);
    }
}

void BreakdownItemWeaponEffects::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        EffectTier copy(effect);
        if (!copy.m_effect.HasDisplayName())
        {
            copy.m_effect.Set_DisplayName(enhancementName);
        }
        AddToAffectedWeapons(&m_weaponEnhancementEffects, copy);
    }
}

void BreakdownItemWeaponEffects::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        EffectTier copy(effect);
        if (!copy.m_effect.HasDisplayName())
        {
            copy.m_effect.Set_DisplayName(enhancementName);
        }
        RemoveFromAffectedWeapons(&m_weaponEnhancementEffects, copy);
    }
}

// Weapon class filters
bool BreakdownItemWeaponEffects::IsMartialWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_HandAxe:
    case Weapon_Kukri:
    case Weapon_LightHammer:
    case Weapon_LightPick:
    case Weapon_Shortsword:
    case Weapon_BattleAxe:
    case Weapon_HeavyPick:
    case Weapon_Longsword:
    case Weapon_Rapier:
    case Weapon_Scimitar:
    case Weapon_Warhammer:
    case Weapon_Falchion:
    case Weapon_GreatAxe:
    case Weapon_GreateClub:
    case Weapon_Maul:
    case Weapon_GreatSword:
    case Weapon_Shortbow:
    case Weapon_Longbow:
    case Weapon_ThrowingAxe:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeaponEffects::IsSimpleWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_Club:
    case Weapon_Dagger:
    case Weapon_Quarterstaff:
    case Weapon_LightMace:
    case Weapon_HeavyMace:
    case Weapon_Morningstar:
    case Weapon_Sickle:
    case Weapon_LightCrossbow:
    case Weapon_HeavyCrossbow:
    case Weapon_ThrowingDagger:
    case Weapon_Dart:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeaponEffects::IsThrownWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsOneHandedWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsRangedWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsTwoHandedWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsBow(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_Longbow:
    case Weapon_Shortbow:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeaponEffects::IsCrossbow(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_GreatCrossbow:
    case Weapon_RepeatingHeavyCrossbow:
    case Weapon_RepeatingLightCrossbow:
        if (m_pCharacter->IsEnhancementTrained("KenseiCore1", "Kensei Focus: Crossbows")
                && m_pCharacter->IsEnhancementTrained("KenseiExoticWeaponMastery", ""))
        {
            isUs = true;
        }
        break;
    case Weapon_HeavyCrossbow:
    case Weapon_LightCrossbow:
       isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeaponEffects::IsRepeatingCrossbow(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_RepeatingHeavyCrossbow:
    case Weapon_RepeatingLightCrossbow:
        isUs = true;
        break;
        // all other weapon types are not a match
    }
    return isUs;
}

bool BreakdownItemWeaponEffects::IsMeleeWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_BastardSword:
    case Weapon_BattleAxe:
    case Weapon_Club:
    case Weapon_Dagger:
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

bool BreakdownItemWeaponEffects::IsDruidicWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsHeavyBladeWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_BastardSword:
    case Weapon_Khopesh:
        if (m_pCharacter->IsEnhancementTrained("KenseiCore1", "Kensei Focus: Heavy Blades")
                && m_pCharacter->IsEnhancementTrained("KenseiExoticWeaponMastery", ""))
        {
            isUs = true;
        }
        break;
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

bool BreakdownItemWeaponEffects::IsLightBladeWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsPickOrHammerWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsMaceOrClubWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsMartialArtsWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsAxe(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
    {
    case Weapon_DwarvenAxe:
        // You get Dwarven axe if you are a dwarf or you have the enhancements
        // for the axe group as your focus weapons as a Kensei
        if (m_pCharacter->Race() == Race_Dwarf)
        {
            isUs = true;
        }
        if (m_pCharacter->IsEnhancementTrained("KenseiCore1", "Kensei Focus: Axes")
                && m_pCharacter->IsEnhancementTrained("KenseiExoticWeaponMastery", ""))
        {
            isUs = true;
        }
        break;
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

bool BreakdownItemWeaponEffects::IsLightWeapon(WeaponType wt) const
{
    bool isUs = false;
    switch (wt)
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

bool BreakdownItemWeaponEffects::IsDamageType(WeaponType wt, WeaponDamageType type) const
{
    bool isUs = false;
    switch (wt)
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

void BreakdownItemWeaponEffects::WeaponsChanged(const EquippedGear & gear)
{
    // first remove any previous weapons breakdowns (if present)
    m_pTreeList->DeleteSubItems(m_hItem);
    delete m_pMainHandWeapon;
    m_pMainHandWeapon = NULL;
    delete m_pOffHandWeapon;
    m_pOffHandWeapon = NULL;

    // now create the new weapon breakdowns (if required)
    if (gear.HasItemInSlot(Inventory_Weapon1))
    {
        m_pMainHandWeapon = CreateWeaponBreakdown(
                Breakdown_MainHand,
                gear.ItemInSlot(Inventory_Weapon1));
    }
    if (gear.HasItemInSlot(Inventory_Weapon2))
    {
        m_pOffHandWeapon = CreateWeaponBreakdown(
                Breakdown_OffHand,
                gear.ItemInSlot(Inventory_Weapon2));
    }
    m_pTreeList->Expand(m_hItem, TVE_EXPAND);
    m_pTreeList->RedrawWindow();    // ensure view updates
}

BreakdownItemWeapon * BreakdownItemWeaponEffects::CreateWeaponBreakdown(
        BreakdownType bt,
        const Item & item)
{
    BreakdownItemWeapon * pWeaponBreakdown = NULL;
    std::string name = item.Name();
    WeaponType wt = item.Weapon();
    HTREEITEM hItem = m_pTreeList->InsertItem(
            name.c_str(),
            m_hItem,
            TVI_LAST);
    pWeaponBreakdown = new BreakdownItemWeapon(
            bt,
            wt,
            name.c_str(),
            m_pTreeList,
            hItem,
            bt == Breakdown_MainHand
                ? Inventory_Weapon1
                : Inventory_Weapon2);
    m_pTreeList->SetItemData(hItem, (DWORD)(void*)pWeaponBreakdown);
    pWeaponBreakdown->SetCharacter(m_pCharacter, true);

    // first apply the Item specific effects only
    if (item.HasItemEffects())
    {
        const std::vector<Effect> & itemsEffects = item.ItemEffects().Effects();
        for (size_t i = 0; i < itemsEffects.size(); ++i)
        {
            pWeaponBreakdown->UpdateItemEffect(
                    m_pCharacter,
                    item.Name(),
                    itemsEffects[i]);
        }
    }
    // for this weapon, filter in all the cached effects
    std::list<Effect>::const_iterator it = m_weaponFeatEffects[wt].begin();
    while (it != m_weaponFeatEffects[wt].end())
    {
        pWeaponBreakdown->UpdateFeatEffect(
                m_pCharacter,
                (*it).DisplayName(),
                (*it));
        ++it;
    }
    it = m_weaponItemEffects[wt].begin();
    while (it != m_weaponItemEffects[wt].end())
    {
        pWeaponBreakdown->UpdateItemEffect(
                m_pCharacter,
                (*it).DisplayName(),
                (*it));
        ++it;
    }
    std::list<EffectTier>::const_iterator itt = m_weaponEnhancementEffects[wt].begin();
    while (itt != m_weaponEnhancementEffects[wt].end())
    {
        pWeaponBreakdown->UpdateEnhancementEffect(
                m_pCharacter,
                (*itt).m_effect.DisplayName(),
                (*itt));
        ++itt;
    }

    return pWeaponBreakdown;
}
