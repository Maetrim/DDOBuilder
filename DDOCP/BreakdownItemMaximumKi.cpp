// BreakdownItemMaximumKi.cpp
//
#include "stdafx.h"
#include "BreakdownItemMaximumKi.h"

#include "GlobalSupportFunctions.h"

BreakdownItemMaximumKi::BreakdownItemMaximumKi(
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(Breakdown_KiMaximum, treeList, hItem)
{
}

BreakdownItemMaximumKi::~BreakdownItemMaximumKi()
{
}

// required overrides
CString BreakdownItemMaximumKi::Title() const
{
    CString text;
    text = "Maximum Ki";
    return text;
}

CString BreakdownItemMaximumKi::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    return value;
}

void BreakdownItemMaximumKi::CreateOtherEffects()
{
    // add class hit points
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        m_itemEffects.clear();
        // monk level bonus
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(m_pCharacter->MaxLevel());
        size_t monkLevels = classLevels[Class_Monk];
        if (monkLevels > 0)
        {
            // monks get Ki bonus from Wisdom
            BreakdownItem * pWisdom = FindBreakdown(Breakdown_Wisdom);
            if (pWisdom != NULL)
            {
                pWisdom->AttachObserver(this);      // need to track changes
                int total = static_cast<int>(pWisdom->Total());
                int wisStatBonus =  BaseStatToBonus(total);
                ActiveEffect wisBonus(
                        Bonus_ability,
                        "Wisdom bonus x5",
                        1,
                        wisStatBonus * 5,
                        "");        // no tree
                AddItemEffect(wisBonus);
            }
            ActiveEffect classBonus(
                    Bonus_class,
                    "Monk levels x10",
                    monkLevels,
                    10,
                    "");        // no tree
            AddOtherEffect(classBonus);
        }
        // sacred fist level bonus
        size_t sacredFistLevels = classLevels[Class_PaladinSacredFist];
        if (sacredFistLevels > 0)
        {
            // Sacred Fists get Ki bonus from Charisma
            BreakdownItem * pCharisma = FindBreakdown(Breakdown_Charisma);
            if (pCharisma != NULL)
            {
                pCharisma->AttachObserver(this);      // need to track changes
                int total = static_cast<int>(pCharisma->Total());
                int chaStatBonus =  BaseStatToBonus(total);
                ActiveEffect chaBonus(
                        Bonus_ability,
                        "Charisma bonus x5",
                        1,
                        chaStatBonus * 5,
                        "");        // no tree
                AddItemEffect(chaBonus);
            }
            ActiveEffect classBonus(
                    Bonus_class,
                    "Sacred Fist levels x10",
                    sacredFistLevels,
                    10,
                    "");        // no tree
            AddOtherEffect(classBonus);
        }
        ActiveEffect baseBonus(
                Bonus_base,
                "Standard Max Ki",
                1,
                40,
                "");        // no tree
        AddOtherEffect(baseBonus);
    }
}

bool BreakdownItemMaximumKi::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this feat effect applies to us, if so add it
    if (effect.Type() == Effect_KiMaximum)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemMaximumKi::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // need to re-create other effects list (class hp)
    CreateOtherEffects();
    Populate();
}

void BreakdownItemMaximumKi::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // total constitution has changed, update
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}
