// BreakdownItemEnergyCasterLevel.cpp
//
#include "stdafx.h"
#include "BreakdownItemEnergyCasterLevel.h"

#include "GlobalSupportFunctions.h"

BreakdownItemEnergyCasterLevel::BreakdownItemEnergyCasterLevel(
        EnergyType energyType,
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_energy(energyType),
    m_maxCasterLevel(type, Effect_MaxCasterLevel, "", NULL, hItem)
{
    m_maxCasterLevel.AttachObserver(this);
}

BreakdownItemEnergyCasterLevel::~BreakdownItemEnergyCasterLevel()
{
}

void BreakdownItemEnergyCasterLevel::SetCharacter(Character * charData, bool observe)
{
    // pass through to the base class
    BreakdownItem::SetCharacter(charData, observe);
    // and set on our owned element
    m_maxCasterLevel.SetCharacter(charData, false);
}

// required overrides
CString BreakdownItemEnergyCasterLevel::Title() const
{
    CString text = EnumEntryText(m_energy, energyTypeMap);
    return text;
}

CString BreakdownItemEnergyCasterLevel::Value() const
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

void BreakdownItemEnergyCasterLevel::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
    }
}

bool BreakdownItemEnergyCasterLevel::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_CasterLevel
            && effect.IncludesEnergy(m_energy))
    {
        // it is a caster level bonus for this energy type
        isUs = true;
    }
    return isUs;
}

void BreakdownItemEnergyCasterLevel::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.IncludesEnergy(m_energy))
    {
        m_maxCasterLevel.UpdateFeatEffect(pCharacter, featName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateFeatEffect(pCharacter, featName, effect);
}

void BreakdownItemEnergyCasterLevel::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.IncludesEnergy(m_energy))
    {
        m_maxCasterLevel.UpdateFeatEffectRevoked(pCharacter, featName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateFeatEffectRevoked(pCharacter, featName, effect);
}

void BreakdownItemEnergyCasterLevel::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.IncludesEnergy(m_energy))
    {
        m_maxCasterLevel.UpdateItemEffect(pCharacter, itemName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateItemEffect(pCharacter, itemName, effect);
}

void BreakdownItemEnergyCasterLevel::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    if (effect.Type() == Effect_MaxCasterLevel
            && effect.IncludesEnergy(m_energy))
    {
        m_maxCasterLevel.UpdateItemEffectRevoked(pCharacter, itemName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateItemEffectRevoked(pCharacter, itemName, effect);
}

void BreakdownItemEnergyCasterLevel::UpdateEnhancementEffect(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_MaxCasterLevel
            && effect.m_effect.IncludesEnergy(m_energy))
    {
        m_maxCasterLevel.UpdateEnhancementEffect(pCharacter, enhancementName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateEnhancementEffect(pCharacter, enhancementName, effect);
}

void BreakdownItemEnergyCasterLevel::UpdateEnhancementEffectRevoked(
        Character * pCharacter,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    if (effect.m_effect.Type() == Effect_MaxCasterLevel
            && effect.m_effect.IncludesEnergy(m_energy))
    {
        m_maxCasterLevel.UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
    }
    // pass through to the base class
    BreakdownItem::UpdateEnhancementEffectRevoked(pCharacter, enhancementName, effect);
}

void BreakdownItemEnergyCasterLevel::UpdateTotalChanged(
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

void BreakdownItemEnergyCasterLevel::AppendItems(CListCtrl * pControl)
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

int BreakdownItemEnergyCasterLevel::MaxTotal() const
{
    return (int)m_maxCasterLevel.Total();
}

