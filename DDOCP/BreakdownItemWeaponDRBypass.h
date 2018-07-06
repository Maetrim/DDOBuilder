// BreakdownItemWeaponDRBypass.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemWeaponDRBypass :
        public BreakdownItem
{
    public:
        BreakdownItemWeaponDRBypass(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemWeaponDRBypass();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        void AddEffectToVector(
                std::vector<std::string> * value,
                const ActiveEffect & effect) const;
};
