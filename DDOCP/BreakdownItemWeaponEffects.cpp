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
    if (charData == NULL)
    {
        // remove any weapons listed from the previous character
        EquippedGear gear;      // empty gear
        WeaponsChanged(gear);
    }
    // clear any effects on a character change
    for (size_t i = 0; i < Weapon_Count; ++i)
    {
        m_weaponFeatEffects[i].clear();
        m_weaponItemEffects[i].clear();
        m_weaponEnhancementEffects[i].clear();
    }
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
    if (effect.Weapon().size() > 0)
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
    if (effect.IncludesWeapon(wt))
    {
        isUs = true;
    }
    if (effect.HasWeaponClass())
    {
        // a class of weapons are affected, determine whether
        // we fall into it
        isUs = IsInWeaponClass(effect.WeaponClass(), wt);
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
    case Weapon_GreatClub:
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
    case Weapon_GreatClub:
    case Weapon_GreatSword:
    case Weapon_Maul:
    case Weapon_Quarterstaff:
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
    case Weapon_GreatClub:
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
    switch (type)
    {
    case WeaponDamage_Bludgeoning:
        switch (wt)
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
        switch (wt)
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
        switch (wt)
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
        switch (wt)
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
        switch (wt)
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

bool BreakdownItemWeaponEffects::AreWeaponsCentering() const
{
    bool isCentered = true;
    if (m_pMainHandWeapon != NULL
            && !m_pMainHandWeapon->IsCentering())
    {
        isCentered = false;
    }
    if (m_pOffHandWeapon != NULL
            && !m_pOffHandWeapon->IsCentering())
    {
        isCentered = false;
    }
    return isCentered;
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
    Dice defaultDice;
    pWeaponBreakdown = new BreakdownItemWeapon(
            bt,
            wt,
            name.c_str(),
            m_pTreeList,
            hItem,
            bt == Breakdown_MainHand
                ? Inventory_Weapon1
                : Inventory_Weapon2,
            item.HasDamageDice()
                ? item.DamageDice()
                : defaultDice,
            item.CriticalMultiplier());
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
    // some item specific effects may come from augments
    const std::vector<ItemAugment> & augments = item.Augments();
    for (size_t ai = 0; ai < augments.size(); ++ai)
    {
        if (augments[ai].HasSelectedAugment())
        {
            // there is an augment in this position
            const Augment & augment = FindAugmentByName(augments[ai].SelectedAugment());
            // name is:
            // <item>:<augment type>:<Augment name>
            std::stringstream ss;
            ss << item.Name()
                    << " : " << augments[ai].Type()
                    << " : " << augment.Name();
            // now revoke the item specific augments effects
            std::string name;
            name = ss.str();
            std::list<Effect> effects = augment.Effects();
            std::list<Effect>::iterator it = effects.begin();
            while (it != effects.end())
            {
                if (augment.HasEnterValue()
                        && augments[ai].HasValue())
                {
                    (*it).Set_Amount(augments[ai].Value());
                }
                if ((*it).HasIsItemSpecific())
                {
                    pWeaponBreakdown->UpdateItemEffect(
                            m_pCharacter,
                            name,
                            (*it));
                }
                ++it;
            }
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

void BreakdownItemWeaponEffects::AddForumExportData(std::stringstream & forumExport)
{
    if (m_pMainHandWeapon != NULL)
    {
        forumExport << "Main Hand: ";
        m_pMainHandWeapon->AddForumExportData(forumExport);
    }
    if (m_pOffHandWeapon != NULL)
    {
        forumExport << "Off Hand: ";
        m_pOffHandWeapon->AddForumExportData(forumExport);
    }
}

BreakdownItem * BreakdownItemWeaponEffects::GetWeaponBreakdown(bool bMainhand, BreakdownType bt)
{
    BreakdownItem * pBI = NULL;
    if (bMainhand
            && m_pMainHandWeapon != NULL)
    {
        pBI = m_pMainHandWeapon->GetWeaponBreakdown(bt);
    }
    if (!bMainhand
            && m_pMainHandWeapon != NULL)
    {
        pBI = m_pOffHandWeapon->GetWeaponBreakdown(bt);
    }
    return pBI;
}

