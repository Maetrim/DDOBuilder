// AutomaticAcquisition.cpp
//
#include "StdAfx.h"
#include "AutomaticAcquisition.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT AutomaticAcquisition

namespace
{
    const wchar_t f_saxElementName[] = L"AutomaticAcquisition";
    DL_DEFINE_NAMES(AutomaticAcquisition_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

AutomaticAcquisition::AutomaticAcquisition() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(AutomaticAcquisition_PROPERTIES)
}

DL_DEFINE_ACCESS(AutomaticAcquisition_PROPERTIES)

XmlLib::SaxContentElementInterface * AutomaticAcquisition::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(AutomaticAcquisition_PROPERTIES)

    return subHandler;
}

void AutomaticAcquisition::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(AutomaticAcquisition_PROPERTIES)
}

void AutomaticAcquisition::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(AutomaticAcquisition_PROPERTIES)
    writer->EndElement();
}

bool AutomaticAcquisition::VerifyObject(std::stringstream * ss) const
{
    bool ok = true;
    // must have at least one optional element
    ok = HasBAB()
            || HasClass()
            || HasLevel()
            || HasRace()
            || HasSpecificLevel()
            || HasRequirementsToGain();
    if (!ok)
    {
        (*ss) << "AutomaticAcquisition has missing elements.\n";
    }
    return ok;
}

