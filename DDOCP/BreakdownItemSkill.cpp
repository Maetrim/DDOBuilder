// BreakdownItemSkill.cpp
//
#include "stdafx.h"
#include "BreakdownItemSkill.h"
#include "BreakdownsView.h"

#include "GlobalSupportFunctions.h"

BreakdownItemSkill::BreakdownItemSkill(
        SkillType skill,
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem,
        BreakdownItem * pAbility) :
    BreakdownItem(type, treeList, hItem),
    m_skill(skill),
    m_pAbility(pAbility)
{
    m_pAbility->AttachObserver(this);   // we update if this changes
}

BreakdownItemSkill::~BreakdownItemSkill()
{
}

// required overrides
CString BreakdownItemSkill::Title() const
{
    // skills use their name as the title
    CString text;
    text = EnumEntryText(m_skill, skillTypeMap);
    return text;
}

CString BreakdownItemSkill::Value() const
{
    // just return the total formatted as a double or "N/A" if not active for this class combination
    CString value;
    value = "N/A";          // assume not active
    if (m_pCharacter != NULL)
    {
        // some skills must have trained ranks else always shown as N/A
        double maxSkill = m_pCharacter->MaxSkillForLevel(
                m_skill,
                19);   // only check heroic level spend
        if (maxSkill > 0)
        {
            value.Format(
                    "%5.1f",            // possible to have half ranks
                    Total());
        }
    }
    return value;
}

void BreakdownItemSkill::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        // all skills have the amount trained first
        double amount = m_pCharacter->SkillAtLevel(
                m_skill,
                MAX_LEVEL - 1, // 0 based
                false);     // skill tome not included
        if (amount > 0)
        {
            ActiveEffect amountTrained(
                    Bonus_levelUps,
                    "Trained ranks",
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(amountTrained);
        }

        // all skills have an ability bonus, we need to use the total after all modifiers applied
        // for the base ability
        AbilityType at = StatFromSkill(m_skill);
        double ability = m_pAbility->Total();
        amount = BaseStatToBonus(ability);
        if (amount != 0)
        {
            // example label is "Str Modifier"
            std::string abilityName = EnumEntryText(at, abilityTypeMap);
            abilityName.resize(3);          // cut down to 1st 3 characters, e.g. Strength becomes Str
            abilityName += " Modifier";
            ActiveEffect abilityMod(
                    Bonus_ability,
                    abilityName,
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(abilityMod);
        }
        // skills can have a tome for them
        amount = m_pCharacter->SkillTomeValue(m_skill);
        if (amount > 0)
        {
            ActiveEffect tome(
                    Bonus_inherent,
                    "Skill tome",
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(tome);
        }
        // armor check penalty
        // armor check penalties may or may not exist for this skill
        int multiplier = ArmorCheckPenalty_Multiplier(m_skill);
        if (multiplier != 0)
        {
            BreakdownItem * pBI = FindBreakdown(Breakdown_ArmorCheckPenalty);
            ASSERT(pBI != NULL);
            pBI->AttachObserver(this);  // need to know about changes to this effect
            ActiveEffect acp(
                    Bonus_penalty,
                    "Armor check penalty",
                    1,
                    pBI->Total() * multiplier,
                    "");        // no tree
            AddOtherEffect(acp);
        }
    }
}

void BreakdownItemSkill::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // ability stat for this skill has changed, update our skill total
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}

void BreakdownItemSkill::UpdateSkillSpendChanged(
        Character * pCharacter,
        size_t level,
        SkillType skill)
{
    if (skill == m_skill)
    {
        // our skill has changed, update
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemSkill::UpdateSkillTomeChanged(
        Character * pCharacter,
        SkillType skill)
{
    if (skill == m_skill)
    {
        // our skill has changed, update
        CreateOtherEffects();
        Populate();
    }
}

bool BreakdownItemSkill::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_SkillBonus)
    {
        if (effect.HasSkill()
                && (effect.Skill() == Skill_All
                || effect.Skill() == m_skill))
        {
            isUs = true;
        }
        // bonus could be by ability, i.e. all Dexterity skills
        if (effect.HasAbility())
        {
            AbilityType at = StatFromSkill(m_skill);
            if (at == effect.Ability())
            {
                isUs = true;
            }
        }
    }
    return isUs;
}

SkillType BreakdownItemSkill::Skill() const
{
    return m_skill;
}

