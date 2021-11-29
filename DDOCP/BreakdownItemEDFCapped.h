// BreakdownItemSimple.h
//
#pragma once
#include "BreakdownItemSimple.h"

class BreakdownItemEDFCapped :
        public BreakdownItemSimple
{
    public:
        BreakdownItemEDFCapped(
                BreakdownType type,
                EffectType effect,
                const CString & Title,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItemEDFCapped();

        // required overrides
        virtual CString Value() const override;
        virtual void CreateOtherEffects() override;
    protected:
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateFeatTrained(Character * charData, const std::string & featName) override;
        virtual void UpdateFeatRevoked(Character * charData, const std::string & featName) override;
};
