// Requirements.cpp
//
#include "StdAfx.h"
#include "Requirements.h"
#include "XmlLib\SaxWriter.h"

#define DL_ELEMENT Requirements

namespace
{
    const wchar_t f_saxElementName[] = L"Requirements";
    DL_DEFINE_NAMES(Requirements_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Requirements::Requirements() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Requirements_PROPERTIES)
}

DL_DEFINE_ACCESS(Requirements_PROPERTIES)

XmlLib::SaxContentElementInterface * Requirements::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Requirements_PROPERTIES)

    return subHandler;
}

void Requirements::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Requirements_PROPERTIES)
}

void Requirements::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Requirements_PROPERTIES)
    writer->EndElement();
}

bool Requirements::Met(
        const Character & charData, 
        const std::vector<size_t> & classLevels,
        size_t totalLevel,
        const std::list<TrainedFeat> & currentFeats,
        bool includeTomes) const
{
    bool met = true;
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (met && it != m_Requires.end())
    {
        met = (*it).Met(charData, classLevels, totalLevel, currentFeats, includeTomes);
        ++it;
    }
    if (met
            && HasOneOf())
    {
        met = OneOf().Met(charData, classLevels, totalLevel, currentFeats, includeTomes);
    }
    if (met
            && HasNoneOf())
    {
        met = NoneOf().Met(charData, classLevels, totalLevel, currentFeats, includeTomes);
    }
    return met;
}

bool Requirements::CanTrainEnhancement(
        const Character & charData,
        size_t trainedRanks) const
{
    // only need to check any enhancement requirements
    bool met = true;
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (met && it != m_Requires.end())
    {
        met = (*it).CanTrainEnhancement(charData, trainedRanks);
        ++it;
    }
    if (met
            && HasOneOf())
    {
        met = OneOf().CanTrainEnhancement(charData, trainedRanks);
    }
    if (met
            && HasNoneOf())
    {
        met = NoneOf().CanTrainEnhancement(charData, trainedRanks);
    }
    return met;
}

bool Requirements::IsAllowed(
        const Character & charData,
        size_t trainedRanks) const
{
    // only need to check any enhancement requirements
    bool met = true;
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (met && it != m_Requires.end())
    {
        // subset, excluding enhancements
        met = (*it).IsAllowed(charData, trainedRanks);
        ++it;
    }
    if (met
            && HasOneOf())
    {
        // subset, excluding enhancements
        met = OneOf().IsAllowed(charData, trainedRanks);
    }
    if (met
            && HasNoneOf())
    {
        // all, including enhancements
        met = NoneOf().CanTrainEnhancement(charData, trainedRanks);
    }
    return met;
}

bool Requirements::CanTrainTree(
        const Character & charData) const
{
    bool met = true;
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (met && it != m_Requires.end())
    {
        met = (*it).CanTrainTree(charData);
        ++it;
    }
    if (met
            && HasOneOf())
    {
        met = OneOf().CanTrainTree(charData);
    }
    if (met
            && HasNoneOf())
    {
        met = NoneOf().CanTrainTree(charData);
    }
    return met;
}

void Requirements::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met,
        size_t level) const
{
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (it != m_Requires.end())
    {
        (*it).CreateRequirementStrings(charData, requirements, met, level);
        ++it;
    }
    if (HasOneOf())
    {
        OneOf().CreateRequirementStrings(charData, requirements, met, level);
    }
    if (HasNoneOf())
    {
        NoneOf().CreateRequirementStrings(charData, requirements, met, level);
    }
}

bool Requirements::RequiresEnhancement(const std::string& name) const
{
    bool bRequiresIt = false;
    // check all the individual requirements
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (it != m_Requires.end())
    {
        bRequiresIt |= (*it).RequiresEnhancement(name);
        ++it;
    }
    return bRequiresIt;
}

bool Requirements::VerifyObject(
        std::stringstream * ss,
        const std::list<EnhancementTree> & allTrees,
        const std::list<Feat> & allFeats) const
{
    bool ok = true;
    // check all the individual requirements
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (it != m_Requires.end())
    {
        ok &= (*it).VerifyObject(ss, allFeats);
        ++it;
    }
    if (HasOneOf())
    {
        ok &= m_OneOf.VerifyObject(ss, allFeats);
    }
    if (HasNoneOf())
    {
        ok &= m_NoneOf.VerifyObject(ss, allFeats);
    }
    return ok;
}

