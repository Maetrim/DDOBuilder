// SetBonusFile.cpp
//
#include "StdAfx.h"
#include "SetBonusFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"SetBonuses"; // root element name to look for
}

SetBonusFile::SetBonusFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


SetBonusFile::~SetBonusFile(void)
{
}

void SetBonusFile::Read()
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

XmlLib::SaxContentElementInterface * SetBonusFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        SetBonus set;
        if (set.SaxElementIsSelf(name, attributes))
        {
            m_loadedSetBonuses.push_back(set);
            subHandler = &(m_loadedSetBonuses.back());
        }
    }

    return subHandler;
}

void SetBonusFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<SetBonus> & SetBonusFile::Sets() const
{
    return m_loadedSetBonuses;
}
