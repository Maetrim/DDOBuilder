// BreakdownItemTactical.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemTactical:
        public BreakdownItem
{
    public:
        BreakdownItemTactical(
                BreakdownType type,
                TacticalType tactical,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemTactical();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;

        bool IsUs(const Effect & effect) const;
        TacticalType m_tacticalType;
};
