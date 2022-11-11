// BreakdownItemBAB.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemBAB :
        public BreakdownItem
{
    public:
        BreakdownItemBAB(MfcControls::CTreeListCtrl * treeList, HTREEITEM hItem);
        virtual ~BreakdownItemBAB();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
};
