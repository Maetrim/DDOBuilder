// BreakdownItemDodge.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemDodge :
        public BreakdownItem
{
    public:
        BreakdownItemDodge(
                BreakdownType type,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemDodge();

        virtual double CappedTotal() const override;
        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        CString m_title;
};
