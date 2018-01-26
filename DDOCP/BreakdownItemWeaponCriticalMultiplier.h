// BreakdownItemWeaponCriticalMultiplier.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemWeaponCriticalMultiplier :
        public BreakdownItem
{
    public:
        BreakdownItemWeaponCriticalMultiplier(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem,
                BreakdownItem * pBaseTotal);
        virtual ~BreakdownItemWeaponCriticalMultiplier();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
        // BreakdownItemWeaponCriticalMultiplier19-20 needs to know if base range changes
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        BreakdownItem * m_pBaseTotal;
};
