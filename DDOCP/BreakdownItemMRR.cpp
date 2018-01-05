// BreakdownItemMRR.cpp
//
#include "stdafx.h"
#include "BreakdownItemMRR.h"
#include "GlobalSupportFunctions.h"

BreakdownItemMRR::BreakdownItemMRR(
        BreakdownType type,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title)
{
}

BreakdownItemMRR::~BreakdownItemMRR()
{
}

// required overrides
CString BreakdownItemMRR::Title() const
{
    return m_title;
}

CString BreakdownItemMRR::Value() const
{
    BreakdownItem * pBI = FindBreakdown(Breakdown_MRRCap);
    double mrr = (int)Total();
    CString mrrCap = pBI->Value();
    bool capped = false;
    if (mrrCap != "None") // None if no cap in force
    {
        // check to see if hit cap
        if (pBI->Total() < mrr)
        {
            mrr = pBI->Total();
            capped = true;
        }
    }
    // Example 133 (57.1%)
    double decrease = 100.0 - (100.0 / (100.0 + mrr)) * 100;
    CString value;
    value.Format(
            "%3d (%.1f%%)",
            (int)mrr,
            decrease);
    if (capped)
    {
        value += " (Capped)";
    }
    return value;
}

void BreakdownItemMRR::CreateOtherEffects()
{
    m_otherEffects.clear();
    // we need to know when this breakdown value changes
    BreakdownItem * pBI = FindBreakdown(Breakdown_MRRCap);
    pBI->AttachObserver(this);
}

bool BreakdownItemMRR::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == Effect_MRR)
    {
        isUs = true;
    }
    return isUs;
}
