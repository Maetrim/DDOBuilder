// SkillTomes.cpp
//
#include "StdAfx.h"
#include "SkillTomes.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT SkillTomes

namespace
{
    const wchar_t f_saxElementName[] = L"SkillTomes";
    DL_DEFINE_NAMES(SkillTomes_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

SkillTomes::SkillTomes() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SkillTomes_PROPERTIES)
}

DL_DEFINE_ACCESS(SkillTomes_PROPERTIES)

XmlLib::SaxContentElementInterface * SkillTomes::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SkillTomes_PROPERTIES)

    return subHandler;
}

void SkillTomes::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SkillTomes_PROPERTIES)
}

void SkillTomes::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SkillTomes_PROPERTIES)
    writer->EndElement();
}
