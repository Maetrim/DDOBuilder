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
    m_type(type),
    m_bHasWeapon(false),
    m_weapon(Weapon_Unknown),
    m_weaponCriticalMultiplier(0),
    m_bAddEnergies(true)
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
    AddActiveItems(m_effects, pControl);
    std::list<ActiveEffect> itemEffects = m_itemEffects;
    std::list<ActiveEffect> inactiveEffects;
    std::list<ActiveEffect> nonStackingEffects;
    RemoveInactive(&itemEffects, &inactiveEffects);
    RemoveNonStacking(&itemEffects, &nonStackingEffects);
    AddActiveItems(itemEffects, pControl);

    // finally add the active percentage items
    AddActivePercentageItems(m_otherEffects, pControl);
    AddActivePercentageItems(m_effects, pControl);
    AddActivePercentageItems(itemEffects, pControl);

    size_t inactiveStart = pControl->GetItemCount();
    // also show inactive and non stack effects if we have any so user
    // knows which duplicate effects they have in place
    AddDeactiveItems(m_otherEffects, pControl);
    AddDeactiveItems(m_effects, pControl);
    AddDeactiveItems(m_itemEffects, pControl);
    if (inactiveStart != pControl->GetItemCount())
    {
        // blank item between active and inactive
        pControl->InsertItem(inactiveStart, "");
        pControl->InsertItem(inactiveStart + 1, "Inactive Items");
    }
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
        AddActivePercentageItems(nonStackingEffects, pControl);
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
    m_effects.clear();
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
    Total();    // ensure active percentage items have numbers
    if (m_pTreeList != NULL)
    {
        m_pTreeList->SetItemText(m_hItem, 0, Title());
        m_pTreeList->SetItemText(m_hItem, 1, Value());
        if (m_pTreeList->GetSelectedItem() == m_hItem)
        {
            // force an update if the actively viewed item has changed
            m_pTreeList->SelectItem(m_pTreeList->GetSelectedItem());
        }
    }
    NotifyTotalChanged();
}

double BreakdownItem::Total() const
{
    // to sum the total, just get the contributions of all the stacking effects
    double total = 0;
    total += SumItems(m_otherEffects);
    total += SumItems(m_effects);

    std::list<ActiveEffect> itemEffects = m_itemEffects;
    std::list<ActiveEffect> inactiveEffects;
    std::list<ActiveEffect> nonStackingEffects;
    RemoveInactive(&itemEffects, &inactiveEffects);
    RemoveNonStacking(&itemEffects, &nonStackingEffects);
    total += SumItems(itemEffects);
    double baseTotal = total;

    // now apply percentage effects. Note percentage effects do not stack
    // a test on live shows a two percentage bonus's to hp adds two lots
    // of the base total (before percentages) to the total
    total += DoPercentageEffects(m_otherEffects, baseTotal);
    total += DoPercentageEffects(m_effects, baseTotal);
    // make sure we update listed items
    DoPercentageEffects(m_itemEffects, baseTotal);
    total += DoPercentageEffects(itemEffects, baseTotal);
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
        if ((*it).IsActive(m_pCharacter, m_weapon)
                && !(*it).IsPercentage())
        {
            // only list it if its non-zero
            if ((*it).TotalAmount(false) != 0)
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

void BreakdownItem::AddActivePercentageItems(
        const std::list<ActiveEffect> & effects,
        CListCtrl * pControl)
{
    // add all the breakdown items from this particular list
    std::list<ActiveEffect>::const_iterator it = effects.begin();
    while (it != effects.end())
    {
        // only add active items when it has an active stance flag and is a percentage
        if ((*it).IsActive(m_pCharacter, m_weapon)
                && (*it).IsPercentage())
        {
            // only list it if its non-zero
            if ((*it).TotalAmount(false) != 0)
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
                CString amount  = (*it).AmountAsPercent();
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
        if (!(*it).IsActive(m_pCharacter, m_weapon))
        {
            // only list it if its non-zero
            if ((*it).TotalAmount(false) != 0)
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
        if ((*it).IsActive(m_pCharacter, m_weapon))
        {
            if (!(*it).IsPercentage())
            {
                total += (*it).TotalAmount(true);
            }
        }
        ++it;
    }
    return total;
}

double BreakdownItem::DoPercentageEffects(
        const std::list<ActiveEffect> & effects,
        double total) const
{
    double amountAdded = 0;
    std::list<ActiveEffect>::const_iterator it = effects.begin();
    while (it != effects.end())
    {
        // only count the active items in the total
        if ((*it).IsActive(m_pCharacter, m_weapon))
        {
            if ((*it).IsPercentage())
            {
                // the amount is a percentage of the current total that
                // needs to be added.
                double percent = (*it).TotalAmount(false);
                double amount = (total * percent / 100.0);
                // round it to a whole number
                amount = (double)(int)amount;
                amountAdded += amount;
                (*it).SetPercentageValue(amount);   // so it can display its amount
            }
        }
        ++it;
    }
    return amountAdded;
}

void BreakdownItem::AddAbility(
        AbilityType ability)
{
    AbilityStance as;
    as.ability = ability;
    m_mainAbility.push_back(as);  // duplicates are fine
    // auto observe this ability
    BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
    pBI->AttachObserver(this);
}

void BreakdownItem::RemoveFirstAbility(
        AbilityType ability)
{
    AbilityStance as;
    as.ability = ability;
    for (size_t i = 0; i < m_mainAbility.size(); ++i)
    {
        if (m_mainAbility[i] == as)
        {
            m_mainAbility.erase(m_mainAbility.begin() + i);
            break;      // done
        }
    }
}

void BreakdownItem::AddAbility(
        AbilityType ability,
        const std::vector<std::string> & stances,
        WeaponType wt)
{
    AbilityStance as;
    as.ability = ability;
    as.stances = stances;
    as.weapon = wt;
    m_mainAbility.push_back(as);  // duplicates are fine
    // auto observe this ability
    BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
    pBI->AttachObserver(this);
}

void BreakdownItem::RemoveFirstAbility(
        AbilityType ability,
        const std::vector<std::string> & stances,
        WeaponType wt)
{
    AbilityStance as;
    as.ability = ability;
    as.stances = stances;
    as.weapon = wt;
    for (size_t i = 0; i < m_mainAbility.size(); ++i)
    {
        if (m_mainAbility[i] == as)
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
        // must be active
        bool active = true;
        for (size_t j = 0; j < m_mainAbility[i].stances.size(); ++j)
        {
            if (!m_pCharacter->IsStanceActive(
                    m_mainAbility[i].stances[j],
                    m_mainAbility[i].weapon))
            {
                active = false;
            }
        }
        if (active)
        {
            BreakdownItem * pBI = FindBreakdown(StatToBreakdown(m_mainAbility[i].ability));
            ASSERT(pBI != NULL);
            pBI->AttachObserver(this);  // need to know about changes to this stat
            int bonus = BaseStatToBonus(pBI->Total());
            if (bonus > largest)
            {
                largest = bonus;
                bestIndex = i;
            }
        }
    }
    if (largest != -999)
    {
        // we now have the best option
        ability = m_mainAbility[bestIndex].ability;
    }
    return ability;
}

void BreakdownItem::AddOtherEffect(const ActiveEffect & effect)
{
    if (effect.IsItemEffect())
    {
        AddEffect(&m_itemEffects, effect);
    }
    else
    {
        AddEffect(&m_otherEffects, effect);
    }
}

void BreakdownItem::AddFeatEffect(const ActiveEffect & effect)
{
    if (effect.IsItemEffect())
    {
        AddEffect(&m_itemEffects, effect);
    }
    else
    {
        AddEffect(&m_effects, effect);
    }
}

void BreakdownItem::AddEnhancementEffect(const ActiveEffect & effect)
{
    if (effect.IsItemEffect())
    {
        AddEffect(&m_itemEffects, effect);
    }
    else
    {
        AddEffect(&m_effects, effect);
    }
}

void BreakdownItem::AddItemEffect(const ActiveEffect & effect)
{
    AddEffect(&m_itemEffects, effect);
}

void BreakdownItem::RevokeOtherEffect(const ActiveEffect & effect)
{
    if (effect.IsItemEffect())
    {
        RevokeEffect(&m_itemEffects, effect);
    }
    else
    {
        RevokeEffect(&m_otherEffects, effect);
    }
}

void BreakdownItem::RevokeFeatEffect(const ActiveEffect & effect)
{
    if (effect.IsItemEffect())
    {
        RevokeEffect(&m_itemEffects, effect);
    }
    else
    {
        RevokeEffect(&m_effects, effect);
    }
}

void BreakdownItem::RevokeEnhancementEffect(const ActiveEffect & effect)
{
    if (effect.IsItemEffect())
    {
        RevokeEffect(&m_itemEffects, effect);
    }
    else
    {
        RevokeEffect(&m_effects, effect);
    }
}

void BreakdownItem::RevokeItemEffect(const ActiveEffect & effect)
{
    RevokeEffect(&m_itemEffects, effect);
}

void BreakdownItem::RemoveInactive(
        std::list<ActiveEffect> * effects,
        std::list<ActiveEffect> * inactiveEffects) const
{
    // add all inactive breakdown items from this particular list
    inactiveEffects->clear(); // ensure empty

    std::list<ActiveEffect>::iterator it = effects->begin();
    while (it != effects->end())
    {
        // only add inactive items when it has a stance flag
        if (!(*it).IsActive(m_pCharacter, m_weapon))
        {
            // add the item to be removed into the inactive list
            inactiveEffects->push_back((*it));
            it = effects->erase(it);      // remove from source list
        }
        else
        {
            ++it;
        }
    }
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
            if (!deleteIt && (*it).TotalAmount(false) == 0)
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
    itemChanged |= UpdateTreeItemTotals(&m_effects);
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
            if (spent != (*it).NumStacks())
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
            (*it).SetAmount(pBI->Total());
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
        const std::string & name,
        const Effect & effect,
        ActiveEffect * activeEffect)
{
    bool hasActiveEffect = true;
    BreakdownType bt = Breakdown_Unknown;

    // a divider may be in effect for some totals
    double divider = 1.0;
    if (effect.HasDivider())
    {
        divider = effect.Divider();
    }
    if (effect.Type() == Effect_DR)
    {
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                effect.Amount(),
                effect.DR(),
                1);
    }
    else if (effect.Type() == Effect_DRBypass)
    {
        // convert the first enum entry to a string. can only handle 1 DR Bypass
        // per effect object.
        std::list<std::string> enumsToString;
        std::list<DamageReductionType> drs = effect.DR();
        std::list<DamageReductionType>::const_iterator it = drs.begin();
        if (it != drs.end())
        {
            enumsToString.push_back((LPCTSTR)EnumEntryText((*it), drTypeMap));
        }
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                enumsToString,
                1);
    }
    else if (effect.HasAmountPerLevel())
    {
        ASSERT(effect.HasClass());
        size_t levels = m_pCharacter->ClassLevels(MAX_LEVEL)[effect.Class()];
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                effect.AmountPerLevel(),
                levels,
                effect.Class());        // no tree
    }
    else if (effect.HasAmountPerAP())
    {
        ASSERT(effect.HasEnhancementTree());
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                effect.AmountPerAP(),
                effect.EnhancementTree());
    }
    else if (effect.HasAmount())
    {
        // it is a feat that handles the amount as a regular amount
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                1,
                effect.Amount(),
                "");        // no tree
    }
    else if (effect.HasAmountVector())
    {
        if (effect.HasClass())
        {
            // its an amount that gets looked up from an amount vector
            // based on the specified class
            *activeEffect = ActiveEffect(
                    effect.Bonus(),
                    name,
                    effect.Class(),
                    effect.AmountVector());
            size_t count = m_pCharacter->ClassLevels(effect.Class());
            activeEffect->SetStacks(count);
        }
        else
        {
            // it is an effect that handles the amount based on count trained
            // for a feat we get notified once for each stack so it always works
            // for an enhancement we get told about all stacks on a single call, with the
            // stack count already set in the activeEffect
            size_t count = activeEffect->NumStacks(); //0 if not an enhancement with number of stacks set
            if (count < 1)
            {
                count = 1;
            }
            *activeEffect = ActiveEffect(
                    effect.Bonus(),
                    name,
                    count,
                    effect.AmountVector());
        }
    }
    else if (effect.HasAbility())
    {
        // it is an effect that handles the amount from a base ability bonus
        // attach to the item to observe it
        bt = StatToBreakdown(effect.Ability());
        BreakdownItem * pBI = FindBreakdown(bt);
        ASSERT(pBI != NULL);
        pBI->AttachObserver(this);  // need to know about changes to this stat
        double amount = (int)(BaseStatToBonus(pBI->Total()) / divider);
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                1,
                amount,
                "");        // no tree
        activeEffect->SetDivider(divider, DT_statBonus);
        activeEffect->SetBreakdownDependency(bt); // so we know which effect to update
    }
    else if (effect.HasFullAbility())
    {
        // it is a feat that handles the amount from a full ability value
        // attach to the item to observe it
        bt = StatToBreakdown(effect.FullAbility());
        BreakdownItem * pBI = FindBreakdown(bt);
        ASSERT(pBI != NULL);
        pBI->AttachObserver(this);  // need to know about changes to this stat
        double amount = (int)(pBI->Total() / divider);
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                1,
                amount,
                "");        // no tree
        activeEffect->SetDivider(divider, DT_fullAbility);
        activeEffect->SetBreakdownDependency(bt); // so we know which effect to update
    }
    else if (effect.HasDiceRoll())
    {
        // a Dice roll bonus to the breakdown. May have additional sub item effects
        // such as it is "Fire" damage
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                1,
                effect.DiceRoll(),
                "");
    }
    else if (effect.HasClass())
    {
        // its per n class levels
        size_t levels = m_pCharacter->ClassLevels(MAX_LEVEL)[effect.Class()];
        double amount = (int)(levels / divider);       // integer arithmetic
        if (amount > 0)
        {
            *activeEffect = ActiveEffect(
                    effect.Bonus(),
                    name,
                    1,
                    amount,
                    "");
        }
        else
        {
            hasActiveEffect = false;
        }
    }
    else if (effect.Skill().size() > 0)
    {
        // its per n skill ranks (round down 0.5's to 0)
        bt = SkillToBreakdown(effect.Skill().front());  // single entry
        BreakdownItem * pBI = FindBreakdown(bt);
        size_t ranks = (size_t)pBI->Total();    // throw away any half ranks
        double amount = (int)(ranks / divider);    // integer arithmetic
        // also need to know about changes to this skill
        pBI->AttachObserver(this);
        if (amount > 0)
        {
            *activeEffect = ActiveEffect(
                    effect.Bonus(),
                    name,
                    1,
                    amount,
                    "");
            activeEffect->SetDivider(divider, DT_fullAbility);
            activeEffect->SetBreakdownDependency(bt); // so we know which effect to update
        }
        else
        {
            hasActiveEffect = false;
        }
    }
    else if (effect.Immunity().size() > 0)
    {
        *activeEffect = ActiveEffect(
                effect.Bonus(),
                name,
                effect.Immunity(),
                1);
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
    if (effect.AnyOfStance().size() > 0)
    {
        for (size_t i = 0; i < effect.AnyOfStance().size(); ++i)
        {
            activeEffect->AddAnyOfStance(effect.AnyOfStance()[i]);
        }
    }
    if (effect.Energy().size() > 0 && m_bAddEnergies)
    {
        activeEffect->SetEnergy(effect.Energy().front());
    }
    if (effect.HasPercent())
    {
        activeEffect->SetIsPercentage(true);
    }
    if (effect.HasWeaponClass()
            && effect.WeaponClass() == WeaponClass_FocusGroup)
    {
        activeEffect->SetWeapon(Weapon());
    }
    if (effect.HasCriticalMultiplier())
    {
        // the critical multiplier of this weapon sets the number of stack for this
        // active effect
        activeEffect->SetStacks(m_weaponCriticalMultiplier);
    }
    if (effect.HasApplyAsItemEffect())
    {
        activeEffect->SetIsItemEffect();
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
        ActiveEffect activeEffect;
        // make sure it starts with the right number of stacks
        activeEffect.SetStacks(effect.m_tier); // num stacks may be changed in GetActiveEffect
        bool hasActiveEffect = GetActiveEffect(charData, name, effect.m_effect, &activeEffect);
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
        ActiveEffect activeEffect;
        bool hasActiveEffect = GetActiveEffect(charData, name, effect.m_effect, &activeEffect);
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
    itemChanged |= UpdateEffectAmounts(&m_effects, type);
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
    itemChanged |= UpdateEffectAmounts(&m_effects, classFrom);
    itemChanged |= UpdateEffectAmounts(&m_itemEffects, classFrom);

    itemChanged |= UpdateEffectAmounts(&m_otherEffects, classTo);
    itemChanged |= UpdateEffectAmounts(&m_effects, classTo);
    itemChanged |= UpdateEffectAmounts(&m_itemEffects, classTo);

    if (itemChanged)
    {
        // an item was updated, re-populate
        Populate();
    }
}

void BreakdownItem::SetHTreeItem(HTREEITEM hItem)
{
    m_hItem = hItem;
}

void BreakdownItem::SetWeapon(WeaponType wt, size_t weaponCriticalMultiplier)
{
    m_bHasWeapon = true;
    m_weapon = wt;
    m_weaponCriticalMultiplier = weaponCriticalMultiplier;
}

WeaponType BreakdownItem::Weapon() const
{
    return m_weapon;
}
