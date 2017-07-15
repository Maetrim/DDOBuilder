// FeatsFile.cpp
//
#include "StdAfx.h"
#include "FeatsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"Feats"; // root element name to look for
}

FeatsFile::FeatsFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


FeatsFile::~FeatsFile(void)
{
}

void FeatsFile::Read()
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
}

XmlLib::SaxContentElementInterface * FeatsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        Feat feat;
        if (feat.SaxElementIsSelf(name, attributes))
        {
            m_loadedFeats.push_back(feat);
            subHandler = &(m_loadedFeats.back());
        }
    }

    return subHandler;
}

void FeatsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<Feat> & FeatsFile::Feats() const
{
    return m_loadedFeats;
}
