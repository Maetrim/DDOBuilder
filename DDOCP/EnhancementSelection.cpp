// EnhancementSelection.cpp
//
#include "StdAfx.h"
#include "EnhancementSelection.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT EnhancementSelection

namespace
{
    const wchar_t f_saxElementName[] = L"EnhancementSelection";
    DL_DEFINE_NAMES(EnhancementSelection_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EnhancementSelection::EnhancementSelection() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(EnhancementSelection_PROPERTIES)
}

DL_DEFINE_ACCESS(EnhancementSelection_PROPERTIES)

XmlLib::SaxContentElementInterface * EnhancementSelection::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EnhancementSelection_PROPERTIES)

    return subHandler;
}

void EnhancementSelection::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EnhancementSelection_PROPERTIES)
}

void EnhancementSelection::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EnhancementSelection_PROPERTIES)
    writer->EndElement();
}

void EnhancementSelection::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met) const
{
    m_RequirementsToTrain.CreateRequirementStrings(charData, requirements, met);
}

bool EnhancementSelection::VerifyObject(
        std::stringstream * ss,
        const std::list<EnhancementTree> & trees,
        const std::list<Feat> & feats) const
{
    bool ok = true;
    if (!ImageFileExists(IT_enhancement, Icon()))
    {
        (*ss) << "EnhancementSelection is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    return ok;
}
