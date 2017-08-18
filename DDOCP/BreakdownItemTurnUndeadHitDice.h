// BreakdownItemTurnUndeadHitDice.h
//
#pragma once
#include "BreakdownItem.h"

class BreakdownItemTurnUndeadHitDice:
        public BreakdownItem
{
    public:
        BreakdownItemTurnUndeadHitDice(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemTurnUndeadHitDice();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateSpellTrained(Character * charData, const TrainedSpell & spell) override;
        virtual void UpdateSpellRevoked(Character * charData, const TrainedSpell & spell) override;

        // BreakdownObserver overrides
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;

        bool IsUs(const Effect & effect) const;
};
