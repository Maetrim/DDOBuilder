// ItemsFile.cpp
//
#include "StdAfx.h"
#include "ItemsFile.h"
#include "XmlLib\SaxReader.h"

namespace
{
    const XmlLib::SaxString f_saxElementName = L"Items";
}

ItemsFile::ItemsFile(const std::string & path) :
    SaxContentElement(f_saxElementName),
    m_path(path),
    m_loadTotal(0)
{
}


ItemsFile::~ItemsFile(void)
{
}

void ItemsFile::ReadFiles()
{
    std::string fileFilter = m_path;
    fileFilter += "*.item";
    // read all the item files found in the Items sub-directory
    // first enumerate each file and load it
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(fileFilter.c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        ReadFile(findFileData.cFileName);
        while (FindNextFile(hFind, &findFileData))
        {
            ReadFile(findFileData.cFileName);
        }
        FindClose(hFind);
    }
}

bool ItemsFile::ReadFile(const std::string & filename)
{
    bool ok = false;
    std::string fullFilename = m_path + filename;
    // set up a reader with this as the expected root node
    XmlLib::SaxReader reader(this, f_saxElementName);
    // read in the xml from a file (fully qualified path)
    ok = reader.Open(fullFilename);
    if (!ok)
    {
        ok = false;
        std::string errorMessage = reader.ErrorMessage();
        AfxMessageBox(errorMessage.c_str(), MB_ICONERROR);
        ::OutputDebugString("Bad item file: ");
        ::OutputDebugString(fullFilename.c_str());
        ::OutputDebugString("\n");
    }
    return ok;
}

XmlLib::SaxContentElementInterface * ItemsFile::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        Item item;
        if (item.SaxElementIsSelf(name, attributes))
        {
            m_loadedItems.push_back(item);
            subHandler = &(m_loadedItems.back());
        }
    }

    return subHandler;
}

void ItemsFile::EndElement()
{
    SaxContentElement::EndElement();
}

const std::list<Item> & ItemsFile::Items() const
{
    return m_loadedItems;
}
