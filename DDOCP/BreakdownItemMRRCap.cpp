// BreakdownItemMRRCap.cpp
//
#include "stdafx.h"
#include "BreakdownItemMRRCap.h"
#include "GlobalSupportFunctions.h"

BreakdownItemMRRCap::BreakdownItemMRRCap(
        BreakdownType type,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title)
{
}

BreakdownItemMRRCap::~BreakdownItemMRRCap()
{
}

// required overrides
CString BreakdownItemMRRCap::Title() const
{
    return m_title;
}

CString BreakdownItemMRRCap::Value() const
{
    // value shown if a cap applies else shows "None"
    CString value;
    if (m_pCharacter != NULL)
    {
        if (m_pCharacter->IsStanceActive("Cloth Armor")
                || m_pCharacter->IsStanceActive("Light Armor"))
        {
            value.Format(
                    "%3d",
                    (int)Total());
        }
        else
        {
            value = "None";
        }
    }
    return value;
}

void BreakdownItemMRRCap::CreateOtherEffects()
{
    m_otherEffects.clear();
    if (m_pCharacter != NULL)
    {
        if (m_pCharacter->IsStanceActive("Cloth Armor"))
        {
            // cap of 50 for no or cloth armor
            ActiveEffect cloth(
                    Bonus_armor,
                    "No/Cloth Armor Cap",
                    1,
                    50,
                    "");        // no tree
            AddOtherEffect(cloth);
        }
        if (m_pCharacter->IsStanceActive("Light Armor"))
        {
            // cap of 100 for light armor
            ActiveEffect cloth(
                    Bonus_armor,
                    "Light Armor Cap",
                    1,
                    100,
                    "");        // no tree
            AddOtherEffect(cloth);
        }
        // no cap for medium armor
        // no cap for heavy armor
    }
}

bool BreakdownItemMRRCap::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == Effect_MRRCap)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemMRRCap::UpdateStanceActivated(
        Character * charData,
        const std::string & stanceName)
{
    CreateOtherEffects();
    Populate();
}

void BreakdownItemMRRCap::UpdateStanceDeactivated(
        Character * charData,
        const std::string & stanceName)
{
    CreateOtherEffects();
    Populate();
}
