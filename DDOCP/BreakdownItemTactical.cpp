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
    if (type != Breakdown_TacticalStunningFist)
    {
        AddAbility(Ability_Strength);
    }
    else
    {
        // stunning fist uses wisdom for its ability bonus
        AddAbility(Ability_Wisdom);
    }
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

        // stunning fits also includes 1/2 character level (always 15 currently)
        if (Type() == Breakdown_TacticalStunningFist)
        {
            ActiveEffect feat(
                    Bonus_base,
                    "1/2 Character Level",
                    1,
                    MAX_LEVEL / 2,
                    "");        // no tree
            AddOtherEffect(feat);
        }
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

bool BreakdownItemTactical::AffectsUs(const Effect & effect) const
{
    // return true if the effect applies to this tactical
    bool isUs = false;
    if (effect.Type() == Effect_TacticalDC)
    {
        isUs = (effect.Tactical() == m_tacticalType
                || effect.Tactical() == Tactical_All);
        if (effect.Tactical() == Tactical_StunningBlow
                && Type() == Breakdown_TacticalStunningFist)
        {
            // anything that affects stunning blow also affects stunning fist
        }
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

void BreakdownItemTactical::UpdateEnhancementTrained(
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
    // check for "Divine Might" in Favored Soul War Soul only being trained specifically
    if (enhancementName == "WSDivineMight")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemTactical::UpdateEnhancementRevoked(
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
    // check for "Divine Might" in Favored Soul War Soul only being revoked specifically
    if (enhancementName == "WSDivineMight")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}
