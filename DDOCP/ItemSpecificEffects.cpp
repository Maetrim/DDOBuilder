// ItemSpecificEffects.cpp
//
#include "StdAfx.h"
#include "ItemSpecificEffects.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT ItemSpecificEffects

namespace
{
    const wchar_t f_saxElementName[] = L"ItemSpecificEffects";
    DL_DEFINE_NAMES(ItemSpecificEffects_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

ItemSpecificEffects::ItemSpecificEffects() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(ItemSpecificEffects_PROPERTIES)
}

DL_DEFINE_ACCESS(ItemSpecificEffects_PROPERTIES)

XmlLib::SaxContentElementInterface * ItemSpecificEffects::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(ItemSpecificEffects_PROPERTIES)

    return subHandler;
}

void ItemSpecificEffects::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(ItemSpecificEffects_PROPERTIES)
}

void ItemSpecificEffects::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(ItemSpecificEffects_PROPERTIES)
    writer->EndElement();
}

bool ItemSpecificEffects::VerifyObject(std::stringstream * ss) const
{
    bool ok = true;
    std::vector<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(ss);
        ++it;
    }
    return ok;
}
