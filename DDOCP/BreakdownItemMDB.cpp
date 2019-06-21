// BreakdownItemMDB.cpp
//
#include "stdafx.h"
#include "BreakdownItemMDB.h"

#include "GlobalSupportFunctions.h"

BreakdownItemMDB::BreakdownItemMDB(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_bNoLimit(false)
{
}

BreakdownItemMDB::~BreakdownItemMDB()
{
}

// required overrides
CString BreakdownItemMDB::Title() const
{
    CString text = "Max Dex Bonus";
    return text;
}

CString BreakdownItemMDB::Value() const
{
    CString value;
    if (m_bNoLimit)
    {
        value = "No limit";
    }
    else
    {
        value.Format(
                "%3d",
                (int)Total());
    }
    return value;
}

void BreakdownItemMDB::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        const Character & charData = *m_pCharacter;
        m_bNoLimit = charData.IsStanceActive("Cloth Armor");
    }
}

bool BreakdownItemMDB::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_MaxDexBonus)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemMDB::UpdateGearChanged(
        Character * charData,
        InventorySlotType slot)
{
    // update if equipped armor changes
    if (slot == Inventory_Armor)
    {
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemMDB::UpdateFeatTrained(
        Character * charData,
        const std::string & featName)
{
    // Warforged Adamantine body can effect MDB
    if (featName == "Adamantine Body")
    {
        CreateOtherEffects();
        Populate();
    }
}

void BreakdownItemMDB::UpdateFeatRevoked(
        Character * charData,
        const std::string & featName)
{
    // Warforged Adamantine body can effect MDB
    if (featName == "Adamantine Body")
    {
        CreateOtherEffects();
        Populate();
    }
}

