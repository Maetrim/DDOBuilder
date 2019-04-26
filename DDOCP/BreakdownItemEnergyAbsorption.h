// BreakdownItemEnergyAbsorption.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemEnergyAbsorption :
        public BreakdownItem
{
    public:
        BreakdownItemEnergyAbsorption(
                BreakdownType type,
                EffectType effect,
                EnergyType ssType,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemEnergyAbsorption();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        virtual double Total() const override;
        virtual double SumItems(
                const std::list<ActiveEffect> & effects,
                bool bApplyMultiplier) const override;
        CString m_title;
        EffectType m_effect;
        EnergyType m_energyType;
};
