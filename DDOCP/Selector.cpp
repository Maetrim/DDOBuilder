// Selector.cpp
//
#include "StdAfx.h"
#include "Selector.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Selector

namespace
{
    const wchar_t f_saxElementName[] = L"Selector";
    DL_DEFINE_NAMES(Selector_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Selector::Selector() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Selector_PROPERTIES)
}

DL_DEFINE_ACCESS(Selector_PROPERTIES)

XmlLib::SaxContentElementInterface * Selector::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Selector_PROPERTIES)

    return subHandler;
}

void Selector::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Selector_PROPERTIES)
}

void Selector::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Selector_PROPERTIES)
    writer->EndElement();
}

std::string Selector::SelectedIcon(const std::string & selectionName) const
{
    // iterate the selections and return the icon of the one that matches
    std::string icon = "NoImage";
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selectionName)
        {
            icon = (*it).Icon();
            break;
        }
        ++it;
    }
    return icon;
}

bool Selector::VerifyObject(
        std::stringstream * ss,
        const std::list<EnhancementTree> & trees,
        const std::list<Feat> & feats) const
{
    bool ok = true;
    if (HasExclusions())
    {
        ok &= m_Exclusions.VerifyObject(ss, trees, feats);
    }
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        ok &= (*it).VerifyObject(ss, trees, feats);
        ++it;
    }
    return ok;
}

std::string Selector::DisplayName(const std::string & selection) const
{
    std::string name;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            name = (*it).Name();
            break;          // done
        }
        ++it;
    }
    return name;
}

std::list<Effect> Selector::Effects(const std::string & selection) const
{
    std::list<Effect> effects;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            effects = (*it).Effects();
            break;          // done
        }
        ++it;
    }
    return effects;
}

std::list<Stance> Selector::Stances(const std::string & selection) const
{
    std::list<Stance> stances;
    std::list<EnhancementSelection>::const_iterator it = m_Selections.begin();
    while (it != m_Selections.end())
    {
        if ((*it).Name() == selection)
        {
            stances = (*it).Stances();
            break;          // done
        }
        ++it;
    }
    return stances;
}
