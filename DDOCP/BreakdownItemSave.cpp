// BreakdownItemSave.cpp
//
#include "stdafx.h"
#include "BreakdownItemSave.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemSave::BreakdownItemSave(
        BreakdownType type,
        SaveType st, 
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem,
        AbilityType ability,
        BreakdownItem * pBaseItem) :
    BreakdownItem(type, treeList, hItem),
    m_saveType(st),
    m_ability(ability),
    m_pBaseBreakdown(pBaseItem),
    m_noFailOn1Count(0)
{
    // need to know when base total value changes
    if (ability != Ability_Unknown)
    {
        AddAbility(ability);
    }
    if (m_pBaseBreakdown != NULL)
    {
        // need to know changes in our base save type
        m_pBaseBreakdown->AttachObserver(this);
    }
}

BreakdownItemSave::~BreakdownItemSave()
{
}

// required overrides
CString BreakdownItemSave::Title() const
{
    CString text;
    text = EnumEntryText(m_saveType, saveTypeMap);
    if (m_saveType != Save_Fortitude
            && m_saveType != Save_Reflex
            && m_saveType != Save_Will)
    {
        text = "vs " + text;
    }
    return text;
}

CString BreakdownItemSave::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    if (HasNoFailOn1())
    {
        value += " (No fail on 1)";
    }
    return value;
}

void BreakdownItemSave::CreateOtherEffects()
{
    // add class/ability saves for the generic saves types of Fortitude, Reflex and Will
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        if (m_saveType == Save_Fortitude
                || m_saveType == Save_Reflex
                || m_saveType == Save_Will)
        {
            std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
            for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
            {
                if (classLevels[ci] > 0
                        && ci != Class_Epic)
                {
                    std::string className = EnumEntryText((ClassType)ci, classTypeMap);
                    className += " Levels";
                    ActiveEffect classBonus(
                            Bonus_class,
                            className,
                            1,
                            ClassSave(m_saveType, (ClassType)ci, classLevels[ci]),
                            "");        // no tree
                    AddOtherEffect(classBonus);
                }
            }
            // special case, look for Divine Grace feat
            size_t count = TrainedCount(m_pCharacter->CurrentFeats(MAX_LEVEL), "Divine Grace");
            if (count > 0)
            {
                // character has divine grace feat trained. Add charisma bonus for this save
                BreakdownItem * pBI = FindBreakdown(StatToBreakdown(Ability_Charisma));
                ASSERT(pBI != NULL);
                pBI->AttachObserver(this); // watch for any changes
                int bonus = BaseStatToBonus(pBI->Total());
                if (bonus != 0) // only add to list if non zero
                {
                    // should now have the best option
                    ActiveEffect feat(
                            Bonus_ability,
                            "Divine Grace (Charisma)",
                            1,
                            bonus,
                            "");        // no tree
                    feat.SetBreakdownDependency(StatToBreakdown(Ability_Charisma)); // so we know which effect to update
                    AddOtherEffect(feat);
                }
            }
        }
        if (m_ability != Ability_Unknown)
        {
            // Base ability bonus to save
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
                AddOtherEffect(feat);
            }
        }
        if (m_pBaseBreakdown != NULL)
        {
            // show the base save value used in our total
            double bonus = m_pBaseBreakdown->Total();
            ActiveEffect statBonus(
                    Bonus_base,
                    (LPCTSTR)(m_pBaseBreakdown->Title() + " Save"),
                    1,
                    bonus,
                    "");        // no tree
            AddOtherEffect(statBonus);
        }
    }
}

bool BreakdownItemSave::AffectsUs(const Effect & effect) const
{
    // return true if the effect applies to this save
    // note that effect that apply to "All" only apply to Fort, reflex and will
    // as the sub-save types use the total from the main category as a part of their total
    // so we do not want to count the bonus twice
    bool isUs = false;
    if (effect.Type() == Effect_SaveBonus)
    {
        if (m_saveType == Save_Fortitude
                || m_saveType == Save_Reflex
                || m_saveType == Save_Will)
        {
            isUs = (effect.Save() == Save_All
                    || effect.Save() == m_saveType);
        }
        else
        {
            // for a sub-type, it must be the specific save type only
            isUs = (effect.Save() == m_saveType);
        }
    }
    return isUs;
}

void BreakdownItemSave::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // need to re-create other effects list
    CreateOtherEffects();
    Populate();
}

void BreakdownItemSave::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // Stat bonus has changed
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}

void BreakdownItemSave::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.HasNoFailOn1())
        {
            if (effect.NoFailOn1()[0] != 0)
            {
                ++m_noFailOn1Count;
            }
        }
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

void BreakdownItemSave::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.HasNoFailOn1())
        {
            if (effect.NoFailOn1()[0] != 0)
            {
                --m_noFailOn1Count;
            }
        }
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

void BreakdownItemSave::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.HasNoFailOn1())
        {
            if (effect.NoFailOn1()[0] != 0)
            {
                ++m_noFailOn1Count;
            }
        }
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

void BreakdownItemSave::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect))
    {
        if (effect.HasNoFailOn1())
        {
            if (effect.NoFailOn1()[0] != 0)
            {
                --m_noFailOn1Count;
            }
        }
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

void BreakdownItemSave::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        if (effect.m_effect.HasNoFailOn1())
        {
            if (effect.m_effect.NoFailOn1()[effect.m_tier-1] != 0)
            {
                ++m_noFailOn1Count;
            }
        }
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

void BreakdownItemSave::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle special affects that change our list of available stats
    if (AffectsUs(effect.m_effect))
    {
        if (effect.m_effect.HasNoFailOn1())
        {
            if (effect.m_effect.NoFailOn1()[effect.m_tier-1] != 0)
            {
                --m_noFailOn1Count;
            }
        }
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

bool BreakdownItemSave::HasNoFailOn1() const
{
    if (m_pBaseBreakdown != NULL)
    {
        // saves which are based on a main save type (Will, Reflex and Fortitude)
        // will have a no fail on a 1 if their base save also has a no fail on a 1
        BreakdownItemSave * pBB = dynamic_cast<BreakdownItemSave*>(m_pBaseBreakdown);
        if (pBB->HasNoFailOn1())
        {
            return true;
        }
    }
    return (m_noFailOn1Count > 0);
}

