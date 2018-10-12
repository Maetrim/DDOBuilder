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
            feat.AddStance("Divine Presence");
            feat.SetDivider(2, DT_statBonus);
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
            feat.AddStance("Divine Will");
            feat.SetDivider(2, DT_statBonus);
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
        isUs = effect.IncludesTactical(m_tacticalType);
    }
    return isUs;
}

// BreakdownObserver overrides
void BreakdownItemTactical::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    BreakdownItem::UpdateTotalChanged(item, type);  // call base class
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
