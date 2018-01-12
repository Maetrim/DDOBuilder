// BreakdownItemWeaponAttackBonus.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemWeapon;

class BreakdownItemWeaponAttackBonus :
        public BreakdownItem
{
    public:
        BreakdownItemWeaponAttackBonus(
                BreakdownType type,
                EffectType effect,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem,
                BreakdownItemWeapon * pWeapon,
                InventorySlotType slot);
        virtual ~BreakdownItemWeaponAttackBonus();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;

        // BreakdownObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateFeatEffect(Character * pCharacater, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
        virtual void UpdateFeatTrained(Character * charData, const std::string & featName) override;
        virtual void UpdateFeatRevoked(Character * charData, const std::string & featName) override;
    private:
        CString m_title;
        EffectType m_effect;
        int m_proficientCount;  // can be proficient from multiple sources
        BreakdownItemWeapon * m_pWeapon;
        InventorySlotType m_slot;
};
