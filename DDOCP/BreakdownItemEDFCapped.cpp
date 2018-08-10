// BreakdownItemEDFCapped.cpp
//
#include "stdafx.h"
#include "BreakdownItemEDFCapped.h"

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
    CString value;
    // is this item capped to 0?
    if (m_pCharacter != NULL
            && m_pCharacter->IsStanceActive("Epic Defensive Fighting"))
    {
        // its capped due to EDF being enabled
        value.Format(
                "0 (Capped %.2f)",
                Total());
    }
    else
    {
        // get he base class to format it
        value = BreakdownItemSimple::Value();
    }
    return value;
}
