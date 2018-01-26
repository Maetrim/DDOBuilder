// BreakdownItemSneakAttackDice.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemSneakAttackDice :
        public BreakdownItem
{
    public:
        BreakdownItemSneakAttackDice(
                BreakdownType type,
                EffectType effect,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemSneakAttackDice();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        CString m_title;
        EffectType m_effect;
};
