// BreakdownItemDestinyAps.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemDestinyAps :
        public BreakdownItem
{
    public:
        BreakdownItemDestinyAps(MfcControls::CTreeListCtrl * treeList, HTREEITEM hItem);
        virtual ~BreakdownItemDestinyAps();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        BreakdownItem * m_pConstitutionBreakdown;
};
