// Stance.cpp
//
#include "StdAfx.h"
#include "Stance.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Stance

namespace
{
    const wchar_t f_saxElementName[] = L"Stance";
    DL_DEFINE_NAMES(Stance_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Stance::Stance() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Stance_PROPERTIES)
}

Stance::Stance(
        const std::string & name,
        const std::string & icon,
        const std::string & description) :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent),
    m_Name(name),
    m_Icon(icon),
    m_Description(description)
{
    m_hasAutoControlled = false;
    m_hasSetBonus = false;
}

DL_DEFINE_ACCESS(Stance_PROPERTIES)

XmlLib::SaxContentElementInterface * Stance::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Stance_PROPERTIES)

    return subHandler;
}

void Stance::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Stance_PROPERTIES)
}

void Stance::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Stance_PROPERTIES)
    writer->EndElement();
}

bool Stance::VerifyObject(std::stringstream * ss) const
{
    bool ok = true;
    if (!ImageFileExists(IT_enhancement, Icon())
            && !ImageFileExists(IT_feat, Icon())
            && !ImageFileExists(IT_ui, Icon()))
    {
        (*ss) << "Stance " << m_Name << " is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    return ok;
}

bool Stance::operator==(const Stance & other) const
{
    return (m_Name == other.m_Name)
            && (m_Icon == other.m_Icon);
}

