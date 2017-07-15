// BreakdownItemSimple.cpp
//
#include "stdafx.h"
#include "BreakdownItemSimple.h"

BreakdownItemSimple::BreakdownItemSimple(
        BreakdownType type,
        EffectType effect,
        const CString & title,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_title(title),
    m_effect(effect)
{
}

BreakdownItemSimple::~BreakdownItemSimple()
{
}

// required overrides
CString BreakdownItemSimple::Title() const
{
    return m_title;
}

CString BreakdownItemSimple::Value() const
{
    CString value;
    value.Format(
            "%3d",
            (int)Total());
    return value;
}

void BreakdownItemSimple::CreateOtherEffects()
{
    // no other effects for this simple item
    m_otherEffects.clear();
}

bool BreakdownItemSimple::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    if (effect.Type() == m_effect)
    {
        isUs = true;
    }
    return isUs;
}
