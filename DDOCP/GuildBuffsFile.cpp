// GuildBuffsFile.cpp
//
#include "StdAfx.h"
#include "GuildBuffsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"GuildBuffs";
}

GuildBuffsFile::GuildBuffsFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


GuildBuffsFile::~GuildBuffsFile(void)
{
}

bool GuildBuffsFile::Read()
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

XmlLib::SaxContentElementInterface * GuildBuffsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        GuildBuff item;
        if (item.SaxElementIsSelf(name, attributes))
        {
            m_loadedGuildBuffs.push_back(item);
            subHandler = &(m_loadedGuildBuffs.back());
        }
    }

    return subHandler;
}

void GuildBuffsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<GuildBuff> & GuildBuffsFile::GuildBuffs() const
{
    return m_loadedGuildBuffs;
}
