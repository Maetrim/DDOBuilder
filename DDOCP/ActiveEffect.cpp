// ActiveEffect.cpp
//
#include "stdafx.h"
#include "ActiveEffect.h"

#include "Character.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"
#include "StancesView.h"

//============================================================================
// ActiveEffect
ActiveEffect::ActiveEffect() :
    m_bonusType(Bonus_Unknown),
    m_type(ET_unknown),
    m_effectName("Not setup"),
    m_numStacks(0),
    m_amount(0),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(""),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        size_t stacks,
        double amount,
        const std::string & tree) :
    m_bonusType(bonusType),
    m_type(ET_amount),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(amount),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        double amount,
        const std::string & tree) :
    m_bonusType(bonusType),
    m_type(ET_amountPerAp),
    m_effectName(name),
    m_numStacks(0),
    m_amount(amount),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
    // stacks is set immediately after this is constructed
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        size_t stacks,
        const Dice & dice,
        const std::string & tree) :
    m_bonusType(bonusType),
    m_type(ET_dice),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),            // not used
    m_dice(dice),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        size_t stacks,
        const std::vector<double> & amounts) :
    m_bonusType(bonusType),
    m_type(ET_amountVector),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),                // not used
    m_amounts(amounts),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        ClassType classType,
        const std::vector<double> & amounts) :
    m_bonusType(bonusType),
    m_type(ET_amountVectorPerClassLevel),
    m_effectName(name),
    m_numStacks(1),
    m_amount(0),                // not used
    m_amounts(amounts),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(classType),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        double amountPerLevel,
        size_t stacks,
        ClassType classType) :
    m_bonusType(bonusType),
    m_type(ET_amountPerLevel),
    m_effectName(name),
    m_numStacks(1),
    m_amount(0),                // not used
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(amountPerLevel),
    m_levelStacks(stacks),
    m_class(classType),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        const std::list<std::string> & immunities,
        size_t stacks) :
    m_bonusType(bonusType),
    m_immunities(immunities),
    m_type(ET_immunity),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),                // not used
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffect::ActiveEffect(
        BonusType bonusType,
        const std::string & name,
        double amount,
        const std::list<DamageReductionType> & drTypes,
        size_t stacks) :
    m_bonusType(bonusType),
    m_drTypes(drTypes),
    m_type(ET_DR),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(amount),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_levelStacks(0),
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_clearValue(false),
    m_bIsItemEffect(false),
    m_divider(1.0),
    m_dividerType(DT_none),
    m_bHasStacksControl(false),
    m_pCharacter(NULL)
{
}

ActiveEffectType ActiveEffect::Type() const
{
    return m_type;
}

BonusType ActiveEffect::Bonus() const
{
    return m_bonusType;
}

bool ActiveEffect::IsAmountPerAP() const
{
    return (m_type == ET_amountPerAp);
}

CString ActiveEffect::Name() const
{
    return m_effectName.c_str();
}

CString ActiveEffect::Stacks() const
{
    CString text;
    text.Format("%d", NumStacks());
    return text;
}

CString ActiveEffect::AmountAsText(double multiplier) const
{
    CString text;
    switch (m_type)
    {
    case ET_dice:
        text.Format("%dD%d",
                (int)m_dice.Number(NumStacks()-1),
                (int)m_dice.Sides(NumStacks()-1));
        if (m_dice.HasBonus())
        {
            CString bonus;
            bonus.Format("+%d", (int)m_dice.Bonus(NumStacks()-1));
            text += bonus;
        }
        if (m_dice.HasScalesWithMeleePower()
                || m_dice.HasScalesWithRangedPower()
                || m_dice.HasScalesWithSpellPower())
        {
            CString withScaling;
            if (m_dice.HasScalesWithSpellPower())
            {
                withScaling.Format("(%s) * %s Spell Power",
                        text,
                        EnumEntryText(m_dice.SpellPower(), spellPowerTypeMap));
            }
            else
            {
                if (m_dice.HasScalesWithMeleePower() && m_dice.HasScalesWithRangedPower())
                {
                    withScaling.Format("(%s) * MAX(Melee, Ranged) Power",
                            text);
                }
                else if (m_dice.HasScalesWithMeleePower())
                {
                    withScaling.Format("(%s) * Melee Power",
                            text);
                }
                else
                {
                    withScaling.Format("(%s) * Ranged Power",
                            text);
                }
            }
            text = withScaling;
        }
        break;
    case ET_amount:
        if (m_dividerType != DT_statBonus)
        {
            if (multiplier != 1.0)
            {
                text.Format("%.2f (* %.2f)", m_amount * NumStacks() * multiplier, multiplier);
            }
            else
            {
                text.Format("%.2f", m_amount * NumStacks() * multiplier);
            }
        }
        else
        {
            // stacks don't multiply a stat bonus
            if (multiplier != 1.0)
            {
                text.Format("%.2f (* %.2f)", m_amount * multiplier, multiplier);
            }
            else
            {
                text.Format("%.2f", m_amount * multiplier);
            }
        }
        break;
    case ET_amountVector:
        {
            int index = NumStacks()-1;
            if (index >= 0)
            {
                if (index >= (int)m_amounts.size())
                {
                    index = m_amounts.size()-1;
                    ::OutputDebugString("ActiveEffect ");
                    ::OutputDebugString((LPCTSTR)Name());
                    ::OutputDebugString(" has more stacks than amount vector\n");
                }
                if (multiplier != 1.0)
                {
                    text.Format("%.2f (* %.2f)", m_amounts[index] * multiplier, multiplier);
                }
                else
                {
                    text.Format("%.2f", m_amounts[index] * multiplier);
                }
            }
            else
            {
                text = "0.00";
            }
        }
        break;
    case ET_amountVectorPerClassLevel:
        {
            int index = m_levelStacks-1;
            if (index >= 0)
            {
                if (index >= (int)m_amounts.size())
                {
                    index = m_amounts.size()-1;
                    ::OutputDebugString("ActiveEffect ");
                    ::OutputDebugString((LPCTSTR)Name());
                    ::OutputDebugString(" has more stacks than amount vector\n");
                }
                if (multiplier != 1.0)
                {
                    text.Format("%.2f (* %.2f)", m_amounts[index] * multiplier, multiplier);
                }
                else
                {
                    text.Format("%.2f", m_amounts[index] * multiplier);
                }
            }
            else
            {
                text = "0.00";
            }
        }
        break;
    case ET_amountPerLevel:
        if (multiplier != 1.0)
        {
            text.Format("%.2f (* %.2f)", m_amountPerLevel * m_levelStacks * multiplier, multiplier);
        }
        else
        {
            text.Format("%.2f", m_amountPerLevel * m_levelStacks * multiplier);
        }
        break;
    case ET_amountPerAp:
        if (multiplier != 1.0)
        {
            text.Format("%.2f (* %.2f)", m_amount * NumStacks() * multiplier, multiplier);
        }
        else
        {
            text.Format("%.2f", m_amount * NumStacks() * multiplier);
        }
        break;
    case ET_immunity:
    case ET_DR:
        text = Description().c_str();
        break;
    default:
        text = "???";
        break;
    }
    text.Replace(".00", "");
    if (m_bHasEnergy)
    {
        text += " ";
        text += EnumEntryText(m_energy, energyTypeMap);
    }
    return text;
}

CString ActiveEffect::AmountAsPercent() const
{
    CString text;
    switch (m_type)
    {
    case ET_dice:
        text.Format("%dD%d",
                (int)m_dice.Number(NumStacks()-1),
                (int)m_dice.Sides(NumStacks()-1));
        if (m_dice.HasBonus())
        {
            CString bonus;
            bonus.Format("+%d", (int)m_dice.Bonus(NumStacks()-1));
            text += bonus;
        }
        if (m_dice.HasScalesWithMeleePower()
                || m_dice.HasScalesWithRangedPower()
                || m_dice.HasScalesWithSpellPower())
        {
            CString withScaling;
            if (m_dice.HasScalesWithSpellPower())
            {
                withScaling.Format("(%s) * %s Spell Power",
                        text,
                        EnumEntryText(m_dice.SpellPower(), spellPowerTypeMap));
            }
            else
            {
                if (m_dice.HasScalesWithMeleePower() && m_dice.HasScalesWithRangedPower())
                {
                    withScaling.Format("(%s) * MAX(Melee, Ranged) Power",
                            text);
                }
                else if (m_dice.HasScalesWithMeleePower())
                {
                    withScaling.Format("(%s) * Melee Power",
                            text);
                }
                else
                {
                    withScaling.Format("(%s) * Ranged Power",
                            text);
                }
            }
            text = withScaling;
        }
        break;
    case ET_amount:
        text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amount * NumStacks());
        break;
    case ET_amountVector:
        {
            int index = NumStacks()-1;
            if (index >= 0)
            {
                if (index >= (int)m_amounts.size())
                {
                    index = m_amounts.size()-1;
                    ::OutputDebugString("ActiveEffect ");
                    ::OutputDebugString((LPCTSTR)Name());
                    ::OutputDebugString(" has more stacks than amount vector\n");
                }
                text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amounts[index]);
            }
            else
            {
                text.Format("%.0f (0%%)", m_percentageAmount);
            }
        }
        break;
    case ET_amountVectorPerClassLevel:
        {
            int index = m_levelStacks-1;
            if (index >= 0)
            {
                if (index >= (int)m_amounts.size())
                {
                    index = m_amounts.size()-1;
                    ::OutputDebugString("ActiveEffect ");
                    ::OutputDebugString((LPCTSTR)Name());
                    ::OutputDebugString(" has more stacks than amount vector\n");
                }
                text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amounts[index]);
            }
            else
            {
                text.Format("%.0f (0%%)", m_percentageAmount);
            }
        }
        break;
    case ET_amountPerLevel:
        text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amountPerLevel * m_levelStacks);
        break;
    case ET_amountPerAp:
        text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amount * NumStacks());
        break;
    case ET_immunity:
    case ET_DR:
        text = Description().c_str();
        break;
    default:
        text = "???";
        break;
    }
    // optional effects such as its Fire damage
    text.Replace(".00", "");
    if (m_bHasEnergy)
    {
        text += " ";
        text += EnumEntryText(m_energy, energyTypeMap);
    }
    return text;
}

double ActiveEffect::Amount() const
{
    if (IsPercentage())
    {
        return m_percentageAmount;
    }
    else
    {
        return m_amount;
    }
}

void ActiveEffect::SetAmount(double amount)
{
    switch (m_dividerType)
    {
    case DT_none:
        // no change
        break;
    case DT_statBonus:
        amount = (int)(BaseStatToBonus(amount) / m_divider + 0.005);
        break;
    case DT_fullAbility:
        amount = (int)(amount / m_divider + 0.005);
        break;
    }
    m_amount = amount;
}

void ActiveEffect::SetEnergy(EnergyType type)
{
    m_bHasEnergy = (type != Energy_Unknown);
    m_energy = type;
    if (m_type == ET_dice)
    {
        m_dice.Set_Energy(m_energy);
    }
}

void ActiveEffect::SetBreakdownDependency(BreakdownType bt)
{
    m_bt = bt;
}

bool ActiveEffect::HasBreakdownDependency(BreakdownType bt) const
{
    return (bt == m_bt);
}

bool ActiveEffect::BasedOnClassLevel(ClassType type) const
{
    if (m_type == ET_amountPerLevel
            || m_type == ET_amountVectorPerClassLevel)
    {
        return (type == m_class);
    }
    return false;
}

void ActiveEffect::AddStack()
{
    ++m_numStacks;
    if (m_type == ET_immunity)
    {
        if (m_numStacks > m_immunities.size())
        {
            ::OutputDebugString("ActiveEffect ");
            ::OutputDebugString((LPCTSTR)Name());
            ::OutputDebugString(" has more stacks than immunity list\n");
        }
    }
}

bool ActiveEffect::RevokeStack()
{
    --m_numStacks;
    return (m_numStacks == 0);  // true if no stacks left
}

void ActiveEffect::SetStacks(size_t count)
{
    m_numStacks = count;
    if (m_type == ET_immunity)
    {
        if (m_numStacks > m_immunities.size())
        {
            ::OutputDebugString("ActiveEffect ");
            ::OutputDebugString((LPCTSTR)Name());
            ::OutputDebugString(" has more stacks than immunity list\n");
        }
    }
}

size_t ActiveEffect::NumStacks() const
{
    size_t count = m_numStacks;
    if (m_bHasStacksControl)
    {
        // look up the actual number of stacks
        // this could be a feat name or a slider name
        CWnd * pWnd = AfxGetMainWnd();
        CMainFrame * pMainWnd = dynamic_cast<CMainFrame*>(pWnd);
        const CStancesView * pStancesView = pMainWnd->GetStancesView();
        const SliderItem * pSlider = pStancesView->GetSlider(m_stacksControl);
        if (pSlider != NULL)
        {
            // its a controlling slider, update our stacks
            count = pSlider->m_position;
        }
        else
        {
            // must be a feat count
            std::list<TrainedFeat> currentFeats = m_pCharacter->CurrentFeats(MAX_LEVEL);
            count = TrainedCount(currentFeats, m_stacksControl);
        }
    }
    return count;
}

double ActiveEffect::TotalAmount(bool allowTruncate) const
{
    double value = 0.0;
    switch (m_type)
    {
    case ET_dice:
        value = 1;  // just need a non-zero value
        break;
    case ET_amount:
        if (m_dividerType != DT_statBonus)
        {
            value = m_amount * NumStacks();
        }
        else
        {
            // stacks don't multiply a stat bonus
            value = m_amount;
        }
        break;
    case ET_amountVector:
        {
            int index = NumStacks()-1;
            if (index >= 0)
            {
                if (index >= (int)m_amounts.size())
                {
                    index = m_amounts.size()-1;
                    ::OutputDebugString("ActiveEffect ");
                    ::OutputDebugString((LPCTSTR)Name());
                    ::OutputDebugString(" has more stacks than amount vector\n");
                }
                value = m_amounts[index];
            }
        }
        break;
    case ET_amountVectorPerClassLevel:
        {
            int index = m_levelStacks-1;
            if (index >= 0)
            {
                if (index >= (int)m_amounts.size())
                {
                    index = m_amounts.size()-1;
                    ::OutputDebugString("ActiveEffect ");
                    ::OutputDebugString((LPCTSTR)Name());
                    ::OutputDebugString(" has more stacks than amount vector\n");
                }
                value = m_amounts[index];
            }
        }
        break;
    case ET_amountPerLevel:
        value = m_amountPerLevel * m_levelStacks;
        break;
    case ET_amountPerAp:
        value = m_amount * NumStacks();
        break;
    case ET_immunity:
        value = Description() != "" ? 1 : 0; // just need a non-zero value
        break;
    case ET_DR:
        value = 1;      // just need a non-zero value
        break;
    default:
        value = 0.0;
        break;
    }
    if (allowTruncate && m_bWholeNumbersOnly)
    {
        // round down to whole number
        value = (int)(value);
    }
    if (m_clearValue)
    {
        // if its not a focus weapon or in required slot, ensure not listed
        value = 0;
    }
    return value;
}

void ActiveEffect::AddFeat(const std::string & featName)
{
    m_feats.push_back(featName);
}

void ActiveEffect::AddStance(const std::string & stance)
{
    m_stances.push_back(stance);
}

const std::vector<std::string> & ActiveEffect::Stances() const
{
    return m_stances;
}

void ActiveEffect::AddAnyOfStance(const std::string & stance)
{
    m_anyOfStances.push_back(stance);
}

const std::vector<std::string> & ActiveEffect::AnyOfStances() const
{
    return m_anyOfStances;
}

void ActiveEffect::AddNoneOfStance(const std::string & stance)
{
    m_noneOfStances.push_back(stance);
}

const std::vector<std::string> & ActiveEffect::NoneOfStances() const
{
    return m_noneOfStances;
}

const std::string & ActiveEffect::Tree() const
{
    return m_tree;
}

bool ActiveEffect::IsActive(const Character * pCharacter, WeaponType wt) const
{
    // return true if the required stance(s) are active
    bool active = true;
    for (size_t i = 0; i < m_stances.size(); ++i)
    {
        if (!pCharacter->IsStanceActive(m_stances[i], wt))
        {
            active = false;
        }
    }
    if (m_anyOfStances.size() > 0)
    {
        bool requiredStanceActive = false;
        for (size_t i = 0; i < m_anyOfStances.size(); ++i)
        {
            if (pCharacter->IsStanceActive(m_anyOfStances[i], wt))
            {
                requiredStanceActive = true;
                break;  // we have one, don't bother with the rest (if any)
            }
        }
        if (!requiredStanceActive)
        {
            active = false;
        }
    }
    if (m_noneOfStances.size() > 0)
    {
        bool badStanceActive = false;
        for (size_t i = 0; i < m_noneOfStances.size(); ++i)
        {
            if (pCharacter->IsStanceActive(m_noneOfStances[i], wt))
            {
                badStanceActive = true;
                break;  // we have one, don't bother with the rest (if any)
            }
        }
        if (badStanceActive)
        {
            active = false;
        }
    }
    // check any dependent feats are trained
    for (size_t i = 0; i < m_feats.size(); ++i)
    {
        if (!pCharacter->IsFeatTrained(m_feats[i]))
        {
            active = false;
        }
    }
    if (m_bHasWeaponType)
    {
        m_clearValue = !pCharacter->IsFocusWeapon(m_weaponType);
    }
    return active;
}

void ActiveEffect::SetIsPercentage(bool isPercentage)
{
    m_bIsPercentage = isPercentage;
}

bool ActiveEffect::IsPercentage() const
{
    return m_bIsPercentage;
}

void ActiveEffect::SetPercentageValue(double amount) const
{
    // this value is mutable
    m_percentageAmount = amount;
}

void ActiveEffect::SetWholeNumbersOnly()
{
    m_bWholeNumbersOnly = true;
}

void ActiveEffect::SetWeapon(WeaponType wt)
{
    m_bHasWeaponType = true;
    m_weaponType = wt;
}

bool ActiveEffect::operator<=(const ActiveEffect & other) const
{
    bool lessThanOrEqual = false;
    if (m_bonusType != Bonus_stacking           // stacking bonus's always stack
            && m_bonusType != Bonus_combatStyle // combat style bonus's always stack
            && m_bonusType != Bonus_destiny     // epic destiny bonus's always stack
            && m_bonusType != Bonus_reaper      // reaper bonus's always stack
            && m_bonusType != Bonus_mythic)    // combat style bonus's always stack
    {
        // must be the same type of bonus to allow a lessThan
        if (m_bonusType == other.m_bonusType
                && IsPercentage() == other.IsPercentage())
        {
            // comes down to the amount field
            lessThanOrEqual = (TotalAmount(false) <= other.TotalAmount(false));
        }
    }
    return lessThanOrEqual;
}

bool ActiveEffect::operator==(const ActiveEffect & other) const
{
    bool equal = false;
    if (m_type == other.m_type
            && m_effectName == other.m_effectName
            && m_bonusType == other.m_bonusType)
    {
        switch (m_type)
        {
        case ET_dice:
            equal = (m_dice == other.m_dice);
            break;
        case ET_amount:
        case ET_amountPerAp:
            equal = (m_amount == other.m_amount);
            break;
        case ET_amountVector:
            equal = (m_amounts == other.m_amounts);
            break;
        case ET_amountPerLevel:
            equal = (m_amountPerLevel == other.m_amountPerLevel);
            break;
        case ET_amountVectorPerClassLevel:
            equal = (m_amounts == other.m_amounts)
                    && (m_class == other.m_class);
            break;
        case ET_immunity:
            equal = (m_immunities == other.m_immunities);
            break;
        case ET_DR:
            equal = (m_drTypes == other.m_drTypes);
            break;
        default:
            equal = false;
            break;
        }
    }
    return equal;
}

std::string ActiveEffect::Description() const
{
    std::stringstream ss;
    switch (m_type)
    {
    case ET_amount:
    case ET_amountVector:
    case ET_amountPerLevel:
    case ET_amountPerAp:
    case ET_amountVectorPerClassLevel:
        ss << AmountAsText(false);
        break;
    case ET_dice:
        ss << m_dice.Description(NumStacks());
        break;
    case ET_immunity:
        if (NumStacks() <= m_immunities.size())
        {
            std::list<std::string>::const_iterator it = m_immunities.begin();
            std::advance(it, NumStacks() - 1);
            ss << (*it);
        }
        else
        {
            ss << m_immunities.back();
        }
        break;
    case ET_DR:
        // barbarian DR is displayed slightly differently
        if (m_drTypes.size() == 1 && m_drTypes.front() == DR_Percent)
        {
            ss << m_amount * NumStacks() << "%";
        }
        else
        {
            ss << m_amount * NumStacks() << "\\";
            std::list<DamageReductionType>::const_iterator it = m_drTypes.begin();
            while (it != m_drTypes.end())
            {
                if (it != m_drTypes.begin())
                {
                    ss << ",";
                }
                ss << EnumEntryText((*it), drTypeMap);
                ++it;
            }
        }
        break;
    }
    return ss.str();
}

void ActiveEffect::SetIsItemEffect()
{
    m_bIsItemEffect = true;
}

bool ActiveEffect::IsItemEffect() const
{
    return m_bIsItemEffect;
}

void ActiveEffect::SetDivider(double divider, DividerType type)
{
    m_divider = divider;
    m_dividerType = type;
}

Dice ActiveEffect::GetDice() const
{
    ASSERT(m_type == ET_dice);
    return m_dice;
}

void ActiveEffect::SetStacksControl(
        const std::string & control,
        Character * pCharacter)
{
    m_bHasStacksControl = true;
    m_stacksControl = control;  // thing that controls the number of stacks
    m_pCharacter = pCharacter;
}

void ActiveEffect::SetClassLevel(size_t count)
{
    m_levelStacks = count;
}
