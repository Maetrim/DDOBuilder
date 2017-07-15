// BreakdownItemEnergyResistance.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemEnergyResistance :
        public BreakdownItem
{
    public:
        BreakdownItemEnergyResistance(
                BreakdownType type,
                EffectType effect,
                EnergyType ssType,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemEnergyResistance();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        CString m_title;
        EffectType m_effect;
        EnergyType m_energyType;
};
