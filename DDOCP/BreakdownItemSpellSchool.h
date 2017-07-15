// BreakdownItemSpellSchool.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemSpellSchool :
        public BreakdownItem
{
    public:
        BreakdownItemSpellSchool(
                BreakdownType type,
                EffectType effect,
                SpellSchoolType ssType,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemSpellSchool();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        CString m_title;
        EffectType m_effect;
        SpellSchoolType m_spellSchoolType;
};
