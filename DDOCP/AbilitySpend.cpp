// AbilitySpend.cpp
//
#include "StdAfx.h"
#include "AbilitySpend.h"
#include "XmlLib\SaxWriter.h"
#include <boost/static_assert.hpp>

#define DL_ELEMENT AbilitySpend

namespace
{
    const wchar_t f_saxElementName[] = L"AbilitySpend";
    DL_DEFINE_NAMES(AbilitySpend_PROPERTIES)

    const unsigned f_verCurrent = 1;

    size_t c_maxSpend = 10;
    int c_statIncreaseCosts[] = {1, 1, 1, 1, 1, 1, 2, 2, 3, 3};
    const unsigned arraySize = sizeof(c_statIncreaseCosts) / sizeof(*c_statIncreaseCosts);
    BOOST_STATIC_ASSERT(arraySize == 10);
}

AbilitySpend::AbilitySpend() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(AbilitySpend_PROPERTIES)
}

DL_DEFINE_ACCESS(AbilitySpend_PROPERTIES)

XmlLib::SaxContentElementInterface * AbilitySpend::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(AbilitySpend_PROPERTIES)

    return subHandler;
}

void AbilitySpend::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(AbilitySpend_PROPERTIES)
    if (m_StrSpend > c_maxSpend)
    {
        SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::m_StrSpend had invalid value");
    }
    if (m_DexSpend > c_maxSpend)
    {
        SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::m_DexSpend had invalid value");
    }
    if (m_ConSpend > c_maxSpend)
    {
        SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::m_ConSpend had invalid value");
    }
    if (m_IntSpend > c_maxSpend)
    {
        SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::m_IntSpend had invalid value");
    }
    if (m_WisSpend > c_maxSpend)
    {
        SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::m_WisSpend had invalid value");
    }
    if (m_ChaSpend > c_maxSpend)
    {
        SAXASSERT(false, DL_STRINGIZE(DL_ELEMENT) "::m_ChaSpend had invalid value");
    }
    if (!m_hasUserSelectedSpend)
    {
        m_hasUserSelectedSpend = true;
    }
}

void AbilitySpend::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(AbilitySpend_PROPERTIES)
    writer->EndElement();
}

size_t AbilitySpend::PointsSpent() const
{
    // sum how many build points have been spent on each ability
    size_t totalSpent = 0;
    totalSpent += GetAbilitySpendPoints(Ability_Strength);
    totalSpent += GetAbilitySpendPoints(Ability_Dexterity);
    totalSpent += GetAbilitySpendPoints(Ability_Constitution);
    totalSpent += GetAbilitySpendPoints(Ability_Intelligence);
    totalSpent += GetAbilitySpendPoints(Ability_Wisdom);
    totalSpent += GetAbilitySpendPoints(Ability_Charisma);
    return totalSpent;
}

bool AbilitySpend::CanSpendOnAbility(
        AbilityType ability) const
{
    bool canSpend = false;
    size_t currentSpend = GetAbilitySpend(ability);
    if (currentSpend < c_maxSpend)
    {
        // its not at max, but are there enough points left to
        // spend to the next level?
        int availablePoints = ((int)m_AvailableSpend - (int)PointsSpent());
        int nextCost = c_statIncreaseCosts[currentSpend];
        canSpend = (availablePoints >= nextCost); // true if enough points available
    }
    return canSpend;
}

bool AbilitySpend::CanRevokeSpend(
        AbilityType ability) const
{
    bool canRevokeSpend = false;
    size_t currentSpend = GetAbilitySpend(ability);
    // can always undo if any points spent
    canRevokeSpend = (currentSpend > 0);
    return canRevokeSpend;
}

size_t AbilitySpend::NextPointsSpentCost(
        AbilityType ability) const
{
    size_t nextCost = 0;
    size_t currentSpend = GetAbilitySpend(ability);
    if (currentSpend < c_maxSpend)
    {
        // its not at max, but are there enough points left to
        // spend to the next level?
        nextCost = c_statIncreaseCosts[currentSpend];
    }
    return nextCost;
}

void AbilitySpend::SpendOnAbility(
        AbilityType ability)
{
    switch (ability)
    {
    case Ability_Strength:
        ++m_StrSpend;
        break;
    case Ability_Dexterity:
        ++m_DexSpend;
        break;
    case Ability_Constitution:
        ++m_ConSpend;
        break;
    case Ability_Intelligence:
        ++m_IntSpend;
        break;
    case Ability_Wisdom:
        ++m_WisSpend;
        break;
    case Ability_Charisma:
        ++m_ChaSpend;
        break;
    default:
        ASSERT(FALSE);
        break;
    }
}

void AbilitySpend::RevokeSpendOnAbility(
        AbilityType ability)
{
    switch (ability)
    {
    case Ability_Strength:
        --m_StrSpend;
        break;
    case Ability_Dexterity:
        --m_DexSpend;
        break;
    case Ability_Constitution:
        --m_ConSpend;
        break;
    case Ability_Intelligence:
        --m_IntSpend;
        break;
    case Ability_Wisdom:
        --m_WisSpend;
        break;
    case Ability_Charisma:
        --m_ChaSpend;
        break;
    default:
        ASSERT(FALSE);
        break;
    }
}

size_t AbilitySpend::GetAbilitySpend(
        AbilityType ability) const
{
    size_t spent = c_maxSpend;  // assume
    switch (ability)
    {
    case Ability_Strength:
        spent = m_StrSpend;
        break;
    case Ability_Dexterity:
        spent = m_DexSpend;
        break;
    case Ability_Constitution:
        spent = m_ConSpend;
        break;
    case Ability_Intelligence:
        spent = m_IntSpend;
        break;
    case Ability_Wisdom:
        spent = m_WisSpend;
        break;
    case Ability_Charisma:
        spent = m_ChaSpend;
        break;
    default:
        ASSERT(FALSE);
        break;
    }
    return spent;
}

size_t AbilitySpend::GetAbilitySpendPoints(
        AbilityType ability) const
{
    size_t spentPoints = 0;
    size_t abilitySpend = GetAbilitySpend(ability);
    for (size_t i = 0; i < abilitySpend; ++i)
    {
        spentPoints += c_statIncreaseCosts[i];
    }
    return spentPoints;
}

