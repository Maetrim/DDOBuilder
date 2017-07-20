// ItemAugment.cpp
//
#include "StdAfx.h"
#include "ItemAugment.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT ItemAugment

namespace
{
    const wchar_t f_saxElementName[] = L"ItemAugment";
    DL_DEFINE_NAMES(ItemAugment_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

ItemAugment::ItemAugment() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(ItemAugment_PROPERTIES)
}

DL_DEFINE_ACCESS(ItemAugment_PROPERTIES)

XmlLib::SaxContentElementInterface * ItemAugment::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(ItemAugment_PROPERTIES)

    return subHandler;
}

void ItemAugment::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(ItemAugment_PROPERTIES)
}

void ItemAugment::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(ItemAugment_PROPERTIES)
    writer->EndElement();
}
