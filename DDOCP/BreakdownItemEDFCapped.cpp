// BreakdownItemEDFCapped.cpp
//
#include "stdafx.h"
#include "BreakdownItemEDFCapped.h"
#include "GlobalSupportFunctions.h"

BreakdownItemEDFCapped::BreakdownItemEDFCapped(
        BreakdownType type,
        EffectType effect,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItemSimple(type, effect, title, treeList, hItem)
{
}

BreakdownItemEDFCapped::~BreakdownItemEDFCapped()
{
}

CString BreakdownItemEDFCapped::Value() const
{
    // EDF was removed and can no longer Cap an item total
    CString value;
    // get the base class to format it
    value = BreakdownItemSimple::Value();
    return value;
}

void BreakdownItemEDFCapped::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        if (m_effect == Effect_DoubleShot
                && m_pCharacter->IsFeatTrained("Manyshot"))
        {
            BreakdownItem * pBBAB = FindBreakdown(Breakdown_BAB);
            ASSERT(pBBAB != NULL);
            pBBAB->AttachObserver(this);  // need to know about changes to this effect
            double amount = pBBAB->Total();
            ActiveEffect ds(
                    Bonus_feat,
                    "Manyshot (BAB * 1.5)",
                    1,
                    (int)(amount * 1.5),        // drop fractions
                    "");        // no tree
            ds.AddAnyOfStance("Shortbow");
            ds.AddAnyOfStance("Longbow");
            AddOtherEffect(ds);
        }
        if (m_effect == Effect_RangedPower
                && m_pCharacter->IsFeatTrained("Rapid Shot"))
        {
            BreakdownItem * pBBAB = FindBreakdown(Breakdown_BAB);
            ASSERT(pBBAB != NULL);
            pBBAB->AttachObserver(this);  // need to know about changes to this effect
            double amount = pBBAB->Total();
            ActiveEffect rp(
                    Bonus_feat,
                    "Rapid Shot (BAB * 1.5)",
                    1,
                    (int)(amount * 1.5),        // drop fractions
                    "");        // no tree
            rp.AddAnyOfStance("Shortbow");
            rp.AddAnyOfStance("Longbow");
            AddOtherEffect(rp);
        }
    }
}

void BreakdownItemEDFCapped::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    CreateOtherEffects();
    BreakdownItemSimple::UpdateClassChanged(charData, classFrom, classTo, level);
    Populate();
}

void BreakdownItemEDFCapped::UpdateFeatTrained(
        Character * charData,
        const std::string & featName)
{
    CreateOtherEffects();
    BreakdownItemSimple::UpdateFeatTrained(charData, featName);
    Populate();
}

void BreakdownItemEDFCapped::UpdateFeatRevoked(
        Character * charData,
        const std::string & featName)
{
    CreateOtherEffects();
    BreakdownItemSimple::UpdateFeatRevoked(charData, featName);
    Populate();
}

