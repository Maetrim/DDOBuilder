// BreakdownItemSpellPoints.cpp
//
#include "stdafx.h"
#include "BreakdownItemSpellPoints.h"
#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemSpellPoints::BreakdownItemSpellPoints(
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

BreakdownItemSpellPoints::~BreakdownItemSpellPoints()
{
}

// required overrides
CString BreakdownItemSpellPoints::Title() const
{
    return m_title;
}

CString BreakdownItemSpellPoints::Value() const
{
    CString value;
    value.Format(
            "%3d",
            (int)Total());
    return value;
}

void BreakdownItemSpellPoints::CreateOtherEffects()
{
    m_otherEffects.clear();
    // create the class specific spell point contributions
    if (m_pCharacter != NULL)
    {
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            // class level spell points
            if (classLevels[ci] > 0) // must have some levels in the class
            {
                //?? should this be based of the breakdown for this class caster level?
                size_t classSpellPoints = ClassSpellPoints((ClassType)ci, classLevels[ci]);
                if (classSpellPoints > 0)
                {
                    CString bonusName;
                    bonusName.Format("%s(%d) Spell points",
                            EnumEntryText((ClassType)ci, classTypeMap),
                            classLevels[ci]);
                    ActiveEffect classSps(
                            Bonus_class,
                            (LPCSTR)bonusName,
                            1,
                            classSpellPoints,
                            "");        // no tree
                    AddOtherEffect(classSps);
                    // if we have class spell points we must have bonus class ability spell points
                    // from the DDOWiki:
                    // Your casting ability grants you a number of bonus spell points equal
                    // to (caster level + 9) * (casting ability modifier)
                    AbilityType at = ClassCastingStat((ClassType)ci);
                    BreakdownItem * pBI = FindBreakdown(StatToBreakdown(at));
                    ASSERT(pBI != NULL);
                    pBI->AttachObserver(this);  // need to know about changes to this stat
                    int abilityBonus = BaseStatToBonus(pBI->Total());
                    int abilitySps = (classLevels[ci] + 9) * abilityBonus;
                    if (abilitySps != 0)
                    {
                        bonusName.Format("%s(%s) ability Spell points",
                                EnumEntryText((ClassType)ci, classTypeMap),
                                EnumEntryText((AbilityType)at, abilityTypeMap));
                        ActiveEffect abilitySpBonus(
                                Bonus_ability,
                                (LPCSTR)bonusName,
                                1,
                                abilitySps,
                                "");        // no tree
                        AddOtherEffect(abilitySpBonus);
                    }
                }
            }
        }
   }
}

bool BreakdownItemSpellPoints::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect)
    {
        isUs = true;
    }
    return isUs;
}

// CharacterObserver overrides
void BreakdownItemSpellPoints::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // if a class has changed, just recreate our other effects
    // this will update spell point totals
    CreateOtherEffects();
}

void BreakdownItemSpellPoints::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // just re-create our other effects
    CreateOtherEffects();
}

double BreakdownItemSpellPoints::Multiplier() const
{
    // Note that favored souls and sorcerers get up to double spell points
    // from item effects
    size_t fvsLevels = m_pCharacter->ClassLevels(Class_FavoredSoul);
    size_t sorcLevels = m_pCharacter->ClassLevels(Class_Sorcerer);
    double factor = 1.0 + (double)(fvsLevels + sorcLevels) / (double)MAX_CLASS_LEVEL;
    return factor;
}

