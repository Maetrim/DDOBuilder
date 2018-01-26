// BreakdownItemWeaponOtherDamageEffects.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemWeaponOtherDamageEffects :
        public BreakdownItem
{
    public:
        BreakdownItemWeaponOtherDamageEffects(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemWeaponOtherDamageEffects();

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
