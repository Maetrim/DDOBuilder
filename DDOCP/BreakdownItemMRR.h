// BreakdownItemMRR.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemMRR :
        public BreakdownItem
{
    public:
        BreakdownItemMRR(
                BreakdownType type,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemMRR();

        bool IsMrrCapped() const;
        double MrrCap() const;

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
        virtual void AddInformationItem(CListCtrl * pControl) override;
    private:
        CString m_title;
};
