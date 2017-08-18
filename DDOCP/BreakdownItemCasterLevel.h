// BreakdownItemCasterLevel.h
//
// A standard base class that all items that can have breakdowns calculated
// for them inherit from to allow a common interface to the CBreakdownsView
#pragma once

#include "BreakdownItem.h"

class BreakdownItemCasterLevel :
        public BreakdownItem
{
    public:
        BreakdownItemCasterLevel(ClassType classType, BreakdownType type, MfcControls::CTreeListCtrl * treeList, HTREEITEM hItem);
        virtual ~BreakdownItemCasterLevel();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
    private:
        ClassType m_class;
};
