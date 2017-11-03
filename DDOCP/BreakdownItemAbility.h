// BreakdownItemAbility.h
//
// A standard base class that all items that can have breakdowns calculated
// for them inherit from to allow a common interface to the CBreakdownsView
#pragma once

#include "BreakdownItem.h"

class BreakdownItemAbility :
        public BreakdownItem
{
    public:
        BreakdownItemAbility(
                AbilityType skill,
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemAbility();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        // CharacterObserver overrides
        virtual void UpdateAbilityValueChanged(
                Character * pCharacater,
                AbilityType ability) override;
        virtual void UpdateAbilityTomeChanged(
                Character * pCharacater,
                AbilityType ability) override;
        virtual void UpdateRaceChanged(
                Character * charData,
                RaceType race) override;
        virtual void UpdateEnhancementTrained(
                Character * charData,
                const std::string & enhancementName,
                const std::string & selection,
                bool isTier5) override;
        virtual void UpdateEnhancementRevoked(
                Character * charData,
                const std::string & enhancementName,
                const std::string & selection,
                bool isTier5) override;
    private:
        AbilityType m_ability;
};
