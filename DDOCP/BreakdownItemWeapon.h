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
                InventorySlotType slot);
        virtual ~BreakdownItemWeapon();

        bool IsCentering() const;

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
         void AddTreeItem(const std::string & entry, BreakdownItem * pBreakdown);
         bool IsCrossbow() const;
         bool IsDruidicWeapon() const;
         bool IsHeavyBladeWeapon() const;
         bool IsLightBladeWeapon() const;
         bool IsPickOrHammerWeapon() const;
         bool IsMaceOrClubWeapon() const;
         bool IsMartialArtsWeapon() const;
         bool IsAxe() const;
         bool IsLightWeapon() const;
         bool IsDamageType(WeaponDamageType type) const;

        CString m_title;
        WeaponType m_weaponType;

        BreakdownItemSimple m_baseDamage;                   // The X of X[2d6]
        BreakdownItemWeaponAttackBonus m_attackBonus;       // Basic +Hit
        BreakdownItemWeaponDamageBonus m_damageBonus;       // Basic +Damage
        //BreakdownItemWeaponVorpalRange m_vorpalRange;       // typically 20, can be extended
        //BreakdownItemWeaponCriticalRange m_criticalThreatRange; // e.g. 18-20
        BreakdownItemWeaponAttackBonus m_criticalAttackBonus;
        BreakdownItemWeaponDamageBonus m_criticalDamageBonus;
        //BreakdownItemWeaponCriticalMultiplier m_criticalMultiplier;
        //BreakdownItemWeaponSpecialMultiplier m_specialMultiplier;   // e.g. extra multiplier on a 19-20
        //BreakdownItemSimple m_attackSpeed;
        size_t m_centeredCount;

        friend class BreakdownItemWeaponAttackBonus;
};
