// ActiveEffect.cpp
//
#include "stdafx.h"
#include "ActiveEffect.h"

#include "Character.h"
#include "GlobalSupportFunctions.h"

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
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_bHasInventoryType(false),
    m_slot(Inventory_Unknown),
    m_clearValue(false)
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
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_bHasInventoryType(false),
    m_slot(Inventory_Unknown),
    m_clearValue(false)
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
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_bHasInventoryType(false),
    m_slot(Inventory_Unknown),
    m_clearValue(false)
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
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_bHasInventoryType(false),
    m_slot(Inventory_Unknown),
    m_clearValue(false)
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
    m_class(Class_Unknown),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_bHasInventoryType(false),
    m_slot(Inventory_Unknown),
    m_clearValue(false)
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
    m_numStacks(0),
    m_amount(0),                // not used
    m_amounts(amounts),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(classType),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_bHasInventoryType(false),
    m_slot(Inventory_Unknown),
    m_clearValue(false)
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
    m_numStacks(stacks),
    m_amount(0),                // not used
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(amountPerLevel),
    m_class(classType),
    m_bIsPercentage(false),
    m_percentageAmount(0),
    m_bWholeNumbersOnly(false),
    m_bHasWeaponType(false),
    m_weaponType(Weapon_Unknown),
    m_bHasInventoryType(false),
    m_slot(Inventory_Unknown),
    m_clearValue(false)
{
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
    text.Format("%d", m_numStacks);
    return text;
}

CString ActiveEffect::AmountAsText() const
{
    CString text;
    switch (m_type)
    {
    case ET_dice:
        text.Format("%dD%d",
                (int)m_dice.Number(m_numStacks),
                (int)m_dice.Sides(m_numStacks));
        if (m_dice.HasBonus())
        {
            CString bonus;
            bonus.Format("+%d", (int)m_dice.Bonus(m_numStacks));
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
        // optional effects such as its Fire damage
        if (m_bHasEnergy)
        {
            text += " ";
            text += EnumEntryText(m_energy, energyTypeMap);
        }
        break;
    case ET_amount:
        text.Format("%.2f", m_amount * m_numStacks);
        break;
    case ET_amountVector:
    case ET_amountVectorPerClassLevel: // handled the same
        {
            size_t index = m_numStacks-1;
            if (index >= m_amounts.size())
            {
                index = m_amounts.size()-1;
                ::OutputDebugString("ActiveEffect ");
                ::OutputDebugString((LPCTSTR)Name());
                ::OutputDebugString(" has more stacks than amount vector\n");
            }
            text.Format("%.2f", m_amounts[index]);
        }
        break;
    case ET_amountPerLevel:
        text.Format("%.2f", m_amountPerLevel * m_numStacks);
        break;
    case ET_amountPerAp:
        text.Format("%.2f", m_amount * m_numStacks);
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
                (int)m_dice.Number(m_numStacks),
                (int)m_dice.Sides(m_numStacks));
        if (m_dice.HasBonus())
        {
            CString bonus;
            bonus.Format("+%d", (int)m_dice.Bonus(m_numStacks));
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
        // optional effects such as its Fire damage
        if (m_bHasEnergy)
        {
            text += " ";
            text += EnumEntryText(m_energy, energyTypeMap);
        }
        break;
    case ET_amount:
        text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amount * m_numStacks);
        break;
    case ET_amountVector:
    case ET_amountVectorPerClassLevel: // handled the same
        {
            size_t index = m_numStacks-1;
            if (index >= m_amounts.size())
            {
                index = m_amounts.size()-1;
                ::OutputDebugString("ActiveEffect ");
                ::OutputDebugString((LPCTSTR)Name());
                ::OutputDebugString(" has more stacks than amount vector\n");
            }
            text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amounts[index]);
        }
        break;
    case ET_amountPerLevel:
        text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amountPerLevel * m_numStacks);
        break;
    case ET_amountPerAp:
        text.Format("%.0f (%.0f%%)", m_percentageAmount, m_amount * m_numStacks);
        break;
    default:
        text = "???";
        break;
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
    m_amount = amount;
}

void ActiveEffect::SetEnergy(EnergyType type)
{
    m_bHasEnergy = (type != Energy_Unknown);
    m_energy = type;
}

void ActiveEffect::SetBreakdownDependency(BreakdownType bt)
{
    m_bt = bt;
}

bool ActiveEffect::HasBreakdownDependency(BreakdownType bt) const
{
    return (bt == m_bt);
}

bool ActiveEffect::HasClass(ClassType type) const
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
    if (m_type == ET_amountVector
            || m_type == ET_amountVectorPerClassLevel)
    {
        if (m_numStacks > m_amounts.size())
        {
            ::OutputDebugString("ActiveEffect ");
            ::OutputDebugString((LPCTSTR)Name());
            ::OutputDebugString(" has more stacks than amount vector\n");
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
    if (m_type == ET_amountVector
            || m_type == ET_amountVectorPerClassLevel)
    {
        if (m_numStacks > m_amounts.size())
        {
            ::OutputDebugString("ActiveEffect ");
            ::OutputDebugString((LPCTSTR)Name());
            ::OutputDebugString(" has more stacks than amount vector\n");
        }
    }
}

size_t ActiveEffect::NumStacks() const
{
    return m_numStacks;
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
        value = m_amount * m_numStacks;
        break;
    case ET_amountVector:
    case ET_amountVectorPerClassLevel:
        {
            size_t index = m_numStacks-1;
            if (index >= m_amounts.size())
            {
                index = m_amounts.size()-1;
                ::OutputDebugString("ActiveEffect ");
                ::OutputDebugString((LPCTSTR)Name());
                ::OutputDebugString(" has more stacks than amount vector\n");
            }
            value = m_amounts[index];
        }
        break;
    case ET_amountPerLevel:
        value = m_amountPerLevel * m_numStacks;
        break;
    case ET_amountPerAp:
        value = m_amount * m_numStacks;
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

const std::string & ActiveEffect::Tree() const
{
    return m_tree;
}

bool ActiveEffect::IsActive(const Character * pCharacter, InventorySlotType slot) const
{
    // return true if the required stance(s) are active
    bool active = true;
    for (size_t i = 0; i < m_stances.size(); ++i)
    {
        if (!pCharacter->IsStanceActive(m_stances[i]))
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
    if (m_bHasInventoryType)
    {
        m_clearValue = (m_slot != slot);
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
            && m_bonusType != Bonus_destiny     // epic destiny bonus's always stack
            && m_bonusType != Bonus_reaper      // reaper bonus's always stack
            && m_bonusType != Bonus_mythic)     // mythic bonus's always stack
    {
        // must be the same type of bonus to allow a lessThan
        if (m_type == other.m_type
                //&& m_effectName == other.m_effectName
                && m_bonusType == other.m_bonusType)
        {
            // comes down to the amount field
            if (m_type == other.m_type)
            {
                lessThanOrEqual = (TotalAmount(false) <= other.TotalAmount(false));
            }
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
        ss << AmountAsText();
        break;
    case ET_dice:
        ss << m_dice.Description(m_numStacks);
        break;
    }
    return ss.str();
}

void ActiveEffect::SetSlot(InventorySlotType slot)
{
    m_bHasInventoryType = true;
    m_slot = slot;
}
