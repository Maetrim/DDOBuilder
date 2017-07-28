// BreakdownItemTactical.cpp
//
#include "stdafx.h"
#include "BreakdownItemTactical.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemTactical::BreakdownItemTactical(
        BreakdownType type,
        TacticalType tactical,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_tacticalType(tactical)
{
    AddAbility(Ability_Strength);
}

BreakdownItemTactical::~BreakdownItemTactical()
{
}

// required overrides
CString BreakdownItemTactical::Title() const
{
    CString text;
    text = EnumEntryText(m_tacticalType, tacticalTypeMap);
    return text;
}

CString BreakdownItemTactical::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    return value;
}

void BreakdownItemTactical::CreateOtherEffects()
{
    // add standard tactical ability based on a stat
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        // all tactical effects have a base DC
        ActiveEffect base(
                Bonus_base,
                "Base DC",
                1,
                10,
                "");        // no tree
        AddOtherEffect(base);

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
    }
}

bool BreakdownItemTactical::AffectsUs(const Effect & effect) const
{
    // return true if the effect applies to this save
    // note that effect that apply to "All" only apply to Fort, reflex and will
    // as the sub-save types use the total from the main category as a part of their total
    // so we do not want to count the bonus twice
    bool isUs = false;
    if (effect.Type() == Effect_TacticalDC)
    {
        isUs = (effect.Tactical() == m_tacticalType
                || effect.Tactical() == Tactical_All);
    }
    return isUs;
}

// BreakdownObserver overrides
void BreakdownItemTactical::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    // a stat ability this breakdown uses as a component has changed
    CreateOtherEffects();
    Populate();
}

