// BreakdownItemMDB.h
//
// A standard base class that all items that can have breakdowns calculated
// for them inherit from to allow a common interface to the CBreakdownsView
#pragma once

#include "BreakdownItem.h"

class BreakdownItemMDB :
        public BreakdownItem
{
    public:
        BreakdownItemMDB(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemMDB();

        // required overrides
        virtual CString Title() const override;
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
        virtual bool AffectsUs(const Effect & effect) const override;
    private:
        // Character overrides
        virtual void UpdateGearChanged(Character * charData, InventorySlotType slot);
        virtual void UpdateFeatTrained(Character * charData, const std::string & featName);
        virtual void UpdateFeatRevoked(Character * charData, const std::string & featName);

        bool m_bNoLimit;
};
