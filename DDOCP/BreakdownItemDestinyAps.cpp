// BreakdownItemDestinyAps.cpp
//
#include "stdafx.h"
#include "BreakdownItemDestinyAps.h"

#include "GlobalSupportFunctions.h"

BreakdownItemDestinyAps::BreakdownItemDestinyAps(
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(Breakdown_DestinyPoints, treeList, hItem)
{
    // need to know when total fate points changes
    FindBreakdown(Breakdown_U51FatePoints)->AttachObserver(this);
}

BreakdownItemDestinyAps::~BreakdownItemDestinyAps()
{
}

// required overrides
CString BreakdownItemDestinyAps::Title() const
{
    CString text;
    text = "U51 Destiny APs";
    return text;
}

CString BreakdownItemDestinyAps::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    return value;
}

void BreakdownItemDestinyAps::CreateOtherEffects()
{
    // add class hit points
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();

        // Fate point bonus to Destiny APs
        double fatePoints = FindBreakdown(Breakdown_U51FatePoints)->Total();
        double bonus = static_cast<int>(fatePoints / 3.0);  // drop fractions
        if (bonus != 0)
        {
            ActiveEffect fpBonus(
                    Bonus_special,
                    "Fate Points / 3",
                    1,
                    bonus,
                    "");        // no tree
            AddOtherEffect(fpBonus);
        }
    }
}

bool BreakdownItemDestinyAps::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this feat effect applies to us, if so add it
    if (effect.Type() == Effect_DestinyAPBonus)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemDestinyAps::UpdateTotalChanged(
        BreakdownItem * item,
        BreakdownType type)
{
    // total fate points has changed, update
    CreateOtherEffects();
    // do base class stuff also
    BreakdownItem::UpdateTotalChanged(item, type);
}
