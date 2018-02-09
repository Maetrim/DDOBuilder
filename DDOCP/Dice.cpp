// Dice.cpp
//
#include "StdAfx.h"
#include "Dice.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Dice

namespace
{
    const wchar_t f_saxElementName[] = L"Dice";
    DL_DEFINE_NAMES(Dice_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Dice::Dice() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Dice_PROPERTIES)
    m_Number.push_back(1);
    m_Sides.push_back(6);   // need a default dice size, changed on read
}

DL_DEFINE_ACCESS(Dice_PROPERTIES)

XmlLib::SaxContentElementInterface * Dice::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Dice_PROPERTIES)

    return subHandler;
}

void Dice::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Dice_PROPERTIES)
}

void Dice::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Dice_PROPERTIES)
    writer->EndElement();
}

size_t Dice::Number(size_t index) const
{
    ASSERT(index < m_Number.size());
    index = min(index, m_Number.size() -1 );
    return m_Number[index];
}

size_t Dice::Sides(size_t index) const
{
    ASSERT(index < m_Sides.size());
    index = min(index, m_Sides.size() -1 );
    return m_Sides[index];
}

int Dice::Bonus(size_t index) const
{
    if (m_hasBonus)
    {
        ASSERT(index < m_Bonus.size());
        return m_Bonus[index];
    }
    else
    {
        return 0;
    }
}

bool Dice::operator==(const Dice & other) const
{
    bool equal = (m_Number == other.m_Number)
            && (m_Sides == other.m_Sides)
            && (HasBonus() == other.HasBonus())
            && m_Bonus == other.m_Bonus;
    return equal;
}

std::string Dice::Description(size_t numStacks) const
{
    std::stringstream ss;
    if (HasScalesWithMeleePower()
            || HasScalesWithRangedPower()
            || HasScalesWithSpellPower())
    {
        ss << "(";
    }
    ss << Number(numStacks-1) << "D"<< Sides(numStacks-1);
    if (m_hasBonus)
    {
        ss << "+" << Bonus(numStacks-1);
    }
    if (m_hasEnergy)
    {
        ss << " " << EnumEntryText(m_Energy, energyTypeMap);
    }
    if (HasScalesWithMeleePower()
            || HasScalesWithRangedPower()
            || HasScalesWithSpellPower())
    {
        if (HasScalesWithSpellPower())
        {
            ss << ") * " << (LPCSTR)EnumEntryText(SpellPower(), spellPowerTypeMap)
                    << " Spell Power";
        }
        else
        {
            if (HasScalesWithMeleePower() && HasScalesWithRangedPower())
            {
                ss << ") * MAX(Melee, Ranged) Power";
            }
            else if (HasScalesWithMeleePower())
            {
                ss << ") * Melee Power";
            }
            else
            {
                ss << ") * Ranged Power";
            }
        }
    }
    return ss.str();
}

