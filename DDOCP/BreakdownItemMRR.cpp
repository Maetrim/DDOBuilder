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
    double mrr = (int)Total();
    if (IsMrrCapped())
    {
        mrr = min(mrr, MrrCap());
    }
    // Example 133 (57.1%)
    double decrease = 100.0 - (100.0 / (100.0 + mrr)) * 100;
    CString value;
    value.Format(
            "%3d (%.1f%%)",
            (int)mrr,
            decrease);
    return value;
}

void BreakdownItemMRR::CreateOtherEffects()
{
    m_otherEffects.clear();
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

void BreakdownItemMRR::AddInformationItem(CListCtrl * pControl)
{
    // add an informational note into the item if this item is capped.
    if (IsMrrCapped())
    {
        // need to add the informational item
        pControl->InsertItem(pControl->GetItemCount(), "");
        pControl->InsertItem(pControl->GetItemCount(), "MRR may be capped due to armor");
        double mrrCap = MrrCap();
        CString text;
        text.Format("Max %d", (int)mrrCap);
        pControl->SetItemText(pControl->GetItemCount() - 1, 1, text);
    }
}

bool BreakdownItemMRR::IsMrrCapped() const
{
    return (Total() > MrrCap());
}

double BreakdownItemMRR::MrrCap() const
{
    // MRR is capped by armor type
    // Cloth = 50
    // Light = 100
    // medium = unlimited
    // heavy = unlimited
    double mrrCap = 0;
    if (m_pCharacter != NULL)
    {
        if (m_pCharacter->IsStanceActive("Cloth Armor"))
        {
            // cap of 50 for no or cloth armor
            mrrCap = 50;
        }
        if (m_pCharacter->IsStanceActive("Light Armor"))
        {
            // cap of 100 for light armor
            mrrCap = 100;
        }
        if (m_pCharacter->IsStanceActive("Medium Armor"))
        {
            // no cap for medium armor
            mrrCap = 999999;    // this number is not shown in the UI unless MRR exceeds it
        }
        if (m_pCharacter->IsStanceActive("Heavy Armor"))
        {
            // no cap for heavy armor
            mrrCap = 999999;    // this number is not shown in the UI unless MRR exceeds it
        }
    }
    return mrrCap;
}
