// SelectorExclusions.cpp
//
#include "StdAfx.h"
#include "SelectorExclusions.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT SelectorExclusions

namespace
{
    const wchar_t f_saxElementName[] = L"SelectorExclusions";
    DL_DEFINE_NAMES(SelectorExclusions_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

SelectorExclusions::SelectorExclusions() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SelectorExclusions_PROPERTIES)
}

DL_DEFINE_ACCESS(SelectorExclusions_PROPERTIES)

XmlLib::SaxContentElementInterface * SelectorExclusions::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SelectorExclusions_PROPERTIES)

    return subHandler;
}

void SelectorExclusions::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SelectorExclusions_PROPERTIES)
}

void SelectorExclusions::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SelectorExclusions_PROPERTIES)
    writer->EndElement();
}

bool SelectorExclusions::VerifyObject(
        std::stringstream * ss,
        const std::list<EnhancementTree> & trees,
        const std::list<Feat> & feats) const
{
    bool ok = true;
    // TBD
    return ok;
}
