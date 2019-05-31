// BreakdownItemPRR.cpp
//
#include "stdafx.h"
#include "BreakdownItemPRR.h"
#include "GlobalSupportFunctions.h"

BreakdownItemPRR::BreakdownItemPRR(
        BreakdownType type,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title)
{
}

BreakdownItemPRR::~BreakdownItemPRR()
{
}

// required overrides
CString BreakdownItemPRR::Title() const
{
    return m_title;
}

CString BreakdownItemPRR::Value() const
{
    // Example 133 (57.1%)
    CString value;
    double decrease = 100.0 - (100.0 / (100.0 + Total())) * 100;
    value.Format(
            "%3d (%.1f%%)",
            (int)Total(),
            decrease);
    return value;
}

void BreakdownItemPRR::CreateOtherEffects()
{
    m_otherEffects.clear();
    if (m_pCharacter != NULL)
    {
        // the armor equipped affects the PRR bonus from BAB
        BreakdownItem * pBBAB = FindBreakdown(Breakdown_BAB);
        ASSERT(pBBAB != NULL);
        pBBAB->AttachObserver(this);  // need to know about changes to this effect
        double amount = pBBAB->Total();
        // Armor type       PRR
        // Cloth armor      0
        // Light armor      BAB * 1
        // Medium armor     BAB * 1.5
        // Heavy armor      BAB * 2
        //
        // Docents          PRR 
        // Mithral Body     BAB * 1
        // Adamantine Body  BAB * 2
        // note that the Mithral body and Adamantine body feats control the state
        // of the stances, so we only need to check them

        if (m_pCharacter->IsStanceActive("Light Armor")
                && (m_pCharacter->IsFeatTrained("Light Armor Proficiency")
                    || m_pCharacter->IsFeatTrained("Mithral Body")))
        {
            ActiveEffect prr(
                    Bonus_armor,
                    "Light Armor PRR (BAB * 1)",
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(prr);
        }
        if (m_pCharacter->IsStanceActive("Medium Armor")
                && m_pCharacter->IsFeatTrained("Medium Armor Proficiency"))
        {
            ActiveEffect prr(
                    Bonus_armor,
                    "Medium Armor PRR (BAB * 1.5)",
                    1,
                    (int)(amount * 1.5),        // drop fractions
                    "");        // no tree
            AddOtherEffect(prr);
        }
        if (m_pCharacter->IsStanceActive("Heavy Armor")
                && (m_pCharacter->IsFeatTrained("Heavy Armor Proficiency")
                    || m_pCharacter->IsFeatTrained("Adamantine Body")))
        {
            ActiveEffect prr(
                    Bonus_armor,
                    "Heavy Armor PRR (BAB * 2)",
                    1,
                    amount * 2.0,
                    "");        // no tree
            AddOtherEffect(prr);
        }
    }
}

bool BreakdownItemPRR::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == Effect_PRR)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemPRR::UpdateStanceActivated(
        Character * charData,
        const std::string & stanceName)
{
    CreateOtherEffects();
    // and finally call the base class
    BreakdownItem::UpdateStanceActivated(charData, stanceName);
}

void BreakdownItemPRR::UpdateStanceDeactivated(
        Character * charData,
        const std::string & stanceName)
{
    CreateOtherEffects();
    // and finally call the base class
    BreakdownItem::UpdateStanceDeactivated(charData, stanceName);
}

// BreakdownObserver overrides
void BreakdownItemPRR::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // BAB has changed
    CreateOtherEffects();
    // and finally call the base class
    BreakdownItem::UpdateTotalChanged(item, type);
}
