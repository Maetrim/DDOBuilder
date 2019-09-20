// BreakdownItemEnergyCasterLevel.h
//
// A standard base class that all items that can have breakdowns calculated
// for them inherit from to allow a common interface to the CBreakdownsView
#pragma once

#include "BreakdownItem.h"
#include "BreakdownItemSimple.h"

class BreakdownItemEnergyCasterLevel :
        public BreakdownItem
{
    public:
        BreakdownItemEnergyCasterLevel(EnergyType energyType, BreakdownType type, MfcControls::CTreeListCtrl * treeList, HTREEITEM hItem);
        virtual ~BreakdownItemEnergyCasterLevel();

        virtual void SetCharacter(Character * charData, bool observe) override;

        virtual void AppendItems(CListCtrl * pControl) override;

        int MaxTotal() const;

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;

        // BreakdownObserver overrides
        virtual void UpdateFeatEffect(Character * pCharacater, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName,  const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        EnergyType m_energy;
        BreakdownItemSimple m_maxCasterLevel;
};
