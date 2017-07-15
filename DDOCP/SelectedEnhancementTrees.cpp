// SelectedEnhancementTrees.cpp
//
#include "StdAfx.h"
#include "SelectedEnhancementTrees.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT SelectedEnhancementTrees

namespace
{
    const wchar_t f_saxElementName[] = L"SelectedEnhancementTrees";
    DL_DEFINE_NAMES(SelectedEnhancementTrees_PROPERTIES)

    const unsigned f_verCurrent = 1;
    const std::string f_noSelection = "No selection";
}

SelectedEnhancementTrees::SelectedEnhancementTrees() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(SelectedEnhancementTrees_PROPERTIES)
    m_TreeName.resize(6, f_noSelection);
}

DL_DEFINE_ACCESS(SelectedEnhancementTrees_PROPERTIES)

XmlLib::SaxContentElementInterface * SelectedEnhancementTrees::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(SelectedEnhancementTrees_PROPERTIES)

    return subHandler;
}

void SelectedEnhancementTrees::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(SelectedEnhancementTrees_PROPERTIES)
    // any load operation will have appended the loaded values
    // onto the m_TreeName object, reduce it down to the last 6 items
    while (m_TreeName.size() > 6)
    {
        m_TreeName.erase(m_TreeName.begin());
    }
}

void SelectedEnhancementTrees::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(SelectedEnhancementTrees_PROPERTIES)
    writer->EndElement();
}

bool SelectedEnhancementTrees::IsTreePresent(const std::string & treeName) const
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

const std::string & SelectedEnhancementTrees::Tree(size_t index) const
{
    ASSERT(index >= 0 && index <= m_TreeName.size());
    return m_TreeName[index];
}

void SelectedEnhancementTrees::SetTree(size_t index, const std::string & treeName)
{
    ASSERT(index >= 0 && index <= m_TreeName.size());
    m_TreeName[index] = treeName;
}

void SelectedEnhancementTrees::SetNotSelected(size_t index)
{
    ASSERT(index >= 0 && index <= m_TreeName.size());
    m_TreeName[index] = f_noSelection;
}

bool SelectedEnhancementTrees::IsNotSelected(const std::string & treeName)
{
    return (treeName == f_noSelection);
}

