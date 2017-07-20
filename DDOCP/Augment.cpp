// Augment.cpp
//
#include "StdAfx.h"
#include "Augment.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT Augment

namespace
{
    const wchar_t f_saxElementName[] = L"Augment";
    DL_DEFINE_NAMES(Augment_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Augment::Augment() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Augment_PROPERTIES)
}

DL_DEFINE_ACCESS(Augment_PROPERTIES)

XmlLib::SaxContentElementInterface * Augment::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Augment_PROPERTIES)

    return subHandler;
}

void Augment::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Augment_PROPERTIES)
}

void Augment::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Augment_PROPERTIES)
    writer->EndElement();
}

bool Augment::IsCompatibleWithSlot(AugmentType type) const
{
    // by default it has to match
    bool compatible = (Type() == type);
    // all augments types must match exactly unless they are compound colour slots
    switch (type)
    {
        case Augment_Green:
            // green augments slots can take blue, yellow or white
            compatible = (Type() == Augment_Blue || Type() == Augment_Yellow || Type() == Augment_White);
            break;
        case Augment_Orange:
            // orange augments slots can take red, yellow or white
            compatible = (Type() == Augment_Red || Type() == Augment_Yellow || Type() == Augment_White);
            break;
        case Augment_Purple:
            // purple augments slots can take blue, red or white
            compatible = (Type() == Augment_Blue || Type() == Augment_Red || Type() == Augment_White);
            break;
        default:
            // all others covered by (Type() == type)
            break;
    }
    return compatible;
}

