// ActiveStances.cpp
//
#include "StdAfx.h"
#include "ActiveStances.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT ActiveStances

namespace
{
    const wchar_t f_saxElementName[] = L"ActiveStances";
    DL_DEFINE_NAMES(ActiveStances_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

ActiveStances::ActiveStances() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(ActiveStances_PROPERTIES)
}

DL_DEFINE_ACCESS(ActiveStances_PROPERTIES)

XmlLib::SaxContentElementInterface * ActiveStances::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(ActiveStances_PROPERTIES)

    return subHandler;
}

void ActiveStances::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(ActiveStances_PROPERTIES)
}

void ActiveStances::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(ActiveStances_PROPERTIES)
    writer->EndElement();
}

bool ActiveStances::IsStanceActive(const std::string & name) const
{
    bool isActive = false;
    std::list<std::string>::const_iterator it = m_Stances.begin();
    while (it != m_Stances.end())
    {
        if ((*it) == name)
        {
            isActive = true;
        }
        ++it;
    }
    return isActive;
}

void ActiveStances::AddActiveStance(const std::string & name)
{
    m_Stances.push_back(name);
}

void ActiveStances::RevokeStance(const std::string & name)
{
    // remove stance type name if present from the list
    std::list<std::string>::iterator it = m_Stances.begin();
    while (it != m_Stances.end())
    {
        if ((*it) == name)
        {
            // remove this one
            it = m_Stances.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
