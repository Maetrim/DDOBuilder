// BreakdownItemBAB.cpp
//
#include "stdafx.h"
#include "BreakdownItemBAB.h"

#include "GlobalSupportFunctions.h"

BreakdownItemBAB::BreakdownItemBAB(
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(Breakdown_BAB, treeList, hItem),
    m_overrideBabCount(0)
{
}

BreakdownItemBAB::~BreakdownItemBAB()
{
}

// required overrides
CString BreakdownItemBAB::Title() const
{
    CString text;
    text = "Base Attack Bonus";
    return text;
}

CString BreakdownItemBAB::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    return value;
}

void BreakdownItemBAB::CreateOtherEffects()
{
    // add class hit points
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            if (classLevels[ci] > 0)
            {
                std::string className = EnumEntryText((ClassType)ci, classTypeMap);
                className += " Levels";
                double classBab = BAB((ClassType)ci);
                ActiveEffect classBonus(
                        Bonus_class,
                        className,
                        classLevels[ci],
                        classBab,
                        "");        // no tree
                classBonus.SetWholeNumbersOnly();
                AddOtherEffect(classBonus);
            }
        }

        // must be proficient with the weapon
        if (m_overrideBabCount > 0)
        {
            // have at least 1 enhancement that boosts BAB to Character level
            size_t currentBab = m_pCharacter->BaseAttackBonus(MAX_LEVEL);
            ActiveEffect amountTrained(
                    Bonus_enhancement,
                    "BAB boost to character level",
                    1,
                    MAX_LEVEL - currentBab - 5,
                    "");        // no tree
            AddOtherEffect(amountTrained);
        }
    }
}

bool BreakdownItemBAB::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this feat effect applies to us, if so add it
    if (effect.Type() == Effect_OverrideBAB)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemBAB::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // need to re-create other effects list
    CreateOtherEffects();
    Populate();
}

void BreakdownItemBAB::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle checking for override BAB
    if (effect.Type() == Effect_OverrideBAB)
    {
        ++m_overrideBabCount;
        CreateOtherEffects();
    }
    // pass through to the base class
    //BreakdownItem::UpdateFeatEffect(pCharacter, featName, effect);
}

void BreakdownItemBAB::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // handle checking for override BAB
    if (effect.Type() == Effect_OverrideBAB)
    {
        --m_overrideBabCount;
        CreateOtherEffects();
    }
    // pass through to the base class
    //BreakdownItem::UpdateFeatEffectRevoked(pCharacter, featName, effect);
}

void BreakdownItemBAB::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle checking for override BAB
    if (effect.Type() == Effect_OverrideBAB)
    {
        ++m_overrideBabCount;
        CreateOtherEffects();
    }
    // pass through to the base class
    //BreakdownItem::UpdateItemEffect(pCharacter, itemName, effect);
}

void BreakdownItemBAB::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // handle checking for override BAB
    if (effect.Type() == Effect_OverrideBAB)
    {
        --m_overrideBabCount;
        CreateOtherEffects();
    }
    // pass through to the base class
    //BreakdownItem::UpdateItemEffectRevoked(pCharacter, itemName, effect);
}

void BreakdownItemBAB::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle checking for override BAB
    if (effect.m_effect.Type() == Effect_OverrideBAB)
    {
        ++m_overrideBabCount;
        CreateOtherEffects();
    }
    // pass through to the base class
    //BreakdownItem::UpdateEnhancementEffect(pCharacter, enhancementName, effect);
}

void BreakdownItemBAB::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // handle checking for override BAB
    if (effect.m_effect.Type() == Effect_OverrideBAB)
    {
        --m_overrideBabCount;
        CreateOtherEffects();
    }
    // pass through to the base class
    //BreakdownItem::UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
}
