// BreakdownItem.cpp
//
#include "stdafx.h"
#include "BreakdownItem.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"
#include "TreeListCtrl.h"

BreakdownItem::BreakdownItem(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    m_pCharacter(NULL),
    m_pTreeList(treeList),
    m_hItem(hItem),
    m_type(type)
{
}

BreakdownItem::~BreakdownItem()
{
}

void BreakdownItem::PopulateBreakdownControl(CListCtrl * pControl)
{
    pControl->LockWindowUpdate();
    pControl->DeleteAllItems();
    // add all the items
    AddActiveItems(m_otherEffects, pControl);
    AddActiveItems(m_featEffects, pControl);
    AddActiveItems(m_enhancementEffects, pControl);
    std::list<ActiveEffect> itemEffects = m_itemEffects;
    std::list<ActiveEffect> nonStackingEffects;
    RemoveNonStacking(&itemEffects, &nonStackingEffects);
    AddActiveItems(itemEffects, pControl);
    size_t inactiveStart = pControl->GetItemCount();
    // also show inactive and non stack effects if we have any so user
    // knows which duplicate effects they have in place
    AddDeactiveItems(m_otherEffects, pControl);
    AddDeactiveItems(m_featEffects, pControl);
    AddDeactiveItems(m_enhancementEffects, pControl);
    AddDeactiveItems(m_itemEffects, pControl);
    if (nonStackingEffects.size() > 0)
    {
        // separate these non-stacking effects from the main list
        size_t index = pControl->InsertItem(
                pControl->GetItemCount(),
                "",         // blank line entry
                0);
        index = pControl->InsertItem(
                pControl->GetItemCount(),
                "Non-Stacking Effects",
                0);
        AddActiveItems(nonStackingEffects, pControl);
    }
    if (inactiveStart != pControl->GetItemCount())
    {
        // blank item between active and inactive
        pControl->InsertItem(inactiveStart, "");
        pControl->InsertItem(inactiveStart + 1, "Inactive Items");
    }
    pControl->UnlockWindowUpdate();
}

void BreakdownItem::SetCharacter(Character * pCharacter, bool observe)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }
    m_pCharacter = pCharacter;
    // if the document has changed, we will
    // need to regenerate all the items that are used to
    // calculate the total for this breakdown
    m_otherEffects.clear();
    m_featEffects.clear();
    m_enhancementEffects.clear();
    m_itemEffects.clear();
    if (m_pCharacter != NULL)
    {
        if (observe)
        {
            m_pCharacter->AttachObserver(this);
        }
        CreateOtherEffects();
    }
    // now add the standard items
    Populate();
}

BreakdownType BreakdownItem::Type() const
{
    return m_type;
}

void BreakdownItem::Populate()
{
    m_pTreeList->SetItemText(m_hItem, 0, Title());
    m_pTreeList->SetItemText(m_hItem, 1, Value());
    if (m_pTreeList->GetSelectedItem() == m_hItem)
    {
        // force an update if the actively viewed item has changed
        m_pTreeList->SelectItem(m_pTreeList->GetSelectedItem());
    }
    NotifyTotalChanged();
}

double BreakdownItem::Total() const
{
    // to sum the total, just get the contributions of all the stacking effects
    double total = 0;
    total += SumItems(m_otherEffects);
    total += SumItems(m_featEffects);
    total += SumItems(m_enhancementEffects);
    std::list<ActiveEffect> itemEffects = m_itemEffects;
    std::list<ActiveEffect> nonStackingEffects;
    RemoveNonStacking(&itemEffects, &nonStackingEffects);
    total += SumItems(itemEffects);
    return total;
}

void BreakdownItem::AddActiveItems(
        const std::list<ActiveEffect> & effects,
        CListCtrl * pControl)
{
    // add all the breakdown items from this particular list
    std::list<ActiveEffect>::const_iterator it = effects.begin();
    while (it != effects.end())
    {
        // only add active items when it has an active stance flag
        if ((*it).IsActive(m_pCharacter))
        {
            // only list it if its non-zero
            if ((*it).TotalAmount() != 0)
            {
                // put the effect name into the control
                CString effectName = (*it).Name();
                size_t index = pControl->InsertItem(
                        pControl->GetItemCount(),
                        effectName,
                        0);

                // the number of stacks of it
                CString stacks = (*it).Stacks();
                pControl->SetItemText(index, CO_Stacks, stacks);

                // and the total amount the number of stacks contribute
                CString amount  = (*it).AmountAsText();
                pControl->SetItemText(index, CO_Value, amount);

                // add the bonus type
                CString bonus = EnumEntryText((*it).Bonus(), bonusTypeMap);
                pControl->SetItemText(index, CO_BonusType, bonus);
            }
        }
        ++it;
    }
}

void BreakdownItem::AddDeactiveItems(
        const std::list<ActiveEffect> & effects,
        CListCtrl * pControl)
{
    // add all inactive breakdown items from this particular list
    std::list<ActiveEffect>::const_iterator it = effects.begin();
    while (it != effects.end())
    {
        // only add inactive items when it has a stance flag
        if (!(*it).IsActive(m_pCharacter))
        {
            // only list it if its non-zero
            if ((*it).TotalAmount() != 0)
            {
                // put the effect name into the control
                CString effectName = (*it).Name();
                size_t index = pControl->InsertItem(
                        pControl->GetItemCount(),
                        effectName,
                        0);

                // the number of stacks of it
                CString stacks = (*it).Stacks();
                pControl->SetItemText(index, CO_Stacks, stacks);

                // and the total amount the number of stacks contribute
                CString amount  = (*it).AmountAsText();
                pControl->SetItemText(index, CO_Value, amount);

                // add the bonus type
                CString bonus = EnumEntryText((*it).Bonus(), bonusTypeMap);
                pControl->SetItemText(index, CO_BonusType, bonus);
            }
        }
        ++it;
    }
}

double BreakdownItem::SumItems(const std::list<ActiveEffect> & effects) const
{
    double total = 0;
    std::list<ActiveEffect>::const_iterator it = effects.begin();
    while (it != effects.end())
    {
        // only count the active items in the total
        if ((*it).IsActive(m_pCharacter))
        {
            total += (*it).TotalAmount();
        }
        ++it;
    }
    return total;
}

void BreakdownItem::AddAbility(AbilityType ability)
{
    m_mainAbility.push_back(ability);  // duplicates are fine
    // auto observe this ability
    BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
    pBI->AttachObserver(this);
}

void BreakdownItem::RemoveFirstAbility(AbilityType ability)
{
    for (size_t i = 0; i < m_mainAbility.size(); ++i)
    {
        if (m_mainAbility[i] == ability)
        {
            m_mainAbility.erase(m_mainAbility.begin() + i);
            break;      // done
        }
    }
}

AbilityType BreakdownItem::LargestStatBonus()
{
    AbilityType ability = Ability_Unknown;

    size_t bestIndex = 0;       // assume first is best
    int largest = -999;         // arbitrarily small
    for (size_t i = 0; i < m_mainAbility.size(); ++i)
    {
        BreakdownItem * pBI = FindBreakdown(StatToBreakdown(m_mainAbility[i]));
        ASSERT(pBI != NULL);
        pBI->AttachObserver(this);  // need to know about changes to this stat
        int bonus = BaseStatToBonus(pBI->Total());
        if (bonus > largest)
        {
            largest = bonus;
            bestIndex = i;
        }
    }
    if (largest != -999)
    {
        // we now have the best option
        ability = m_mainAbility[bestIndex];
    }
    return ability;
}

void BreakdownItem::AddOtherEffect(const ActiveEffect & effect)
{
    AddEffect(&m_otherEffects, effect);
}

void BreakdownItem::AddFeatEffect(const ActiveEffect & effect)
{
    AddEffect(&m_featEffects, effect);
}

void BreakdownItem::AddEnhancementEffect(const ActiveEffect & effect)
{
    AddEffect(&m_enhancementEffects, effect);
}

void BreakdownItem::AddItemEffect(const ActiveEffect & effect)
{
    AddEffect(&m_itemEffects, effect);
}

void BreakdownItem::RevokeOtherEffect(const ActiveEffect & effect)
{
    RevokeEffect(&m_otherEffects, effect);
}

void BreakdownItem::RevokeFeatEffect(const ActiveEffect & effect)
{
    RevokeEffect(&m_featEffects, effect);
}

void BreakdownItem::RevokeEnhancementEffect(const ActiveEffect & effect)
{
    RevokeEffect(&m_enhancementEffects, effect);
}

void BreakdownItem::RevokeItemEffect(const ActiveEffect & effect)
{
    RevokeEffect(&m_itemEffects, effect);
}

void BreakdownItem::RemoveNonStacking(
        std::list<ActiveEffect> * effects,
        std::list<ActiveEffect> * nonStackingEffects) const
{
    // the same effect type can come from multiple sources in the form of
    // buffs or equipment effects. To avoid these giving increased values
    // we remove all the duplicate and lesser powered versions of any effects
    // from the list provided and push them onto a separate list
    nonStackingEffects->clear();        // ensure empty
    // look at each item in the list and see if it is a lesser or duplicate
    // of another effect
    std::list<ActiveEffect>::iterator sit = effects->begin();
    while (sit != effects->end())
    {
        bool removeIt = false;
        // now compare it to all other items in the same list
        std::list<ActiveEffect>::iterator tit = effects->begin();
        while (!removeIt && tit != effects->end())
        {
            // don't compare the item against itself
            if (sit != tit)
            {
                // not the same item, gets removed if it is a duplicate
                removeIt |= ((*sit) <= (*tit));
            }
            ++tit;
        }
        if (removeIt)
        {
            // add the item to be removed into the non stacking list
            nonStackingEffects->push_back((*sit));
            sit = effects->erase(sit);      // remove from source list
        }
        else
        {
            // not removed, this is the top level kosher item for this effect
            ++sit;
        }
    }
}

void BreakdownItem::AddEffect(
        std::list<ActiveEffect> * effectList,
        const ActiveEffect & effect)
{
    // if an identical effect is already in the list, increase its stacking
    // else add a new item
    bool found = false;
    std::list<ActiveEffect>::iterator it = effectList->begin();
    while (!found && it != effectList->end())
    {
        if (effect == (*it))
        {
            // it is an existing effect, add another stack
            found = true;
            (*it).AddStack();
        }
        ++it;
    }
    if (!found)
    {
        // it is a new effect, just add it to the list
        effectList->push_back(effect);
    }
    Populate();
}

void BreakdownItem::RevokeEffect(
        std::list<ActiveEffect> * effectList,
        const ActiveEffect & effect)
{
    // if an identical effect is already in the list, decrease its stacking
    // and remove if all stacks gone
    bool found = false;
    std::list<ActiveEffect>::iterator it = effectList->begin();
    while (!found && it != effectList->end())
    {
        if (effect == (*it))
        {
            // it an existing effect, clear a stack
            found = true;
            bool deleteIt = (*it).RevokeStack();        // true if no stacks left
            if (!deleteIt && (*it).TotalAmount() == 0)
            {
                // if it resolves to no bonus, time to delete it also
                deleteIt = true;
            }
            if (deleteIt)
            {
                it = effectList->erase(it);
            }
            break;
        }
        ++it;
    }
    Populate();
}

// DocumentObserver overrides
void BreakdownItem::UpdateStanceActivated(
        Character * charData,
        const std::string & stanceName)
{
    // just repopulate
    Populate();
}

void BreakdownItem::UpdateStanceDeactivated(
        Character * charData,
        const std::string & stanceName)
{
    // just repopulate
    Populate();
}

void BreakdownItem::UpdateFeatTrained(Character * charData, const std::string & featName)
{
    // just repopulate
    Populate();
}

void BreakdownItem::UpdateFeatRevoked(Character * charData, const std::string & featName)
{
    // just repopulate
    Populate();
}

void BreakdownItem::NotifyTotalChanged()
{
    NotifyAll(&BreakdownObserver::UpdateTotalChanged, this, m_type);
}

bool BreakdownItem::UpdateTreeItemTotals()
{
    // check all items that are dependent on AP spent in a tree
    bool itemChanged = false;
    itemChanged |= UpdateTreeItemTotals(&m_otherEffects);
    itemChanged |= UpdateTreeItemTotals(&m_featEffects);
    itemChanged |= UpdateTreeItemTotals(&m_enhancementEffects);
    itemChanged |= UpdateTreeItemTotals(&m_itemEffects);

    if (itemChanged)
    {
        Populate();
    }

    return itemChanged;
}

bool BreakdownItem::UpdateTreeItemTotals(std::list<ActiveEffect> * list)
{
    bool itemChanged = false;
    std::list<ActiveEffect>::iterator it = list->begin();
    while (it != list->end())
    {
        if ((*it).IsAmountPerAP())
        {
            size_t spent = m_pCharacter->APSpentInTree((*it).Tree());
            if (spent != (*it).Stacks())
            {
                (*it).SetStacks(spent);
                itemChanged = true;
            }
        }
        ++it;
    }
    return itemChanged;
}

bool BreakdownItem::UpdateEffectAmounts(
        std::list<ActiveEffect> * list,
        BreakdownType bt)
{
    bool itemChanged = false;
    std::list<ActiveEffect>::iterator it = list->begin();
    while (it != list->end())
    {
        if ((*it).HasBreakdownDependency(bt))
        {
            BreakdownItem * pBI = FindBreakdown(bt);
            ASSERT(pBI != NULL);
            (*it).SetAmount(BaseStatToBonus(pBI->Total()));
            itemChanged = true;
        }
        ++it;
    }
    return itemChanged;
}

bool BreakdownItem::UpdateEffectAmounts(std::list<ActiveEffect> * list, ClassType type)
{
    bool itemChanged = false;
    std::list<ActiveEffect>::iterator it = list->begin();
    while (it != list->end())
    {
        if ((*it).HasClass(type))
        {
            std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
            (*it).SetStacks(classLevels[type]);
            itemChanged = true;
        }
        ++it;
    }
    return itemChanged;
}

bool BreakdownItem::GetActiveEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect,
        ActiveEffect * activeEffect)
{
    bool hasActiveEffect = true;
    BreakdownType bt = Breakdown_Unknown;

    if (effect.HasAmountPerLevel())
    {
        ASSERT(effect.HasClass());
        size_t levels = m_pCharacter->ClassLevels(MAX_LEVEL)[effect.Class()];
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                featName,
                effect.AmountPerLevel(),
                levels,
                effect.Class());        // no tree
    }
    else if (effect.HasAmount())
    {
        // it is a feat that handles the amount as a regular amount
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                featName,
                1,
                effect.Amount(),
                "");        // no tree
    }
    else if (effect.HasAmountVector())
    {
        // it is a feat that handles the amount based on count trained
        // amount can be 0 at a given tier, breakdown does not display if that
        // happens
        size_t count = pCharacter->GetSpecialFeatTrainedCount(featName);
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                featName,
                1,
                effect.AmountVector());
    }
    else if (effect.HasAbility())   // some feat effects can have amount and ability, amount always takes precedence
    {
        // it is a feat that handles the amount from a base ability bonus
        // attach to the item to observe it
        bt = StatToBreakdown(effect.Ability());
        BreakdownItem * pBI = FindBreakdown(bt);
        ASSERT(pBI != NULL);
        pBI->AttachObserver(this);  // need to know about changes to this stat
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                featName,
                1,
                BaseStatToBonus(pBI->Total()),
                "");        // no tree
    }
    else if (effect.HasDiceRoll())
    {
        // a Dice roll bonus to the breakdown. May have additional sub item effects
        // such as it is "Fire" damage
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                featName,
                1,
                effect.DiceRoll(),
                "");
    }
    else if (effect.HasDivider())
    {
        // its an amount per n skill/class
        int amount = 0;
        if (effect.HasClass())
        {
            // its per n class levels
            size_t levels = m_pCharacter->ClassLevels(MAX_LEVEL)[effect.Class()];
            amount = (levels / effect.Divider());       // integer arithmetic
        }
        if (effect.HasSkill())
        {
            // its per n skill ranks (round down 0.5's to 0)
            bt = SkillToBreakdown(effect.Skill());
            BreakdownItem * pBI = FindBreakdown(bt);
            size_t ranks = (size_t)pBI->Total();    // throw away any half ranks
            amount = (ranks / effect.Divider());    // integer arithmetic
            // also need to know about changes to this skill
            pBI->AttachObserver(this);
        }
        if (amount > 0)
        {
            *activeEffect = ActiveEffect(
                    effect.Bonus(),
                    featName,
                    1,
                    amount,
                    "");
        }
        else
        {
            hasActiveEffect = false;
        }
    }
    if (effect.HasFeat())
    {
        activeEffect->AddFeat(effect.Feat());
    }

    if (effect.Stance().size() > 0)
    {
        for (size_t i = 0; i < effect.Stance().size(); ++i)
        {
            activeEffect->AddStance(effect.Stance()[i]);
        }
    }
    if (effect.HasEnergy())
    {
        activeEffect->SetEnergy(effect.Energy());
    }
    if (bt != Breakdown_Unknown)
    {
        activeEffect->SetBreakdownDependency(bt); // so we know which effect to update
    }
    return hasActiveEffect;
}

void BreakdownItem::UpdateFeatEffect(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // see if this feat effect applies to us, if so add it
    if (AffectsUs(effect))
    {
        std::string name(featName);
        if (effect.HasDisplayName())
        {
            name = effect.DisplayName();
        }
        // yup, it applies to us, add it in
        ActiveEffect feat;
        if (GetActiveEffect(pCharacter, name, effect, &feat))
        {
            // may not get an affect if the amount applied is 0
            AddFeatEffect(feat);
        }
    }
}

void BreakdownItem::UpdateFeatEffectRevoked(
        Character * pCharacter,
        const std::string & featName,
        const Effect & effect)
{
    // see if this feat effect applies to us, if so revoke it
    if (AffectsUs(effect))
    {
        std::string name(featName);
        if (effect.HasDisplayName())
        {
            name = effect.DisplayName();
        }
        // yup, it applies to us, revoke it
        ActiveEffect feat;
        if (GetActiveEffect(pCharacter, name, effect, &feat))
        {
            // may not get an affect if the amount applied is 0
            RevokeFeatEffect(feat);
        }
    }
}

void BreakdownItem::UpdateItemEffect(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // see if this item effect applies to us, if so add it
    if (AffectsUs(effect))
    {
        std::string name(itemName);
        if (effect.HasDisplayName())
        {
            name = effect.DisplayName();
        }
        // yup, it applies to us, add it in
        ActiveEffect item;
        if (GetActiveEffect(pCharacter, name, effect, &item))
        {
            // may not get an affect if the amount applied is 0
            AddItemEffect(item);
        }
    }
}

void BreakdownItem::UpdateItemEffectRevoked(
        Character * pCharacter,
        const std::string & itemName,
        const Effect & effect)
{
    // see if this item effect applies to us, if so revoke it
    if (AffectsUs(effect))
    {
        std::string name(itemName);
        if (effect.HasDisplayName())
        {
            name = effect.DisplayName();
        }
        // yup, it applies to us, revoke it
        ActiveEffect item;
        if (GetActiveEffect(pCharacter, name, effect, &item))
        {
            // may not get an affect if the amount applied is 0
            RevokeItemEffect(item);
        }
    }
}

void BreakdownItem::UpdateEnhancementEffect(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // see if this enhancement effect applies to us, if so add it
    if (AffectsUs(effect.m_effect))
    {
        std::string name(enhancementName);
        if (effect.m_effect.HasDisplayName())
        {
            name = effect.m_effect.DisplayName();
        }
        bool hasActiveEffect = true;
        ActiveEffect activeEffect;

        // could be an enhancement based on AP spend in a tree
        if (effect.m_effect.HasAmountVector())
        {
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_tier,
                    effect.m_effect.AmountVector());
        }
        else if (effect.m_effect.HasAmount())
        {
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_tier,
                    effect.m_effect.Amount(),
                    "");
        }
        else if (effect.m_effect.HasAmountPerLevel())
        {
            ASSERT(effect.m_effect.HasClass());
            std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_effect.AmountPerLevel(),
                    classLevels[effect.m_effect.Class()],
                    effect.m_effect.Class());
        }
        else
        {
            // it is an AP spend in tree version
            ASSERT(effect.m_effect.HasEnhancementTree());
            ASSERT(effect.m_effect.HasAmountPerAP());
            size_t spentInTree = charData->APSpentInTree(effect.m_effect.EnhancementTree());
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_effect.AmountPerAP(),
                    effect.m_effect.EnhancementTree());
            activeEffect.SetStacks(spentInTree);
        }
        if (effect.m_effect.HasFeat())
        {
            activeEffect.AddFeat(effect.m_effect.Feat());
        }
        if (effect.m_effect.Stance().size() > 0)
        {
            for (size_t i = 0; i < effect.m_effect.Stance().size(); ++i)
            {
                activeEffect.AddStance(effect.m_effect.Stance()[i]);
            }
        }
        if (hasActiveEffect)
        {
            AddEnhancementEffect(activeEffect);
        }
    }
}

void BreakdownItem::UpdateEnhancementEffectRevoked(
        Character * charData,
        const std::string & enhancementName,
        const EffectTier & effect)
{
    // see if this feat effect applies to us, if so revoke it
    if (AffectsUs(effect.m_effect))
    {
        std::string name(enhancementName);
        if (effect.m_effect.HasDisplayName())
        {
            name = effect.m_effect.DisplayName();
        }
        bool hasActiveEffect = true;
        ActiveEffect activeEffect;

        // could be an enhancement based on spend in a tree
        if (effect.m_effect.HasAmountVector())
        {
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_tier,
                    effect.m_effect.AmountVector());
        }
        else if (effect.m_effect.HasAmount())
        {
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_tier,
                    effect.m_effect.Amount(),
                    "");
        }
        else if (effect.m_effect.HasAmountPerLevel())
        {
            ASSERT(effect.m_effect.HasClass());
            std::vector<size_t> classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_effect.AmountPerLevel(),
                    classLevels[effect.m_effect.Class()],
                    effect.m_effect.Class());
        }
        else
        {
            // it is an AP spend in tree version
            ASSERT(effect.m_effect.HasEnhancementTree());
            ASSERT(effect.m_effect.HasAmountPerAP());
            size_t spentInTree = charData->APSpentInTree(effect.m_effect.EnhancementTree());
            activeEffect = ActiveEffect(
                    effect.m_effect.Bonus(),
                    name,
                    effect.m_effect.AmountPerAP(),
                    effect.m_effect.EnhancementTree());
            activeEffect.SetStacks(spentInTree);
        }
        if (effect.m_effect.HasFeat())
        {
            activeEffect.AddFeat(effect.m_effect.Feat());
        }

        if (effect.m_effect.Stance().size() > 0)
        {
            for (size_t i = 0; i < effect.m_effect.Stance().size(); ++i)
            {
                activeEffect.AddStance(effect.m_effect.Stance()[i]);
            }
        }
        if (hasActiveEffect)
        {
            RevokeEnhancementEffect(activeEffect);
        }
    }
}

void BreakdownItem::UpdateAPSpentInTreeChanged(
        Character * charData,
        const std::string & treeName)
{
    UpdateTreeItemTotals();
}

// BreakdownObserver overrides (may be specialised in inheriting classes)
void BreakdownItem::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    // see if the breakdown item applies to any of our effects
    // if it does, update it and repopulate
    // check all items that are dependent on AP spent in a tree
    bool itemChanged = false;
    itemChanged |= UpdateEffectAmounts(&m_otherEffects, type);
    itemChanged |= UpdateEffectAmounts(&m_featEffects, type);
    itemChanged |= UpdateEffectAmounts(&m_enhancementEffects, type);
    itemChanged |= UpdateEffectAmounts(&m_itemEffects, type);

    if (itemChanged)
    {
        // an item was updated, re-populate
        Populate();
    }
}

void BreakdownItem::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    bool itemChanged = false;
    itemChanged |= UpdateEffectAmounts(&m_otherEffects, classFrom);
    itemChanged |= UpdateEffectAmounts(&m_featEffects, classFrom);
    itemChanged |= UpdateEffectAmounts(&m_enhancementEffects, classFrom);
    itemChanged |= UpdateEffectAmounts(&m_itemEffects, classFrom);
    itemChanged |= UpdateEffectAmounts(&m_otherEffects, classTo);
    itemChanged |= UpdateEffectAmounts(&m_featEffects, classTo);
    itemChanged |= UpdateEffectAmounts(&m_enhancementEffects, classTo);
    itemChanged |= UpdateEffectAmounts(&m_itemEffects, classTo);

    if (itemChanged)
    {
        // an item was updated, re-populate
        Populate();
    }
}
