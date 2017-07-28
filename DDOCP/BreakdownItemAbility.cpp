// BreakdownItemAbility.cpp
//
#include "stdafx.h"
#include "BreakdownItemAbility.h"

#include "GlobalSupportFunctions.h"

BreakdownItemAbility::BreakdownItemAbility(
        AbilityType ability,
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_ability(ability)
{
}

BreakdownItemAbility::~BreakdownItemAbility()
{
}

// required overrides
CString BreakdownItemAbility::Title() const
{
    CString text = EnumEntryText(m_ability, abilityTypeMap);
    return text;
}

CString BreakdownItemAbility::Value() const
{
    CString value;
    value.Format(
            "%3d",          // ability values are always whole numbers
            (int)Total());
    return value;
}

void BreakdownItemAbility::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        const Character & charData = *m_pCharacter;
        const AbilitySpend & as = charData.BuildPoints();
        // basic build point spend
        int amount = 8 + as.GetAbilitySpend(m_ability);
        if (amount != 0)
        {
            ActiveEffect tome(
                    Bonus_ability,
                    "Base",
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(tome);
        }
        // racial
        amount = RacialModifier(m_pCharacter->Race(), m_ability);
        if (amount != 0)
        {
            ActiveEffect tome(
                    Bonus_racial,
                    "Racial Modifier",
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(tome);
        }
        // tome
        amount = m_pCharacter->AbilityTomeValue(m_ability);
        if (amount > 0)
        {
            ActiveEffect tome(
                    Bonus_inherent,
                    "Ability Tome",
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(tome);
        }
        // level ups
        amount = charData.LevelUpsAtLevel(m_ability, MAX_LEVEL);
        if (amount > 0)
        {
            ActiveEffect tome(
                    Bonus_levelUps,
                    "Level Ups",
                    (int)amount,
                    1,
                    "");        // no tree
            AddOtherEffect(tome);
        }
    }
}

void BreakdownItemAbility::UpdateAbilityValueChanged(
        Character * pCharacter,
        AbilityType ability)
{
    if (m_ability == ability)
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemAbility::UpdateAbilityTomeChanged(
        Character * pCharacter,
        AbilityType ability)
{
    if (m_ability == ability)
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}


void BreakdownItemAbility::UpdateRaceChanged(Character * charData, RaceType race)
{
    // race change could affect any ability, always repopulate for this
    CreateOtherEffects();
    Populate();
}

bool BreakdownItemAbility::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_AbilityBonus)
    {
        // it is an ability bonus, it way well affect us
        ASSERT(effect.HasAbility());    // its expected
        if (effect.Ability() == Ability_All
                || effect.Ability() == m_ability)
        {
            isUs = true;
        }
    }
    return isUs;
}
