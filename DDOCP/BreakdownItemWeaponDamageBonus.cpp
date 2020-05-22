// BreakdownItemWeaponDamageBonus.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeaponDamageBonus.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemWeaponDamageBonus::BreakdownItemWeaponDamageBonus(
        BreakdownType type,
        EffectType effect,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect),
    m_bOffHand(false)
{
}

BreakdownItemWeaponDamageBonus::~BreakdownItemWeaponDamageBonus()
{
}

// required overrides
CString BreakdownItemWeaponDamageBonus::Title() const
{
    return m_title;
}

CString BreakdownItemWeaponDamageBonus::Value() const
{
    CString value;
    value.Format(
            "%+3d",
            (int)Total());
    return value;
}

void BreakdownItemWeaponDamageBonus::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();

        // by default all weapons use Strength as their base stat for bonus damage
        // but other stats may also be allowed for this particular weapon. look through
        // the list of those available and get the one with the largest value
        AbilityType ability = LargestStatBonus();
        if (ability != Ability_Unknown)
        {
            double multiplier = 1.0;
            BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
            ASSERT(pBI != NULL);
            int bonus = BaseStatToBonus(pBI->Total());
            if (m_bOffHand)
            {
                // off hand only gets 50% of ability bonus to damage unless
                // TempestDualPerfection is trained
                if (!m_pCharacter->IsEnhancementTrained("TempestDualPerfection", ""))
                {
                    multiplier = 0.5;
                }
            }
            else
            {
                // single weapon fighting line of feats can affect the amount of damage bonus
                // from your main damage stat
                if (m_pCharacter->IsStanceActive("Single Weapon Fighting"))
                {
                    // they are single weapon fighting, work out the total stat multiplier to use
                    if (m_pCharacter->IsFeatTrained("Improved Single Weapon Fighting"))
                    {
                        multiplier = 1.25;
                        if (m_pCharacter->IsFeatTrained("Greater Single Weapon Fighting"))
                        {
                            multiplier = 1.5;
                        }
                    }
                }
                // two handed weapons get 1.5 to 3.0 times the damage bonus
                // depending on trained feats
                if (IsTwoHandedWeapon(Weapon()))
                {
                    // Wolf/Bear form does not affect multiplier unless Natural Fighting is trained (Bears only)
                    if (m_pCharacter->IsStanceActive("Wolf")
                            || m_pCharacter->IsStanceActive("Bear")
                            || m_pCharacter->IsStanceActive("Dire Wolf")
                            || m_pCharacter->IsStanceActive("Dire Bear"))
                    {
                        if (m_pCharacter->IsStanceActive("Bear")
                            || m_pCharacter->IsStanceActive("Dire Bear"))
                        {
                            // bears get a multiplier based on number of Natural Fighting feats trained
                            size_t count = TrainedCount(m_pCharacter->CurrentFeats(MAX_LEVEL), "Natural Fighting");
                            switch (count)
                            {
                            case 0: multiplier = 1.0; break;
                            case 1: multiplier = 2.0; break;
                            case 2: multiplier = 2.5; break;
                            default:
                            case 3: multiplier = 3.0; break;
                            }
                        }
                    }
                    else
                    {
                        multiplier = 1.5;       // no THF feats
                        if (m_pCharacter->IsFeatTrained("Two Handed Fighting"))
                        {
                            multiplier = 2.0;
                            if (m_pCharacter->IsFeatTrained("Improved Two Handed Fighting"))
                            {
                                multiplier = 2.5;
                                if (m_pCharacter->IsFeatTrained("Greater Two Handed Fighting"))
                                {
                                    multiplier = 3.0;
                                }
                            }
                        }
                    }
                }
            }

            // Bastard Sword and Dwarven Waraxe now apply 1.1x your relevant Ability Score Modifier to damage.
            // This stacks with the bonuses from Improved and Greater Single Weapon Fighting.
            // Improved Two Handed Fighting now also improves Bastard Sword and Dwarven Waraxe to 1.35x your relevant Ability Score Modifier to damage
            // Greater Two Handed Fighting now also improves Bastard Sword and Dwarven Waraxe to 1.6x your relevant Ability Score Modifier to damage
            if (Weapon() == Weapon_BastardSword
                    || Weapon() == Weapon_DwarvenAxe)
            {
                multiplier += 0.1; // basic 1.1
                if (m_pCharacter->IsFeatTrained("Improved Two Handed Fighting"))
                {
                    multiplier += 0.25;      // 1.35 overall
                    if (m_pCharacter->IsFeatTrained("Greater Two Handed Fighting"))
                    {
                        multiplier += 0.25; // 1.6 overall
                    }
                }
            }
            if (bonus != 0) // only add to list if non zero
            {
                // should now have the best option
                std::string bonusName;
                if (multiplier != 1.0)
                {
                    CString text;
                    text.Format(
                            "%d%% of Ability bonus (%s)",
                            (int)(multiplier * 100),
                            EnumEntryText(ability, abilityTypeMap));
                    bonusName = text;
                    bonus = (int)(bonus * multiplier);
                }
                else
                {
                    bonusName = "Ability bonus (" + EnumEntryText(ability, abilityTypeMap) + ")";
                }
                ActiveEffect feat(
                        Bonus_ability,
                        bonusName,
                        1,
                        bonus,
                        "");        // no tree
                feat.SetBreakdownDependency(StatToBreakdown(ability)); // so we know which effect to update
                feat.SetDivider(1.0 / multiplier, DT_statBonus);
                AddOtherEffect(feat);
            }
        }
    }
}

bool BreakdownItemWeaponDamageBonus::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect)
    {
        // if its the right effect its for us as our holder class determines whether
        // it is the right weapon target type
        isUs = true;
    }
    if (effect.Type() == Effect_WeaponDamageAbility)
    {
        // weapon enchantments affect us if specific weapon
        isUs = true;
    }
    if (effect.Type() == Effect_WeaponEnchantment)
    {
        // weapon enchantments affect us if specific weapon
        isUs = true;
    }
    if (effect.Type() == Effect_WeaponPlus)
    {
        // weapon plus affect us if specific weapon
        isUs = true;
    }
    return isUs;
}

void BreakdownItemWeaponDamageBonus::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // damage bonus due to favored Soul levels with any feat
    // of "Grace of Battle" or "Knowledge of Battle" can cause change
    CreateOtherEffects();
}

void BreakdownItemWeaponDamageBonus::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.Type() == Effect_WeaponDamageAbility)
        {
            // add to the list of available stats for this weapon
            ASSERT(effect.HasAbility());
            AddAbility(effect.Ability());  // duplicates are fine
            CreateOtherEffects();
            Populate();
        }
        else
        {
            // pass through to the base class
            BreakdownItem::UpdateFeatEffect(pCharacter, featName, effect);
        }
    }
}

void BreakdownItemWeaponDamageBonus::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.Type() == Effect_WeaponDamageAbility)
        {
            ASSERT(effect.HasAbility());
            RemoveFirstAbility(effect.Ability());
            CreateOtherEffects();
            Populate();
        }
        else
        {
            // pass through to the base class
            BreakdownItem::UpdateFeatEffectRevoked(pCharacter, featName, effect);
        }
    }
}

void BreakdownItemWeaponDamageBonus::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.Type() == Effect_WeaponDamageAbility)
        {
            // add to the list of available stats for this weapon
            ASSERT(effect.HasAbility());
            AddAbility(effect.Ability());  // duplicates are fine
            CreateOtherEffects();
            Populate();
        }
        else
        {
            // pass through to the base class
            BreakdownItem::UpdateItemEffect(pCharacter, itemName, effect);
        }
    }
}

void BreakdownItemWeaponDamageBonus::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.Type() == Effect_WeaponDamageAbility)
        {
            ASSERT(effect.HasAbility());
            RemoveFirstAbility(effect.Ability());
            CreateOtherEffects();
            Populate();
        }
        else
        {
            // pass through to the base class
            BreakdownItem::UpdateItemEffectRevoked(pCharacter, itemName, effect);
        }
    }
}

void BreakdownItemWeaponDamageBonus::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        if (effect.m_effect.Type() == Effect_WeaponDamageAbility)
        {
            // add to the list of available stats for this weapon
            ASSERT(effect.m_effect.HasAbility());
            AddAbility(effect.m_effect.Ability());  // duplicates are fine
            CreateOtherEffects();
            Populate();
        }
        else
        {
            // pass through to the base class
            BreakdownItem::UpdateEnhancementEffect(pCharacter, enhancementName, effect);
        }
    }
}

void BreakdownItemWeaponDamageBonus::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        if (effect.m_effect.Type() == Effect_WeaponDamageAbility)
        {
            ASSERT(effect.m_effect.HasAbility());
            RemoveFirstAbility(effect.m_effect.Ability());
            CreateOtherEffects();
            Populate();
        }
        else
        {
            // pass through to the base class
            BreakdownItem::UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
        }
    }
}

void BreakdownItemWeaponDamageBonus::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // ability stat for this skill has changed, update our skill total
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}

void BreakdownItemWeaponDamageBonus::UpdateEnhancementTrained(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    BreakdownItem::UpdateEnhancementTrained(
            charData,
            enhancementName,
            selection,
            isTier5);
}

void BreakdownItemWeaponDamageBonus::UpdateEnhancementRevoked(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    BreakdownItem::UpdateEnhancementRevoked(
            charData,
            enhancementName,
            selection,
            isTier5);
}

void BreakdownItemWeaponDamageBonus::SetIsOffHand(bool offHand)
{
    m_bOffHand = offHand;
}
