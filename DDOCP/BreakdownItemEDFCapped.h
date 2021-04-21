// BreakdownItemSimple.h
//
#pragma once
#include "BreakdownItemSimple.h"

class BreakdownItemEDFCapped :
        public BreakdownItemSimple
{
    public:
        BreakdownItemEDFCapped(
                BreakdownType type,
                EffectType effect,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemEDFCapped();

        // required overrides
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
};
