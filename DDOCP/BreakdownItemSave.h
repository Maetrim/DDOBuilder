// BreakdownItemSave.h
//
#pragma once
#include "BreakdownItem.h"
#include "BreakdownItemSimple.h"

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
                BreakdownItemSave * pBaseItem);
        virtual ~BreakdownItemSave();

        virtual void SetCharacter(Character * charData, bool observe) override;

        virtual void AppendItems(CListCtrl * pControl) override;

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;

        bool HasNoFailOn1() const;
    protected:
        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateEnhancementTrained(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;
        virtual void UpdateEnhancementRevoked(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;

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
        bool IsOurSaveType(SaveType st) const;
        SaveType m_saveType;
        AbilityType m_ability;
        BreakdownItemSave * m_pBaseBreakdown;
        BreakdownItemSimple m_noFailOnOne;
};
