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

    // some trees have had <InternName> updates.
    // make sure any loaded enhancements in those trees are updated
    if (m_Name == "War Soul"
            || m_Name == "Vistani")
    {
        // iterate the loaded trained elements and update if required
        std::list<EnhancementTreeItem>::iterator it = m_Items.begin();
        while (it != m_Items.end())
        {
            if ((*it).InternalName() == "WSCore1")
            {
                (*it).Set_InternalName("WarSoulCore1");
            }
            if ((*it).InternalName() == "WSCore2")
            {
                (*it).Set_InternalName("WarSoulCore2");
            }
            if ((*it).InternalName() == "WSCore3")
            {
                (*it).Set_InternalName("WarSoulCore3");
            }
            if ((*it).InternalName() == "WSCore4")
            {
                (*it).Set_InternalName("WarSoulCore4");
            }
            if ((*it).InternalName() == "WSCore5")
            {
                (*it).Set_InternalName("WarSoulCore5");
            }
            if ((*it).InternalName() == "WSCore6")
            {
                (*it).Set_InternalName("WarSoulCore6");
            }
            if ((*it).InternalName() == "WSAbilityI")
            {
                (*it).Set_InternalName("WarSoulAbilityI");
            }
            if ((*it).InternalName() == "WSAbilityII")
            {
                (*it).Set_InternalName("WarSoulAbilityII");
            }
            if ((*it).InternalName() == "BlessedBlades")
            {
                (*it).Set_InternalName("VistaniBlessedBlades");
            }
            ++it;
        }
    }
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
        std::map<std::string, int> * names,
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
        if (names->find((*it).InternalName()) != names->end())
        {
            ss << "EnhancementTree item " << (*it).InternalName() << " is a duplicate\n";
            ok = false;
        }
        else
        {
            // add this one to the map of used names
            (*names)[(*it).InternalName()] = 0;
        }
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

