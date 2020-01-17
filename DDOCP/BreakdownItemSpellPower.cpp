// BreakdownItemSpellPower.cpp
//
#include "stdafx.h"
#include "BreakdownItemSpellPower.h"
#include "BreakdownItemSkill.h"
#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"
#include "BreakdownItemWeaponEffects.h"

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
        // these bonus's only applies for spell powers, not critical chances etc
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

            // Battle engineer core 6: 
            // Your equipped weapon in your main hand is now a Spellcasting Implement,
            // providing a +3 Implement bonus to Universal Spell Power for every +1
            // Enhancement bonus on the weapon.
            // find the main hand weapon breakdown
            if (m_pCharacter->IsEnhancementTrained("BECore6", "")
                    || m_pCharacter->IsEnhancementTrained("BomCore2", ""))
            {
                // get the main hand weapon breakdown
                BreakdownItem * pBI = FindBreakdown(Breakdown_WeaponEffectHolder);
                BreakdownItemWeaponEffects * pBIW = dynamic_cast<BreakdownItemWeaponEffects*>(pBI);
                if (pBIW != NULL)
                {
                    pBI = pBIW->GetWeaponBreakdown(true, Breakdown_WeaponAttackBonus);
                    if (pBI != NULL)
                    {
                        int weaponPlus = (int)pBI->GetEffectValue(Bonus_weaponEnchantment);
                        ActiveEffect implementBonus(
                                Bonus_implement,
                                "Battle Engineer: Master Engineer",
                                weaponPlus,
                                3,          // +3 per weapon plus
                                "");        // no tree
                        AddOtherEffect(implementBonus);
                    }
                }
            }
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

void BreakdownItemSpellPower::UpdateEnhancementTrained(
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
    // check for "Battle Engineer: Master Engineer"  being trained specifically
    if (enhancementName == "BECore6")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemSpellPower::UpdateEnhancementRevoked(
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
    // check for "Battle Engineer: Master Engineer"  being revoked specifically
    if (enhancementName == "BECore6")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemSpellPower::UpdateGearChanged(Character * charData, InventorySlotType slot)
{
    BreakdownItem::UpdateGearChanged(
            charData,
            slot);
    if (slot == Inventory_Weapon1)
    {
        // if "Battle Engineer: Master Engineer" is trained, the implement bonus may have changed
        if (m_pCharacter->IsEnhancementTrained("BECore6", ""))
        {
            // need to re-create other effects list
            CreateOtherEffects();
            Populate();
        }
    }
}

