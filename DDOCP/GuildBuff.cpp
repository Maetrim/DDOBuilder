// GuildBuff.cpp
//
#include "StdAfx.h"
#include "GuildBuff.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT GuildBuff

namespace
{
    const wchar_t f_saxElementName[] = L"GuildBuff";
    DL_DEFINE_NAMES(GuildBuff_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

GuildBuff::GuildBuff() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(GuildBuff_PROPERTIES)
}

GuildBuff::GuildBuff(const XmlLib::SaxString & objectName) :
    XmlLib::SaxContentElement(objectName, f_verCurrent)
{
    DL_INIT(GuildBuff_PROPERTIES)
}

DL_DEFINE_ACCESS(GuildBuff_PROPERTIES)

XmlLib::SaxContentElementInterface * GuildBuff::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(GuildBuff_PROPERTIES)

    return subHandler;
}

void GuildBuff::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(GuildBuff_PROPERTIES)
}

void GuildBuff::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(GuildBuff_PROPERTIES)
    writer->EndElement();
}
