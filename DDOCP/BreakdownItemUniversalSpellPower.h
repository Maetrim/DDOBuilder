// BreakdownItemUniversalSpellPower.h
//
#pragma once
#include "BreakdownItemSimple.h"

class BreakdownItemUniversalSpellPower :
        public BreakdownItemSimple
{
    public:
        BreakdownItemUniversalSpellPower(
                BreakdownType type,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemUniversalSpellPower();

        // required overrides
        virtual void CreateOtherEffects() override;

        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
        // CharacterObserver overrides
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
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot) override;
        virtual void UpdateStanceActivated(Character * charData, const std::string & stanceName) override;
        virtual void UpdateStanceDeactivated(Character * charData, const std::string & stanceName) override;
};
