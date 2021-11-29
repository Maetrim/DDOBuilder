// SelectedDestinyTrees.cpp
//
#include "StdAfx.h"
#include "SelectedDestinyTrees.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT SelectedDestinyTrees

namespace
{
    const wchar_t f_saxElementName[] = L"SelectedDestinyTrees";
    DL_DEFINE_NAMES(SelectedDestinyTrees_PROPERTIES)

    const unsigned f_verCurrent = 1;
    const std::string f_noSelection = "No selection";
}

SelectedDestinyTrees::SelectedDestinyTrees() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SelectedDestinyTrees_PROPERTIES)
        // +1 as we have a preview tree
    m_TreeName.resize(MAX_EPIC_DESTINY_TREES+1, f_noSelection);
}

DL_DEFINE_ACCESS(SelectedDestinyTrees_PROPERTIES)

XmlLib::SaxContentElementInterface * SelectedDestinyTrees::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SelectedDestinyTrees_PROPERTIES)

    return subHandler;
}

void SelectedDestinyTrees::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SelectedDestinyTrees_PROPERTIES)
    // any load operation will have appended the loaded values
    // onto the m_TreeName object
    while (m_TreeName.size() > MAX_EPIC_DESTINY_TREES + 1
            || m_TreeName.front() == f_noSelection)
    {
        m_TreeName.erase(m_TreeName.begin());
    }
    // ensure we have the right number of elements after load
    m_TreeName.resize(MAX_EPIC_DESTINY_TREES + 1, f_noSelection);
}

void SelectedDestinyTrees::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SelectedDestinyTrees_PROPERTIES)
    writer->EndElement();
}

bool SelectedDestinyTrees::IsTreePresent(const std::string & treeName) const
{
    // return true if the tree is present
    bool present = false;
    for (size_t i = 0; i < m_TreeName.size(); ++i)
    {
        if (m_TreeName[i] == treeName)
        {
            present = true;
        }
    }
    return present;
}

const std::string & SelectedDestinyTrees::Tree(size_t index) const
{
    ASSERT(index >= 0 && index <= m_TreeName.size());
    return m_TreeName[index];
}

void SelectedDestinyTrees::SetTree(size_t index, const std::string & treeName)
{
    ASSERT(index >= 0 && index <= m_TreeName.size());
    m_TreeName[index] = treeName;
}

void SelectedDestinyTrees::SetNotSelected(size_t index)
{
    ASSERT(index >= 0 && index <= m_TreeName.size());
    m_TreeName[index] = f_noSelection;
}

bool SelectedDestinyTrees::IsNotSelected(const std::string & treeName)
{
    return (treeName == f_noSelection);
}

void SelectedDestinyTrees::SwapTrees(
        const std::string & tree1,
        const std::string & tree2)
{
    bool swapped1 = false;
    bool swapped2 = false;
    for (size_t i = 0; i < m_TreeName.size(); ++i)
    {
        if (!swapped1
                && m_TreeName[i] == tree1)
        {
            swapped1 = true;
            m_TreeName[i] = tree2;
        }
        else if (!swapped2
                && m_TreeName[i] == tree2)
        {
            swapped2 = true;
            m_TreeName[i] = tree1;
        }
    }
}

