// BreakdownItemWeapon.h
//
// This class is a holder class for all breakdowns associated
// with this specific weapon. It can filter incoming effects
// and decide whether they apply to the weapon before passing these onto the sub
// breakdowns for this specific weapon type.
#pragma once
#include "BreakdownItemWeaponAttackBonus.h"
#include "BreakdownItemWeaponDamageBonus.h"
#include "BreakdownItemSimple.h"
#include "BreakdownItemWeaponCriticalThreatRange.h"
#include "BreakdownItemWeaponCriticalMultiplier.h"
#include "BreakdownItemWeaponOtherDamageEffects.h"

class BreakdownItemWeapon :
        public BreakdownItem
{
    public:
        enum
        {
            subItemCount = 5    // increase as more added
        };
        BreakdownItemWeapon(
                BreakdownType type,
                WeaponType weaponType,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem,
                InventorySlotType slot,
                const Dice & damageDice);
        virtual ~BreakdownItemWeapon();

        bool IsCentering() const;
        void AddForumExportData(std::stringstream & forumExport);

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
        virtual void SetCharacter(Character * charData, bool observe);

        // BreakdownObserver overrides
        virtual void UpdateFeatEffect(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
        virtual void UpdateEnhancementTrained(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;
        virtual void UpdateEnhancementRevoked(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;
        virtual void UpdateFeatTrained(Character * charData, const std::string & featName) override;
        virtual void UpdateFeatRevoked(Character * charData, const std::string & featName) override;
    private:
         void AddTreeItem(const std::string & entry, const std::string & total, BreakdownItem * pBreakdown);
         bool IsDamageType(WeaponDamageType type) const;

        CString m_title;
        WeaponType m_weaponType;
        Dice m_damageDice;

        BreakdownItemSimple m_baseDamage;                   // The X of X[2d6]
        BreakdownItemWeaponAttackBonus m_attackBonus;       // Basic +Hit
        BreakdownItemWeaponDamageBonus m_damageBonus;       // Basic +Damage
        BreakdownItemWeaponOtherDamageEffects m_otherDamageEffects; // e.g. +2d6 Light Damage
        BreakdownItemWeaponCriticalThreatRange m_criticalThreatRange;
        //BreakdownItemWeaponVorpalRange m_vorpalRange;       // typically 20, can be extended
        BreakdownItemWeaponAttackBonus m_criticalAttackBonus;
        BreakdownItemWeaponDamageBonus m_criticalDamageBonus;
        BreakdownItemWeaponOtherDamageEffects m_otherCriticalDamageEffects; // e.g. +2d6 Light Damage
        BreakdownItemWeaponCriticalMultiplier m_criticalMultiplier;
        BreakdownItemWeaponCriticalMultiplier m_criticalMultiplier19To20;
        BreakdownItemSimple m_attackSpeed;
        size_t m_centeredCount;

        friend class BreakdownItemWeaponAttackBonus;
};
