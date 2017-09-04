// EnhancementTree.cpp
//
#include "StdAfx.h"
#include "EnhancementTree.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"
#include "Feat.h"

#define DL_ELEMENT EnhancementTree

namespace
{
    const wchar_t f_saxElementName[] = L"EnhancementTree";
    DL_DEFINE_NAMES(EnhancementTree_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

EnhancementTree::EnhancementTree() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(EnhancementTree_PROPERTIES)
}

DL_DEFINE_ACCESS(EnhancementTree_PROPERTIES)

XmlLib::SaxContentElementInterface * EnhancementTree::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(EnhancementTree_PROPERTIES)

    ASSERT(subHandler != NULL || wasFlag);
    return subHandler;
}

void EnhancementTree::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(EnhancementTree_PROPERTIES)
}

void EnhancementTree::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(EnhancementTree_PROPERTIES)
    writer->EndElement();
}

bool EnhancementTree::operator==(const EnhancementTree & other) const
{
    // only need to compare the name
    return (Name() == other.Name());
}

bool EnhancementTree::MeetRequirements(const Character & charData) const
{
    bool met = m_RequirementsToTrain.CanTrainTree(charData);
    return met;
}

void EnhancementTree::VerifyObject(
        const std::list<EnhancementTree> & trees,
        const std::list<Feat> & feats) const
{
    bool ok = true;
    std::stringstream ss;
    ss << "=====" << m_Name << "=====\n";
    if (!ImageFileExists(IT_ui, Icon()))
    {
        ss << "EnhancementTree is missing image file \"" << Icon() << "\"\n";
        ok = false;
    }
    // check this enhancement tree out for any inconsistencies
    std::list<EnhancementTreeItem>::const_iterator it = m_Items.begin();
    while (it != m_Items.end())
    {
        ok &= (*it).VerifyObject(&ss, trees, feats);
        ++it;
    }
    if (!ok)
    {
        ::OutputDebugString(ss.str().c_str());
    }
}

const EnhancementTreeItem * EnhancementTree::FindEnhancementItem(
        const std::string & enhancementName) const
{
    const EnhancementTreeItem * pItem = NULL;
    std::list<EnhancementTreeItem>::const_iterator it = m_Items.begin();
    while (it != m_Items.end())
    {
        if ((*it).InternalName() == enhancementName)
        {
            pItem = &(*it);
            break;      // found
        }
        ++it;
    }
    return pItem;
}

