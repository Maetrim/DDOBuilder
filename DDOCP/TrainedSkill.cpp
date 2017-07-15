// TrainedSkill.cpp
//
#include "StdAfx.h"
#include "TrainedSkill.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT TrainedSkill

namespace
{
    const wchar_t f_saxElementName[] = L"TrainedSkill";
    DL_DEFINE_NAMES(TrainedSkill_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

TrainedSkill::TrainedSkill() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(TrainedSkill_PROPERTIES)
}

DL_DEFINE_ACCESS(TrainedSkill_PROPERTIES)

XmlLib::SaxContentElementInterface * TrainedSkill::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(TrainedSkill_PROPERTIES)

    return subHandler;
}

void TrainedSkill::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(TrainedSkill_PROPERTIES)
}

void TrainedSkill::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(TrainedSkill_PROPERTIES)
    writer->EndElement();
}
