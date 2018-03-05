// BreakdownItemDR.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemDR :
        public BreakdownItem
{
    public:
        BreakdownItemDR(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemDR();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        void AddEffectToString(
                CString * value,
                const ActiveEffect & effect) const;
};
