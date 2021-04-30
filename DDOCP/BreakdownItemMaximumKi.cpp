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
            AddOtherEffect(wisBonus);
        }
        // monk level bonus
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
        size_t monkLevels = classLevels[Class_Monk];
        if (monkLevels > 0)
        {
            ActiveEffect classBonus(
                    Bonus_class,
                    "Monk levels x10",
                    monkLevels,
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
