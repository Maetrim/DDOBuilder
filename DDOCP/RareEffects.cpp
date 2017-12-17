// RareEffects.cpp
//
#include "StdAfx.h"
#include "RareEffects.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT RareEffects

namespace
{
    const wchar_t f_saxElementName[] = L"RareEffects";
    DL_DEFINE_NAMES(RareEffects_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

RareEffects::RareEffects() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(RareEffects_PROPERTIES)
}

DL_DEFINE_ACCESS(RareEffects_PROPERTIES)

XmlLib::SaxContentElementInterface * RareEffects::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(RareEffects_PROPERTIES)

    return subHandler;
}

void RareEffects::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(RareEffects_PROPERTIES)
}

void RareEffects::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(RareEffects_PROPERTIES)
    writer->EndElement();
}
