// TwistOfFate.cpp
//
#include "StdAfx.h"
#include "TwistOfFate.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT TwistOfFate

namespace
{
    const wchar_t f_saxElementName[] = L"TwistOfFate";
    DL_DEFINE_NAMES(TwistOfFate_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

TwistOfFate::TwistOfFate() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(TwistOfFate_PROPERTIES)
}

TwistOfFate::TwistOfFate(const std::string & name, const std::string & icon) :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
}

DL_DEFINE_ACCESS(TwistOfFate_PROPERTIES)

XmlLib::SaxContentElementInterface * TwistOfFate::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(TwistOfFate_PROPERTIES)

    return subHandler;
}

void TwistOfFate::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(TwistOfFate_PROPERTIES)
}

void TwistOfFate::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(TwistOfFate_PROPERTIES)
    writer->EndElement();
}

bool TwistOfFate::VerifyObject(std::stringstream * ss) const
{
    bool ok = true;
    if (!ok)
    {
        (*ss) << "TwistOfFate ???\n";
    }
    return ok;
}

void TwistOfFate::IncrementTier()
{
    ASSERT(m_Tier < MAX_TWIST_LEVEL);
    m_Tier++;
}

void TwistOfFate::DecrementTier()
{
    ASSERT(m_Tier > 0);
    m_Tier--;
}
