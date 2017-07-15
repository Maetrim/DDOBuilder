// Dice.cpp
//
#include "StdAfx.h"
#include "Dice.h"
#include "XmlLib\SaxWriter.h"

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
    return m_Number[index];
}

size_t Dice::Sides(size_t index) const
{
    ASSERT(index < m_Sides.size());
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
