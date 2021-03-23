// IgnoredFeatsFile.cpp
//
#include "StdAfx.h"
#include "IgnoredFeatsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"IgnoredFeats"; // root element name to look for
}

IgnoredFeatsFile::IgnoredFeatsFile(const std::string & filename) :
    SaxContentElement(f_saxElementName),
    m_filename(filename),
    m_loadTotal(0)
{
}


IgnoredFeatsFile::~IgnoredFeatsFile(void)
{
}

void IgnoredFeatsFile::Read()
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

XmlLib::SaxContentElementInterface * IgnoredFeatsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        std::string featName;
        if (name == L"Feat")
        {
            m_loadedFeats.push_back(featName);
            subHandler = HandleSimpleElement(&(m_loadedFeats.back()));
        }
    }

    return subHandler;
}

void IgnoredFeatsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<std::string> & IgnoredFeatsFile::IgnoredFeats()
{
    return m_loadedFeats;
}

void IgnoredFeatsFile::Save(std::list<std::string> & featList)
{
    // load all the global data required by the program
    // all data files are in the same directory as the executable
    char fullPath[MAX_PATH];
    ::GetModuleFileName(NULL, fullPath, MAX_PATH);

    char drive[_MAX_DRIVE];
    char folder[_MAX_PATH];
    _splitpath_s(fullPath, drive, _MAX_DRIVE, folder, _MAX_PATH, NULL, 0, NULL, 0);

    char path[_MAX_PATH];
    _makepath_s(path, _MAX_PATH, drive, folder, NULL, NULL);
    std::string filename = path;
    filename += "FeatIgnoreList.xml";

    try
    {
        XmlLib::SaxWriter writer;
        writer.Open(filename);
        writer.StartDocument(f_saxElementName);
        std::list<std::string>::const_iterator it = featList.begin();
        while (it != featList.end())
        {
            writer.WriteSimpleElement(L"Feat", (*it));
            ++it;
        }
        writer.EndDocument();
    }
    catch (const std::exception & e)
    {
        std::string errorMessage = e.what();
        // document has failed to save. Tell the user what we can about it
        CString text;
        text.Format("The document %s\n"
                "failed to save. The XML parser reported the following problem:\n"
                "\n", filename.c_str());
        text += errorMessage.c_str();
        AfxMessageBox(text, MB_ICONERROR);
    }
}

