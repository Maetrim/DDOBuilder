// ActiveEffect.cpp
//
#include "stdafx.h"
#include "ActiveEffect.h"

#include "Character.h"
#include "GlobalSupportFunctions.h"

//============================================================================
// ActiveEffect
ActiveEffect::ActiveEffect() :
    m_type(ET_unknown),
    m_effectName("Not setup"),
    m_numStacks(0),
    m_amount(0),
    m_bHasDice(false),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(""),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        ActiveEffectType type,
        const std::string & name,
        size_t stacks,
        double amount,
        const std::string & tree) :
    m_type(type),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(amount),
    m_bHasDice(false),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        ActiveEffectType type,
        const std::string & name,
        size_t stacks,
        const Dice & dice,
        const std::string & tree) :
    m_type(type),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),            // not used
    m_dice(dice),
    m_bHasDice(true),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_tree(tree),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        ActiveEffectType type,
        const std::string & name,
        size_t stacks,
        const std::vector<double> & amounts) :
    m_type(type),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),                // not used
    m_amounts(amounts),
    m_bHasDice(false),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(0),
    m_class(Class_Unknown)
{
}

ActiveEffect::ActiveEffect(
        ActiveEffectType type,
        const std::string & name,
        double amountPerLevel,
        size_t stacks,
        ClassType classType) :
    m_type(type),
    m_effectName(name),
    m_numStacks(stacks),
    m_amount(0),                // not used
    m_bHasDice(false),
    m_bHasEnergy(false),
    m_energy(Energy_Unknown),
    m_bt(Breakdown_Unknown),
    m_amountPerLevel(amountPerLevel),
    m_class(classType)
{
    ASSERT(type == ET_enhancementPerLevel);
}

ActiveEffectType ActiveEffect::Type() const
{
    return m_type;
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
    if (m_bHasDice)
    {
        text.Format("%dD%d", m_dice.Number(), m_dice.Sides());
        // optional effects such as its Fire damage
        if (m_bHasEnergy)
        {
            text += " ";
            text += EnumEntryText(m_energy, energyTypeMap);
        }
    }
    else
    {
        switch (m_type)
        {
        case ET_enhancement:
            text.Format("%.2f", (double)m_amounts[m_numStacks-1]);
            break;
        case ET_enhancementPerLevel:
            text.Format("%.2f", m_amountPerLevel * m_numStacks);
            break;
        default:
        case ET_feat:
        case ET_enhancementPerAP:
            text.Format("%.2f", m_amount * m_numStacks);
            break;
        }
    }
    return text;
}

double ActiveEffect::Amount() const
{
    return m_amount;
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
    if (m_type == ET_enhancementPerLevel)
    {
        return (type == m_class);
    }
    return false;
}

void ActiveEffect::AddStack()
{
    ++m_numStacks;
}

bool ActiveEffect::RevokeStack()
{
    --m_numStacks;
    return (m_numStacks == 0);  // true if no stacks left
}

void ActiveEffect::SetStacks(size_t count)
{
    m_numStacks = count;
}

double ActiveEffect::TotalAmount() const
{
    switch (m_type)
    {
    case ET_enhancement:
        return (double)m_amounts[m_numStacks-1];        // 0 based
    case ET_enhancementPerLevel:
        ASSERT(m_numStacks > 0);
        return m_amountPerLevel * m_numStacks;
    default:
    case ET_feat:
    case ET_enhancementPerAP:
        ASSERT(m_numStacks > 0);
        return m_amount * m_numStacks;
    }
    ASSERT(false);
    return 0;
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

bool ActiveEffect::IsActive(const Character * pCharacter) const
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
    return active;
}

bool ActiveEffect::operator<=(const ActiveEffect & other) const
{
    bool lessThanOrEqual = false;
    // must be the same type of bonus to allow a lessThan
    if (m_type == other.m_type
            && m_effectName == other.m_effectName)
    {
        // comes down to the amount field
        if (m_type != ET_enhancement)
        {
            lessThanOrEqual = (m_amount <= other.m_amount);
        }
        else
        {
            lessThanOrEqual = (m_amounts <= other.m_amounts);
        }
    }
    return lessThanOrEqual;
}

bool ActiveEffect::operator==(const ActiveEffect & other) const
{
    bool equal = false;
    // must be the same type of bonus to allow a lessThan
    if (m_type == other.m_type
            && m_effectName == other.m_effectName)
    {
        if (m_type != ET_enhancement)
        {
            equal = (m_amount == other.m_amount);
        }
        else
        {
            equal = (m_amounts == other.m_amounts);
        }
    }
    return equal;
}
