// SelectorExclusion.cpp
//
#include "StdAfx.h"
#include "SelectorExclusion.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT SelectorExclusion

namespace
{
    const wchar_t f_saxElementName[] = L"SelectorExclusion";
    DL_DEFINE_NAMES(SelectorExclusion_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

SelectorExclusion::SelectorExclusion() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SelectorExclusion_PROPERTIES)
}

DL_DEFINE_ACCESS(SelectorExclusion_PROPERTIES)

XmlLib::SaxContentElementInterface * SelectorExclusion::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SelectorExclusion_PROPERTIES)

    return subHandler;
}

void SelectorExclusion::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SelectorExclusion_PROPERTIES)
}

void SelectorExclusion::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SelectorExclusion_PROPERTIES)
    writer->EndElement();
}
