// BreakdownItemWeaponEffects.h
//
// This class is a holder class for all effects for each given
// weapon type. This allows weapon items to be moved in and out of
// gear slots and being able to re-generate all the effects for the
// given weapon type without having to notify all effects from all
// sources.
#pragma once
#include "BreakdownItem.h"
#include "WeaponTypes.h"
// this item is never visible in the UI

class BreakdownItemWeaponEffects :
        public BreakdownItem
{
    public:
        BreakdownItemWeaponEffects(
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemWeaponEffects();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
        virtual void SetCharacter(Character * charData, bool observe);

        void WeaponsChanged(const EquippedGear & gear);
        bool AreWeaponsCentering() const;
        void AddForumExportData(std::stringstream & forumExport);

        BreakdownItem * GetWeaponBreakdown(bool bMainhand, BreakdownType bt);

    protected:
        // BreakdownObserver overrides
        virtual void UpdateFeatEffect(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
    private:
        void AddToAffectedWeapons(std::vector<std::list<Effect> > * list, const Effect & effect);
        void RemoveFromAffectedWeapons(std::vector<std::list<Effect> > * list, const Effect & effect);
        void AddToAffectedWeapons(std::vector<std::list<EffectTier> > * list, const EffectTier & effect);
        void RemoveFromAffectedWeapons(std::vector<std::list<EffectTier> > * list, const EffectTier & effect);
        bool AffectsThisWeapon(WeaponType wt, const Effect & effect);
        bool IsMartialWeapon(WeaponType wt) const;
        bool IsSimpleWeapon(WeaponType wt) const;
        bool IsThrownWeapon(WeaponType wt) const;
        bool IsOneHandedWeapon(WeaponType wt) const;
        bool IsRangedWeapon(WeaponType wt) const;
        bool IsTwoHandedWeapon(WeaponType wt) const;
        bool IsRepeatingCrossbow(WeaponType wt) const;
        bool IsMeleeWeapon(WeaponType wt) const;
        bool IsLightWeapon(WeaponType wt) const;
        bool IsDamageType(WeaponType wt, WeaponDamageType type) const;
        BreakdownItemWeapon * CreateWeaponBreakdown(
                BreakdownType bt,
                const Item & item);

        std::vector<std::list<Effect> > m_weaponFeatEffects;
        std::vector<std::list<Effect> > m_weaponItemEffects;
        std::vector<std::list<EffectTier> > m_weaponEnhancementEffects;
        MfcControls::CTreeListCtrl * m_pTreeList;
        HTREEITEM m_hItem;
        BreakdownItemWeapon * m_pMainHandWeapon;
        BreakdownItemWeapon * m_pOffHandWeapon;
};
