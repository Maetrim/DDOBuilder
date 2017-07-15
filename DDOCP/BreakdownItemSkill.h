// BreakdownItemSkill.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemSkill :
        public BreakdownItem
{
    public:
        BreakdownItemSkill(
                SkillType skill,
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem,
                BreakdownItem * pAbility);
        virtual ~BreakdownItemSkill();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        // CharacterObserver overrides
        virtual void UpdateSkillSpendChanged(Character * pCharacater, size_t level, SkillType skill) override;
        virtual void UpdateSkillTomeChanged(Character * pCharacater, SkillType skill) override;
        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        SkillType m_skill;
        BreakdownItem * m_pAbility;
};
