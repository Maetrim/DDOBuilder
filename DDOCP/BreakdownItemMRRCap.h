// BreakdownItemMRRCap.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemMRRCap :
        public BreakdownItem
{
    public:
        BreakdownItemMRRCap(
                BreakdownType type,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemMRRCap();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        virtual void UpdateStanceActivated(Character * charData, const std::string & stanceName) override;
        virtual void UpdateStanceDeactivated(Character * charData, const std::string & stanceName) override;
    private:
        CString m_title;
};
