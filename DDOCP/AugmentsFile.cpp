// AugmentsFile.cpp
//
#include "StdAfx.h"
#include "AugmentsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"Augments"; // root element name to look for
}

AugmentsFile::AugmentsFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


AugmentsFile::~AugmentsFile(void)
{
}

void AugmentsFile::Read()
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

XmlLib::SaxContentElementInterface * AugmentsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        Augment augment;
        if (augment.SaxElementIsSelf(name, attributes))
        {
            m_loadedAugments.push_back(augment);
            subHandler = &(m_loadedAugments.back());
        }
    }

    return subHandler;
}

void AugmentsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<Augment> & AugmentsFile::Augments() const
{
    return m_loadedAugments;
}
