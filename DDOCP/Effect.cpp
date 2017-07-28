// Effect.cpp
//
#include "StdAfx.h"
#include "Effect.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT Effect

namespace
{
    const wchar_t f_saxElementName[] = L"Effect";
    DL_DEFINE_NAMES(Effect_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Effect::Effect() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Effect_PROPERTIES)
}

DL_DEFINE_ACCESS(Effect_PROPERTIES)

XmlLib::SaxContentElementInterface * Effect::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Effect_PROPERTIES)

    return subHandler;
}

void Effect::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Effect_PROPERTIES)
}

void Effect::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Effect_PROPERTIES)
    writer->EndElement();
}

double Effect::Amount(size_t tier) const
{
    double amount = 0;
    if (HasAmountVector())
    {
        ASSERT(tier <= m_AmountVector.size());
        amount = m_AmountVector.at(tier-1);     // 0 based vs 1 based
    }
    else
    {
        ASSERT(HasAmount());
        ASSERT(tier == 1);
        amount = Amount();
    }
    return amount;
}
