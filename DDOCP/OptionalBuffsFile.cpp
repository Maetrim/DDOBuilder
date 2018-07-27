// OptionalBuffsFile.cpp
//
#include "StdAfx.h"
#include "OptionalBuffsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"SelfAndPartyBuffs";
}

OptionalBuffsFile::OptionalBuffsFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


OptionalBuffsFile::~OptionalBuffsFile(void)
{
}

bool OptionalBuffsFile::Read()
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
    return ok;
}

XmlLib::SaxContentElementInterface * OptionalBuffsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        OptionalBuff item;
        if (item.SaxElementIsSelf(name, attributes))
        {
            m_loadedBuffs.push_back(item);
            subHandler = &(m_loadedBuffs.back());
        }
    }

    return subHandler;
}

void OptionalBuffsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<OptionalBuff> & OptionalBuffsFile::OptionalBuffs() const
{
    return m_loadedBuffs;
}
