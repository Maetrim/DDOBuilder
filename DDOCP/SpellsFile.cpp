// SpellsFile.cpp
//
#include "StdAfx.h"
#include "SpellsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"Spells"; // root element name to look for
}

SpellsFile::SpellsFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


SpellsFile::~SpellsFile(void)
{
}

void SpellsFile::Read()
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

XmlLib::SaxContentElementInterface * SpellsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        Spell spell;
        if (spell.SaxElementIsSelf(name, attributes))
        {
            m_loadedSpells.push_back(spell);
            subHandler = &(m_loadedSpells.back());
        }
    }

    return subHandler;
}

void SpellsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<Spell> & SpellsFile::Spells() const
{
    return m_loadedSpells;
}
