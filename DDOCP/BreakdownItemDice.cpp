// BreakdownItemDice.cpp
//
#include "stdafx.h"
#include "BreakdownItemDice.h"

BreakdownItemDice::BreakdownItemDice(
        BreakdownType type,
        EffectType effect,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect)
{
}

BreakdownItemDice::~BreakdownItemDice()
{
}

// required overrides
CString BreakdownItemDice::Title() const
{
    return m_title;
}

CString BreakdownItemDice::Value() const
{
    CString value;
    value = SumDice();  // its the sum of all Dice effects
    return value;
}

void BreakdownItemDice::CreateOtherEffects()
{
    // no other effects for this simple item
    m_otherEffects.clear();
}

bool BreakdownItemDice::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect)
    {
        isUs = true;
    }
    return isUs;
}

CString BreakdownItemDice::SumDice() const
{
    // get a list of all the active effects first
    // build a list of all the current active effects
    std::list<ActiveEffect> allActiveEffects = AllActiveEffects();
    // now we have the list look for and sum all dice effects which can be combined
    std::list<Dice> dice;
    std::list<ActiveEffect>::iterator it = allActiveEffects.begin();
    while (it != allActiveEffects.end())
    {
        // is this dice setup already present in the list?
        if ((*it).Type() == ET_dice)
        {
            bool found = false;
            std::list<Dice>::iterator dit = dice.begin();
            while (!found && dit != dice.end())
            {
                Dice newDice = (*it).GetDice();
                newDice.StripDown((*it).NumStacks());
                if ((*dit).IsSameDiceType(newDice))
                {
                    // need to add the stacks to this one
                    (*dit).AddStacks((*it).NumStacks());
                    found = true;
                }
                ++dit;
            }
            if (!found)
            {
                // its a new type, add it
                Dice effectDice = (*it).GetDice();
                effectDice.StripDown((*it).NumStacks());    // 1 element vectors used here
                dice.push_back(effectDice);
            }
        }
        ++it;
    }
    std::string text;
    // now show all the dice descriptions
    bool first = true;
    std::list<Dice>::iterator dit = dice.begin();
    while (dit != dice.end())
    {
        if (!first)
        {
            text += " + ";
        }
        text += (*dit).Description(1);
        ++dit;
        first = false;
    }
    return text.c_str();
}

