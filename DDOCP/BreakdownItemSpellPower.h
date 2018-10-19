// BreakdownItemSpellPower.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemSpellPower :
        public BreakdownItem
{
    public:
        BreakdownItemSpellPower(
                BreakdownType type,
                EffectType effect,
                SpellPowerType sptype,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemSpellPower();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;

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
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot);
    private:
        BreakdownType SpellPowerBreakdown() const;

        CString m_title;
        EffectType m_effect;
        SpellPowerType m_spellPowerType;
};
