// BreakdownItemAssassinate.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemAssassinate:
        public BreakdownItem
{
    public:
        BreakdownItemAssassinate(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemAssassinate();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;

        bool IsUs(const Effect & effect) const;
};
