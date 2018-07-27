// OptionalBuff.cpp
//
#include "StdAfx.h"
#include "OptionalBuff.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT OptionalBuff

namespace
{
    const wchar_t f_saxElementName[] = L"OptionalBuff";
    DL_DEFINE_NAMES(OptionalBuff_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

OptionalBuff::OptionalBuff() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(OptionalBuff_PROPERTIES)
}

OptionalBuff::OptionalBuff(const XmlLib::SaxString & objectName) :
    XmlLib::SaxContentElement(objectName, f_verCurrent)
{
    DL_INIT(OptionalBuff_PROPERTIES)
}

DL_DEFINE_ACCESS(OptionalBuff_PROPERTIES)

XmlLib::SaxContentElementInterface * OptionalBuff::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(OptionalBuff_PROPERTIES)

    return subHandler;
}

void OptionalBuff::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(OptionalBuff_PROPERTIES)
}

void OptionalBuff::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(OptionalBuff_PROPERTIES)
    writer->EndElement();
}

void OptionalBuff::VerifyObject() const
{
    bool ok = true;
    std::stringstream ss;
    ss << "===== Optional Buff " << m_Name << "=====\n";
    // check the item effects also
    std::vector<Effect>::const_iterator it = m_Effects.begin();
    while (it != m_Effects.end())
    {
        ok &= (*it).VerifyObject(&ss);
        ++it;
    }

    if (!ok)
    {
        ::OutputDebugString(ss.str().c_str());
    }
}
