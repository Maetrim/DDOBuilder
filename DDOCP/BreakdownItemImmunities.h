// BreakdownItemImmunities.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemImmunities :
        public BreakdownItem
{
    public:
        BreakdownItemImmunities(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemImmunities();

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
