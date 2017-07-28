// BreakdownItemAssassinate.cpp
//
#include "stdafx.h"
#include "BreakdownItemAssassinate.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemAssassinate::BreakdownItemAssassinate(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem)
{
    // assassinate uses best of dexterity/intelligence
    AddAbility(Ability_Dexterity);
    AddAbility(Ability_Intelligence);
}

BreakdownItemAssassinate::~BreakdownItemAssassinate()
{
}

// required overrides
CString BreakdownItemAssassinate::Title() const
{
    CString text("Assassinate");
    return text;
}

CString BreakdownItemAssassinate::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    return value;
}

void BreakdownItemAssassinate::CreateOtherEffects()
{
    // add standard tactical ability based on a stat
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        // Base ability bonus to tactical DC
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

        // assassinate also includes Rogue level
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
        if (classLevels[Class_Rogue] != 0)
        {
            ActiveEffect feat(
                    Bonus_class,
                    "Rogue levels",
                    1,
                    classLevels[Class_Rogue],
                    "");        // no tree
            AddOtherEffect(feat);
        }
    }
}

bool BreakdownItemAssassinate::AffectsUs(const Effect & effect) const
{
    // return true if the effect applies
    bool isUs = false;
    if (effect.Type() == Effect_Assassinate)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemAssassinate::UpdateClassChanged(
        Character * charData,
        ClassType type, size_t
        level)
{
    BreakdownItem::UpdateClassChanged(charData, type, level);
    // need to re-create other effects list
    CreateOtherEffects();
    Populate();
}


// BreakdownObserver overrides
void BreakdownItemAssassinate::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    // a stat ability this breakdown uses as a component has changed
    CreateOtherEffects();
    Populate();
}

