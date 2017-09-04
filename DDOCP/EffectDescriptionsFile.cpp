// EffectDescriptionsFile.cpp
//
#include "StdAfx.h"
#include "EffectDescriptionsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"EffectDescriptions";
}

EffectDescriptionsFile::EffectDescriptionsFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


EffectDescriptionsFile::~EffectDescriptionsFile(void)
{
}

bool EffectDescriptionsFile::Read()
{
    bool ok = false;
    try
    {
        // set up a reader with this as the expected root node
        XmlLib::SaxReader reader(this, f_saxElementName);
        // read in the xml from a file (fully qualified path)
        ok = reader.Open(m_filename);
    }
    catch (const std::exception & e)
    {
        ok = false;
        std::string errorMessage = e.what();
        AfxMessageBox(errorMessage.c_str(), MB_ICONERROR);
    }
    return ok;
}

XmlLib::SaxContentElementInterface * EffectDescriptionsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        EffectDescription item;
        if (item.SaxElementIsSelf(name, attributes))
        {
            m_loadedEffectDescriptions.push_back(item);
            subHandler = &(m_loadedEffectDescriptions.back());
        }
    }

    return subHandler;
}

void EffectDescriptionsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<EffectDescription> & EffectDescriptionsFile::EffectDescriptions() const
{
    return m_loadedEffectDescriptions;
}
