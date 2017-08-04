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
    // has to be in any of the augment types list to be a match
    bool compatible = false;
    std::list<AugmentType>::const_iterator it = m_Type.begin();
    while (it != m_Type.end())
    {
        if ((*it) == type)
        {
            compatible = true;
            break;  // no need to check the rest
        }
        ++it;
    }
    return compatible;
}

