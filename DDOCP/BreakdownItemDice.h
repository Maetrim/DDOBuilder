// BreakdownItemSimple.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemDice :
        public BreakdownItem
{
    public:
        BreakdownItemDice(
                BreakdownType type,
                EffectType effect,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemDice();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        CString SumDice() const;
        CString m_title;
        EffectType m_effect;
};
