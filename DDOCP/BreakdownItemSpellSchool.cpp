// BreakdownItemSpellSchool.cpp
//
#include "stdafx.h"
#include "BreakdownItemSpellSchool.h"

BreakdownItemSpellSchool::BreakdownItemSpellSchool(
        BreakdownType type,
        EffectType effect,
        SpellSchoolType ssType,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect),
    m_spellSchoolType(ssType)
{
}

BreakdownItemSpellSchool::~BreakdownItemSpellSchool()
{
}

// required overrides
CString BreakdownItemSpellSchool::Title() const
{
    return m_title;
}

CString BreakdownItemSpellSchool::Value() const
{
    CString value;
    value.Format(
            "%3d",
            (int)Total());
    return value;
}

void BreakdownItemSpellSchool::CreateOtherEffects()
{
    m_otherEffects.clear();
    if (m_spellSchoolType != SpellSchool_GlobalDC)
    {
        // all spell DC's start at a base of 10
        ActiveEffect amountTrained(
                Bonus_base,
                "Base DC",
                1,
                10,
                "");        // no tree
        AddOtherEffect(amountTrained);
    }
}

bool BreakdownItemSpellSchool::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect
            && (effect.School() == m_spellSchoolType
                || effect.School() == SpellSchool_All))
    {
        isUs = true;
    }
    return isUs;
}
