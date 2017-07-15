// BreakdownItemTactical.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemTactical:
        public BreakdownItem
{
    public:
        BreakdownItemTactical(
                BreakdownType type,
                TacticalType tactical,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemTactical();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType type, size_t level) override;
        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;

        bool IsUs(const Effect & effect) const;
        TacticalType m_tacticalType;
};
