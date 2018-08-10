// BreakdownItemWeaponAttackBonus.cpp
//
#include "stdafx.h"
#include "BreakdownItemWeaponAttackBonus.h"

#include "BreakdownsView.h"
#include "BreakdownItemWeapon.h"
#include "GlobalSupportFunctions.h"

const std::string c_TWF = "Two Weapon Fighting";
const std::string c_OTWF = "Oversized Two Weapon Fighting";

BreakdownItemWeaponAttackBonus::BreakdownItemWeaponAttackBonus(
        BreakdownType type,
        EffectType effect,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem,
        BreakdownItemWeapon * pWeapon,
        InventorySlotType slot) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect),
    m_proficientCount(0),
    m_pWeapon(pWeapon),
    m_slot(slot)
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
        // all weapon to hits include BAB, which may be changed by a
        // enhancement
        BreakdownItem * pBBAB = FindBreakdown(Breakdown_BAB);
        ASSERT(pBBAB != NULL);
        pBBAB->AttachObserver(this);  // need to know about changes to this effect
        double amount = pBBAB->Total();
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
            BreakdownItem * pBI = FindBreakdown(Breakdown_ArmorCheckPenalty);
            ASSERT(pBI != NULL);
            pBI->AttachObserver(this);  // need to know about changes to this effect
            double total = max(0, pBI->Total()); // ACP cannot be a bonus!
            if (total != 0)
            {
                ActiveEffect acp(
                        Bonus_penalty,
                        "Armor check penalty",
                        1,
                        -total,
                        "");        // no tree
                AddOtherEffect(acp);
            }
        }

        // by default all weapons use Strength as their base stat for attack bonus
        // but other stats may also be allowed for this particular weapon. look through
        // the list of those available and get the one with the largest value
        AbilityType ability = LargestStatBonus();
        BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
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
            feat.SetDivider(1, DT_statBonus);
            AddOtherEffect(feat);
        }

        if (m_pCharacter->IsStanceActive("Two Weapon Fighting"))
        {
            int bonus = 0;
            // they are two weapon fighting, apply attack penalties to this weapon
            if (m_pCharacter->IsFeatTrained(c_TWF))
            {
                // -4/-4 penalty when TWF with light off hand weapon
                bonus = -4;
            }
            else
            {
                // -6/-10 penalty when no TWF
                bonus = (m_slot == Inventory_Weapon1) ? -6 : -10;
            }
            // heavy weapon off hand weapon penalty
            if (m_pCharacter->LightWeaponInOffHand()
                    || m_pCharacter->IsFeatTrained(c_OTWF))
            {
                // 2 less penalty if off hand weapon is light
                // or over sized TWF is trained
                bonus += 2;
            }
            ActiveEffect twf(
                    Bonus_penalty,
                    "TWF attack penalty",
                    1,
                    bonus,
                    "");        // no tree
            AddOtherEffect(twf);
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
    // can also affect attack bonus due to favored Soul levels with any feat
    // of "Grace of Battle" or "Knowledge of Battle"
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
            AddAbility(effect.Ability(), effect.Stance(), Weapon());  // duplicates are fine
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
            RemoveFirstAbility(effect.Ability(), effect.Stance(), Weapon());
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
            AddAbility(effect.Ability(), effect.Stance(), Weapon());  // duplicates are fine
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
            RemoveFirstAbility(effect.Ability(), effect.Stance(), Weapon());
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
            AddAbility(effect.m_effect.Ability(), effect.m_effect.Stance(), Weapon());  // duplicates are fine
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
            RemoveFirstAbility(effect.m_effect.Ability(), effect.m_effect.Stance(), Weapon());
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

void BreakdownItemWeaponAttackBonus::UpdateFeatTrained(
        Character * charData,
        const std::string & featName)
{
    // TWF and Oversized TWF affect attack penalty
    if (featName == c_TWF)
    {
        CreateOtherEffects();
    }
    else if (m_slot == Inventory_Weapon2
            && featName == c_OTWF)
    {
        CreateOtherEffects();
    }
}

void BreakdownItemWeaponAttackBonus::UpdateFeatRevoked(
        Character * charData,
        const std::string & featName)
{
    // TWF and Oversized TWF affect attack penalty
    if (featName == c_TWF)
    {
        CreateOtherEffects();
    }
    else if (m_slot == Inventory_Weapon2
            && featName == c_OTWF)
    {
        CreateOtherEffects();
    }
}

