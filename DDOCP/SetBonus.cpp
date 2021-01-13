// SetBonus.cpp
//
#include "StdAfx.h"
#include "SetBonus.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT SetBonus

namespace
{
    const wchar_t f_saxElementName[] = L"SetBonus";
    DL_DEFINE_NAMES(SetBonus_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

SetBonus::SetBonus() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SetBonus_PROPERTIES)
}

DL_DEFINE_ACCESS(SetBonus_PROPERTIES)

XmlLib::SaxContentElementInterface * SetBonus::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SetBonus_PROPERTIES)

    return subHandler;
}

void SetBonus::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SetBonus_PROPERTIES)
}

void SetBonus::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SetBonus_PROPERTIES)
    writer->EndElement();
}

void SetBonus::VerifyObject() const
{
    bool ok = true;
    std::stringstream ss;
    if (!ImageFileExists(IT_augment, Icon()))
    {
        ss << "SetBonus " << Name() << " is missing image file \"" << Icon() << "\"\n";
        ::OutputDebugString(ss.str().c_str());
    }
}

bool SetBonus::operator<(const SetBonus & other) const
{
    // (assumes all set bonus names are unique)
    // sort by name
    return (Name() < other.Name());
}
