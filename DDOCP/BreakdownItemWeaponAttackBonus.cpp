// BreakdownItemWeaponAttackBonus.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeaponAttackBonus.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemWeaponAttackBonus::BreakdownItemWeaponAttackBonus(
        BreakdownType type,
        EffectType effect,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect),
    m_proficientCount(0)
{
}

BreakdownItemWeaponAttackBonus::~BreakdownItemWeaponAttackBonus()
{
}

// required overrides
CString BreakdownItemWeaponAttackBonus::Title() const
{
    return m_title;
}

CString BreakdownItemWeaponAttackBonus::Value() const
{
    CString value;
    value.Format(
            "%+3d",
            (int)Total());
    return value;
}

void BreakdownItemWeaponAttackBonus::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        // all weapon to hits include BAB
        double amount = m_pCharacter->BaseAttackBonus(MAX_LEVEL);
        if (amount > 0)
        {
            ActiveEffect amountTrained(
                    Bonus_base,
                    "Base Attack Bonus",
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(amountTrained);
        }
        // must be proficient with the weapon
        if (m_proficientCount == 0)
        {
            // non-proficient penalty
            ActiveEffect amountTrained(
                    Bonus_penalty,
                    "Non proficient penalty",
                    1,
                    -4,
                    "");        // no tree
            AddOtherEffect(amountTrained);

            // also apply any armor check penalty
            BreakdownItem * pBI = m_pBreakdownView->FindBreakdown(Breakdown_ArmorCheckPenalty);
            ASSERT(pBI != NULL);
            pBI->AttachObserver(this);  // need to know about changes to this effect
            if (pBI->Total() != 0)
            {
                ActiveEffect acp(
                        Bonus_penalty,
                        "Armor check penalty",
                        1,
                        pBI->Total(),
                        "");        // no tree
                AddOtherEffect(acp);
            }
        }

        // by default all weapons use Strength as their base stat for attack bonus
        // but other stats may also be allowed for this particular weapon. look through
        // the list of those available and get the one with the largest value
        AbilityType ability = LargestStatBonus();
        BreakdownItem * pBI = m_pBreakdownView->FindBreakdown(StatToBreakdown(ability));
        ASSERT(pBI != NULL);
        int bonus = BaseStatToBonus(pBI->Total());
        if (bonus != 0) // only add to list if non zero
        {
            // should now have the best option
            std::string bonusName = "Ability bonus (" + EnumEntryText(ability, abilityTypeMap) + ")";
            ActiveEffect feat(
                    Bonus_ability,
                    bonusName,
                    1,
                    bonus,
                    "");        // no tree
            feat.SetBreakdownDependency(StatToBreakdown(ability)); // so we know which effect to update
            AddOtherEffect(feat);
        }
    }
}

bool BreakdownItemWeaponAttackBonus::AffectsUs(const Effect & effect) const
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

void BreakdownItemWeaponAttackBonus::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // if a class has changed, then the BAB may have changed
    CreateOtherEffects();
}

void BreakdownItemWeaponAttackBonus::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle checking for weapon proficiency
    if (effect.Type() == Effect_WeaponProficiency)
    {
        ++m_proficientCount;
        CreateOtherEffects();
    }
    if (AffectsUs(effect))
    {
        // handle special affects that change our list of available stats
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

void BreakdownItemWeaponAttackBonus::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle checking for weapon proficiency
    if (effect.Type() == Effect_WeaponProficiency)
    {
        --m_proficientCount;
        CreateOtherEffects();
    }
    if (AffectsUs(effect))
    {
        // handle special affects that change our list of available stats
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

void BreakdownItemWeaponAttackBonus::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle checking for weapon proficiency
    if (effect.Type() == Effect_WeaponProficiency)
    {
        ++m_proficientCount;
        CreateOtherEffects();
    }
    if (AffectsUs(effect))
    {
        // handle special affects that change our list of available stats
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

void BreakdownItemWeaponAttackBonus::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle checking for weapon proficiency
    if (effect.Type() == Effect_WeaponProficiency)
    {
        --m_proficientCount;
        CreateOtherEffects();
    }
    if (AffectsUs(effect))
    {
        // handle special affects that change our list of available stats
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

void BreakdownItemWeaponAttackBonus::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle checking for weapon proficiency
    if (effect.m_effect.Type() == Effect_WeaponProficiency)
    {
        ++m_proficientCount;
        CreateOtherEffects();
    }
    if (AffectsUs(effect.m_effect))
    {
        // handle special affects that change our list of available stats
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

void BreakdownItemWeaponAttackBonus::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle checking for weapon proficiency
    if (effect.m_effect.Type() == Effect_WeaponProficiency)
    {
        --m_proficientCount;
        CreateOtherEffects();
    }
    if (AffectsUs(effect.m_effect))
    {
        // handle special affects that change our list of available stats
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

void BreakdownItemWeaponAttackBonus::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // ability stat for this skill has changed, update our skill total
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}
