// BreakdownItemHitpoints.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemHitpoints :
        public BreakdownItem
{
    public:
        BreakdownItemHitpoints(MfcControls::CTreeListCtrl * treeList, HTREEITEM hItem, BreakdownItem * pCon);
        virtual ~BreakdownItemHitpoints();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    protected:
        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;

        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;
    private:
        BreakdownItem * m_pConstitutionBreakdown;
};
