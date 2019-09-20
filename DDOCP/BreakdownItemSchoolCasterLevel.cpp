// BreakdownItemCasterLevel.cpp
//
#include "stdafx.h"
#include "BreakdownItemSchoolCasterLevel.h"

#include "GlobalSupportFunctions.h"

BreakdownItemSchoolCasterLevel::BreakdownItemSchoolCasterLevel(
        SpellSchoolType schoolType,
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_school(schoolType),
    m_maxCasterLevel(type, Effect_MaxCasterLevel, "", NULL, hItem)
{
    m_maxCasterLevel.AttachObserver(this);
}

BreakdownItemSchoolCasterLevel::~BreakdownItemSchoolCasterLevel()
{
}

void BreakdownItemSchoolCasterLevel::SetCharacter(Character * charData, bool observe)
{
    // pass through to the base class
    BreakdownItem::SetCharacter(charData, observe);
    // and set on our owned element
    m_maxCasterLevel.SetCharacter(charData, false);
}

// required overrides
CString BreakdownItemSchoolCasterLevel::Title() const
{
    CString text = EnumEntryText(m_school, spellSchoolTypeMap);
    return text;
}

CString BreakdownItemSchoolCasterLevel::Value() const
{
    CString value;
    value.Format(
            "%+3d",          // Caster level values are always whole numbers
            (int)Total());
    if (m_maxCasterLevel.Total() != 0)
    {
        CString strMax;
        strMax.Format(" (Max %+d)", (int)m_maxCasterLevel.Total());
        value += strMax;
    }
    return value;
}

void BreakdownItemSchoolCasterLevel::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
    }
}

bool BreakdownItemSchoolCasterLevel::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_CasterLevel
            && effect.HasSchool()
            && effect.School() == m_school)
    {
        // it is a caster level school bonus
        isUs = true;
    }
    return isUs;
}

void BreakdownItemSchoolCasterLevel::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.HasSchool()
            && effect.School() == m_school)
    {
        m_maxCasterLevel.UpdateFeatEffect(pCharacter, featName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateFeatEffect(pCharacter, featName, effect);
}

void BreakdownItemSchoolCasterLevel::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.HasSchool()
            && effect.School() == m_school)
    {
        m_maxCasterLevel.UpdateFeatEffectRevoked(pCharacter, featName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateFeatEffectRevoked(pCharacter, featName, effect);
}

void BreakdownItemSchoolCasterLevel::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.HasSchool()
            && effect.School() == m_school)
    {
        m_maxCasterLevel.UpdateItemEffect(pCharacter, itemName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateItemEffect(pCharacter, itemName, effect);
}

void BreakdownItemSchoolCasterLevel::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.HasSchool()
            && effect.School() == m_school)
    {
        m_maxCasterLevel.UpdateItemEffectRevoked(pCharacter, itemName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateItemEffectRevoked(pCharacter, itemName, effect);
}

void BreakdownItemSchoolCasterLevel::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_MaxCasterLevel
            && effect.m_effect.HasSchool()
            && effect.m_effect.School() == m_school)
    {
        m_maxCasterLevel.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateEnhancementEffect(pCharacter, enhancementName, effect);
}

void BreakdownItemSchoolCasterLevel::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_MaxCasterLevel
            && effect.m_effect.HasSchool()
            && effect.m_effect.School() == m_school)
    {
        m_maxCasterLevel.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
}

void BreakdownItemSchoolCasterLevel::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // max caster level bonus has changed
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
    // a no fail on a 1 may have changed, ensure updates
    Populate();
}

void BreakdownItemSchoolCasterLevel::AppendItems(CListCtrl * pControl)
{
    if (m_maxCasterLevel.Total() != 0)
    {
        // add the stuff for max caster level below the caster level stuff
        size_t start = pControl->GetItemCount();
        pControl->InsertItem(start, "");
        pControl->InsertItem(start + 1, "Max Caster Level");
        m_maxCasterLevel.AddItems(pControl);
    }
}

int BreakdownItemSchoolCasterLevel::MaxTotal() const
{
    return (int)m_maxCasterLevel.Total();
}
