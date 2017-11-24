// SlotUpgrade.cpp
//
#include "StdAfx.h"
#include "SlotUpgrade.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT SlotUpgrade

namespace
{
    const wchar_t f_saxElementName[] = L"SlotUpgrade";
    DL_DEFINE_NAMES(SlotUpgrade_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

SlotUpgrade::SlotUpgrade() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SlotUpgrade_PROPERTIES)
}

DL_DEFINE_ACCESS(SlotUpgrade_PROPERTIES)

XmlLib::SaxContentElementInterface * SlotUpgrade::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SlotUpgrade_PROPERTIES)

    return subHandler;
}

void SlotUpgrade::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SlotUpgrade_PROPERTIES)
}

void SlotUpgrade::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SlotUpgrade_PROPERTIES)
    writer->EndElement();
}
