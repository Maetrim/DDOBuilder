// BreakdownItemSpellPoints.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemSpellPoints :
        public BreakdownItem
{
    public:
        BreakdownItemSpellPoints(
                BreakdownType type,
                EffectType effect,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemSpellPoints();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
        virtual double Multiplier() const override;

        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        // BreakdownObserver overrides (may be specialised in inheriting classes)
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        CString m_title;
        EffectType m_effect;
};
