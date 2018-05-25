// BreakdownItemSpellPower.cpp
//
#include "stdafx.h"
#include "BreakdownItemSpellPower.h"
#include "BreakdownItemSkill.h"
#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemSpellPower::BreakdownItemSpellPower(
        BreakdownType type,
        EffectType effect,
        SpellPowerType spType,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect),
    m_spellPowerType(spType)
{
}

BreakdownItemSpellPower::~BreakdownItemSpellPower()
{
}

// required overrides
CString BreakdownItemSpellPower::Title() const
{
    return m_title;
}

CString BreakdownItemSpellPower::Value() const
{
    CString value;

    value.Format(
            "%.2f",             // spell power can have fractional values to 2 dp
            Total());

    return value;
}

BreakdownType BreakdownItemSpellPower::SpellPowerBreakdown() const
{
    // from the wiki:
    // Skill bonuses to spell power
    // Heal increases your Positive and Negative spell power 
    // Perform increases your Sonic spell power 
    // Repair increases your Repair and Rust spell power 
    // Spellcraft affects everything else 

    // assume spellcraft
    BreakdownType bt = Breakdown_SkillSpellCraft;
    switch (m_spellPowerType)
    {
    case SpellPower_Positive:
    case SpellPower_Negative:
        bt = Breakdown_SkillHeal;
        break;
    case SpellPower_Sonic:
        bt = Breakdown_SkillPerform;
        break;
    case SpellPower_Repair:
    case SpellPower_Rust:
        bt = Breakdown_SkillRepair;
        break;
    default:
        // all the rest are spell craft
        break;
    }
    return bt;
}

void BreakdownItemSpellPower::CreateOtherEffects()
{
    m_otherEffects.clear();
    if (m_pCharacter != NULL)
    {
        // this bonus only applies for spell powers, not critical chances etc
        if (m_effect == Effect_SpellPower)
        {
            // specific skill only affects spell power (used for critical and multiplier also)
            BreakdownType bt = SpellPowerBreakdown();
            BreakdownItemSkill * pBI = dynamic_cast<BreakdownItemSkill*>(FindBreakdown(bt));
            CString text;
            text.Format("%s skill bonus",
                    EnumEntryText(pBI->Skill(), skillTypeMap));
            pBI->AttachObserver(this);      // need to observe changes
            ActiveEffect levels(
                    Bonus_skill,
                    (LPCTSTR)text,
                    1,
                    pBI->Total(),
                    "");        // no tree
            AddOtherEffect(levels);
        }
    }
}

bool BreakdownItemSpellPower::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect
            && effect.IncludesSpellPower(m_spellPowerType))
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemSpellPower::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // call base class also
    BreakdownItem::UpdateTotalChanged(item, type);
    CreateOtherEffects();
}

