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
            BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
            ASSERT(pBI != NULL);
            int bonus = BaseStatToBonus(pBI->Total());
            bool only50Percent = false;
            if (m_bOffHand)
            {
                // off hand only gets 50% of ability bonus to damage unless
                // TempestDualPerfection is trained
                if (!m_pCharacter->IsEnhancementTrained("TempestDualPerfection", ""))
                {
                    // divide the bonus by 2
                    only50Percent = true;
                    bonus /= 2; // rounds down
                }
            }
            if (bonus != 0) // only add to list if non zero
            {
                // should now have the best option
                std::string bonusName;
                if (only50Percent)
                {
                    bonusName = "50% of Ability bonus (" + EnumEntryText(ability, abilityTypeMap) + ")";
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
                feat.SetDivider(1, DT_statBonus);
                AddOtherEffect(feat);
            }
        }
        if (IsMeleeWeapon(Weapon()))
        {
            // handle divine presence which cannot be handled with effects
            if (m_pCharacter->IsEnhancementTrained("WSDivineMight", "Divine Presence"))
            {
                // divine presence is trained
                BreakdownItem * pBI = FindBreakdown(StatToBreakdown(Ability_Charisma));
                ASSERT(pBI != NULL);
                pBI->AttachObserver(this); // watch for any changes
                int bonus = BaseStatToBonus(pBI->Total()) / 2;
                ActiveEffect feat(
                        Bonus_insightful,
                        "Divine Presence (Cha Mod / 2)",
                        1,
                        bonus,
                        "");        // no tree
                AddOtherEffect(feat);
            }
            // handle divine will which cannot be handled with effects
            if (m_pCharacter->IsEnhancementTrained("WSDivineMight", "Divine Will"))
            {
                // divine will is trained
                BreakdownItem * pBI = FindBreakdown(StatToBreakdown(Ability_Wisdom));
                ASSERT(pBI != NULL);
                pBI->AttachObserver(this); // watch for any changes
                int bonus = BaseStatToBonus(pBI->Total()) / 2;
                ActiveEffect feat(
                        Bonus_insightful,
                        "Divine Will (Wis Mod / 2)",
                        1,
                        bonus,
                        "");        // no tree
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
    if (effect.Type() == Effect_WeaponEnchantment)
    {
        // weapon enchantments affect us if specific weapon
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
        if (effect.HasAbility())
        {
            // add to the list of available stats for this weapon
            ASSERT(effect.HasAbility());
            AddAbility(effect.Ability());  // duplicates are fine
            CreateOtherEffects();
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
        if (effect.HasAbility())
        {
            ASSERT(effect.HasAbility());
            RemoveFirstAbility(effect.Ability());
            CreateOtherEffects();
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
        if (effect.HasAbility())
        {
            // add to the list of available stats for this weapon
            ASSERT(effect.HasAbility());
            AddAbility(effect.Ability());  // duplicates are fine
            CreateOtherEffects();
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
        if (effect.HasAbility())
        {
            ASSERT(effect.HasAbility());
            RemoveFirstAbility(effect.Ability());
            CreateOtherEffects();
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
        if (effect.m_effect.HasAbility())
        {
            // add to the list of available stats for this weapon
            ASSERT(effect.m_effect.HasAbility());
            AddAbility(effect.m_effect.Ability());  // duplicates are fine
            CreateOtherEffects();
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
        if (effect.m_effect.HasAbility())
        {
            ASSERT(effect.m_effect.HasAbility());
            RemoveFirstAbility(effect.m_effect.Ability());
            CreateOtherEffects();
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
    // check for "Divine Might" in War Soul tree only being trained specifically
    if (IsMeleeWeapon(Weapon())
            && enhancementName == "WSDivineMight")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
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
    // check for "Divine Might" War Soul tree only being revoked specifically
    if (IsMeleeWeapon(Weapon())
            && enhancementName == "WSDivineMight")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemWeaponDamageBonus::SetIsOffHand(bool offHand)
{
    m_bOffHand = offHand;
}
