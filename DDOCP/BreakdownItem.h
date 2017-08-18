// BreakdownItem.h
//
// A standard base class that all items that can have breakdowns calculated
// for them inherit from to allow a common interface to the CBreakdownsView
#pragma once

#include "ActiveEffect.h"
#include "BreakdownTypes.h"
#include "Character.h"
#include "Effect.h"
#include "ObserverSubject.h"

namespace MfcControls
{
    class CTreeListCtrl;
};

enum ColumnOrder
{
    CO_Source = 0,
    CO_Stacks,
    CO_Value,
    CO_BonusType,

    CO_count
};

class CBreakdownsView;
class BreakdownItem;

class BreakdownObserver :
    public Observer<BreakdownItem>
{
    public:
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) {};
};

// this is the base class for all items that can have a break down done for them
// it provides the common interface that allows information from feats, enhancements
// and items to filter in and be collated.
class BreakdownItem :
    public CharacterObserver,
    public Subject<BreakdownObserver>,
    public BreakdownObserver
{
    public:
        BreakdownItem(
                BreakdownType type,
                MfcControls::CTreeListCtrl * treeList,
                HTREEITEM hItem);
        virtual ~BreakdownItem();

        static void SetBreakdownViewPointer(const CBreakdownsView *);

        virtual void SetCharacter(Character * charData, bool observe);

        BreakdownType Type() const;
        virtual CString Title() const = 0;
        virtual CString Value() const = 0;
        void Populate();            // updates the HTREEITEM
        void PopulateBreakdownControl(CListCtrl * pControl);
        double Total() const;

        void AddAbility(AbilityType ability);
        void RemoveFirstAbility(AbilityType ability);

        // support functions for breakdowns only
        static int BaseStatToBonus(double ability);
        static BreakdownType StatToBreakdown(AbilityType ability);
        static BreakdownType SkillToBreakdown(SkillType skill);

    protected:
        void AddOtherEffect(const ActiveEffect & effect);
        void AddFeatEffect(const ActiveEffect & effect);
        void AddEnhancementEffect(const ActiveEffect & effect);
        void AddItemEffect(const ActiveEffect & effect);

        void RevokeOtherEffect(const ActiveEffect & effect);
        void RevokeFeatEffect(const ActiveEffect & effect);
        void RevokeEnhancementEffect(const ActiveEffect & effect);
        void RevokeItemEffect(const ActiveEffect & effect);

        bool UpdateTreeItemTotals();
        bool UpdateTreeItemTotals(std::list<ActiveEffect> * list);
        bool UpdateEffectAmounts(std::list<ActiveEffect> * list, BreakdownType bt);
        bool UpdateEffectAmounts(std::list<ActiveEffect> * list, ClassType type);

        virtual void CreateOtherEffects() = 0;
        virtual bool AffectsUs(const Effect & effect) const = 0;

        Character * m_pCharacter;
        std::list<ActiveEffect> m_otherEffects;       // these always stack
        std::list<ActiveEffect> m_featEffects;        // these always stack
        std::list<ActiveEffect> m_enhancementEffects; // these always stack
        std::list<ActiveEffect> m_itemEffects;        // highest of a given type counts

        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
        virtual void UpdateStanceActivated(Character * charData, const std::string & stanceName) override;
        virtual void UpdateStanceDeactivated(Character * charData, const std::string & stanceName) override;
        virtual void UpdateFeatTrained(Character * charData, const std::string & featName) override;
        virtual void UpdateFeatRevoked(Character * charData, const std::string & featName) override;
        virtual void UpdateFeatEffect(Character * pCharacater, const std::string & featName,  const Effect & effect) override;
        virtual void UpdateFeatEffectRevoked(Character * pCharacater, const std::string & featName, const Effect & effect) override;
        virtual void UpdateEnhancementEffect(Character * charData, const std::string & enhancementName,  const EffectTier & effect) override;
        virtual void UpdateEnhancementEffectRevoked(Character * charData, const std::string & enhancementName, const EffectTier & effect) override;
        virtual void UpdateItemEffect(Character * charData, const std::string & itemName,  const Effect & effect) override;
        virtual void UpdateItemEffectRevoked(Character * charData, const std::string & itemName, const Effect & effect) override;
        virtual void UpdateAPSpentInTreeChanged(Character * charData, const std::string & treeName) override;

        // BreakdownObserver overrides (may be specialised in inheriting classes)
        virtual void UpdateTotalChanged(BreakdownItem * item, BreakdownType type) override;

        void NotifyTotalChanged();
    private:
        void AddActiveItems(const std::list<ActiveEffect> & effects, CListCtrl * pControl);
        void AddDeactiveItems(const std::list<ActiveEffect> & effects, CListCtrl * pControl);
        double SumItems(const std::list<ActiveEffect> & effects) const;
        void RemoveNonStacking(std::list<ActiveEffect> * effects, std::list<ActiveEffect> * nonStackingEffects) const;
        void AddEffect(std::list<ActiveEffect> * effectList, const ActiveEffect & effect);
        void RevokeEffect(std::list<ActiveEffect> * effectList, const ActiveEffect & effect);
        bool GetActiveEffect(Character * pCharacter, const std::string & featName, const Effect & effect, ActiveEffect * activeEffect);
        MfcControls::CTreeListCtrl * m_pTreeList;
        HTREEITEM m_hItem;
        BreakdownType m_type;
    protected:
        static const CBreakdownsView * m_pBreakdownView;   // used to allow breakdowns to lookup other breakdowns
        AbilityType LargestStatBonus();
        std::vector<AbilityType> m_mainAbility; // ability types that are used for this breakdown (highest of those listed)

        friend class BreakdownItemWeapon;
};
