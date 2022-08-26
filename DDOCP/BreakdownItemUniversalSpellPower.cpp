// BreakdownItemUniversalSpellPower.cpp
//
#include "stdafx.h"
#include "BreakdownItemUniversalSpellPower.h"
#include "BreakdownItemSkill.h"
#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"
#include "BreakdownItemWeaponEffects.h"

BreakdownItemUniversalSpellPower::BreakdownItemUniversalSpellPower(
        BreakdownType type,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItemSimple(type, Effect_UniversalSpellPower, title, treeList, hItem)
{
}

BreakdownItemUniversalSpellPower::~BreakdownItemUniversalSpellPower()
{
}

void BreakdownItemUniversalSpellPower::CreateOtherEffects()
{
    m_otherEffects.clear();
    if (m_pCharacter != NULL)
    {
        BreakdownItem * pBIIYH = FindBreakdown(Breakdown_ImplementInYourHands);
        if (pBIIYH != NULL  && pBIIYH->Total() > 0)
        {
            pBIIYH->AttachObserver(this);   // we need to know about changes
            // we do need to list an implement bonus for our main hands weapon
            // based on its weapon plus
            // get the main hand weapon breakdown
            BreakdownItem * pBI = FindBreakdown(Breakdown_WeaponEffectHolder);
            BreakdownItemWeaponEffects * pBIW = dynamic_cast<BreakdownItemWeaponEffects*>(pBI);
            if (pBIW != NULL)
            {
                pBI = pBIW->GetWeaponBreakdown(true, Breakdown_WeaponAttackBonus);
                if (pBI != NULL)
                {
                    int weaponPlus = (int)pBI->GetEffectValue(Bonus_weaponEnchantment);
                    // this is increased by battle engineer cores by +3
                    weaponPlus += 3;

                    // Enchant weapon past life feat can be active
                    size_t count = m_pCharacter->GetSpecialFeatTrainedCount("Past Life: Arcane Sphere: Enchant Weapon");
                    if (m_pCharacter->IsStanceActive("Enchant Weapon", Weapon_Unknown))
                    {
                        weaponPlus += count;
                    }
                    ActiveEffect implementBonus(
                            Bonus_implement,
                            "Implement Bonus",
                            weaponPlus,
                            3,          // +3 per weapon plus
                            "");        // no tree
                    AddOtherEffect(implementBonus);
                }
            }
        }
    }
}

void BreakdownItemUniversalSpellPower::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // call base class also
    BreakdownItemSimple::UpdateTotalChanged(item, type);
    CreateOtherEffects();
}

void BreakdownItemUniversalSpellPower::UpdateEnhancementTrained(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    BreakdownItemSimple::UpdateEnhancementTrained(
            charData,
            enhancementName,
            selection,
            isTier5);
}

void BreakdownItemUniversalSpellPower::UpdateEnhancementRevoked(
        Character * charData,
        const std::string & enhancementName,
        const std::string & selection,
        bool isTier5)
{
    BreakdownItemSimple::UpdateEnhancementRevoked(
            charData,
            enhancementName,
            selection,
            isTier5);
}

void BreakdownItemUniversalSpellPower::UpdateGearChanged(Character * charData, InventorySlotType slot)
{
    BreakdownItemSimple::UpdateGearChanged(
            charData,
            slot);
    if (slot == Inventory_Weapon1)
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemUniversalSpellPower::UpdateStanceActivated(
        Character * charData,
        const std::string & stanceName)
{
    BreakdownItemSimple::UpdateStanceActivated(
            charData,
            stanceName);
    if (stanceName == "Enchant Weapon")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemUniversalSpellPower::UpdateStanceDeactivated(
        Character * charData,
        const std::string & stanceName)
{
    BreakdownItemSimple::UpdateStanceDeactivated(
            charData,
            stanceName);
    if (stanceName == "Enchant Weapon")
    {
        // need to re-create other effects list
        CreateOtherEffects();
        Populate();
    }
}

