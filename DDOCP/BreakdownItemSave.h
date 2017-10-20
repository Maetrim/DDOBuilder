// BreakdownItemSave.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemSave:
        public BreakdownItem
{
    public:
        BreakdownItemSave(
                BreakdownType type,
                SaveType st,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem,
                AbilityType ability,
                BreakdownItem * pBaseItem);
        virtual ~BreakdownItemSave();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;

        bool HasNoFailOn1() const;
    protected:
        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateEnhancementTrained(Character * charData, const std::string & enhancementName, bool isTier5) override;
        virtual void UpdateEnhancementRevoked(Character * charData, const std::string & enhancementName, bool isTier5) override;

        // BreakdownObserver overrides
        virtual void UpdateFeatEffect(Character * pCharacater, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName,  const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        bool IsUs(const Effect & effect) const;
        SaveType m_saveType;
        AbilityType m_ability;
        BreakdownItem * m_pBaseBreakdown;
        size_t m_noFailOn1Count;
};
