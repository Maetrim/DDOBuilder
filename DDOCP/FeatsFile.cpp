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
    // set up a reader with this as the expected root node
    XmlLib::SaxReader reader(this, f_saxElementName);
    // read in the xml from a file (fully qualified path)
    bool ok = reader.Open(m_filename);
    if (!ok)
    {
        std::string errorMessage = reader.ErrorMessage();
        // document has failed to load. Tell the user what we can about it
        CString text;
        text.Format("The document %s\n"
                "failed to load. The XML parser reported the following problem:\n"
                "\n", m_filename);
        text += errorMessage.c_str();
        AfxMessageBox(text, MB_ICONERROR);
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
