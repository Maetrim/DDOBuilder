// EffectDescription.cpp
//
#include "StdAfx.h"
#include "EffectDescription.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT EffectDescription

namespace
{
    const wchar_t f_saxElementName[] = L"EffectDescription";
    DL_DEFINE_NAMES(EffectDescription_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EffectDescription::EffectDescription() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(EffectDescription_PROPERTIES)
}

DL_DEFINE_ACCESS(EffectDescription_PROPERTIES)

XmlLib::SaxContentElementInterface * EffectDescription::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EffectDescription_PROPERTIES)

    return subHandler;
}

void EffectDescription::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EffectDescription_PROPERTIES)
}

void EffectDescription::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EffectDescription_PROPERTIES)
    writer->EndElement();
}
