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

bool Requirements::CanTrainFeat(
        const Character & charData, 
        const std::vector<size_t> & classLevels,
        size_t totalLevel,
        const std::list<TrainedFeat> & currentFeats,
        bool includeTomes) const
{
    bool canTrain = true;
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (canTrain && it != m_Requires.end())
    {
        canTrain = (*it).CanTrainFeat(charData, classLevels, totalLevel, currentFeats, includeTomes);
        ++it;
    }
    if (canTrain
            && HasOneOf())
    {
        canTrain = OneOf().CanTrainFeat(charData, classLevels, totalLevel, currentFeats, includeTomes);
    }
    if (canTrain
            && HasNoneOf())
    {
        canTrain = NoneOf().CanTrainFeat(charData, classLevels, totalLevel, currentFeats, includeTomes);
    }
    return canTrain;
}

bool Requirements::CanTrainEnhancement(
        const Character & charData,
        size_t trainedRanks) const
{
    // only need to check any enhancement requirements
    bool canTrain = true;
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (canTrain && it != m_Requires.end())
    {
        canTrain = (*it).CanTrainEnhancement(charData, trainedRanks);
        ++it;
    }
    if (canTrain
            && HasOneOf())
    {
        canTrain = OneOf().CanTrainEnhancement(charData, trainedRanks);
    }
    if (canTrain
            && HasNoneOf())
    {
        canTrain = NoneOf().CanTrainEnhancement(charData, trainedRanks);
    }
    return canTrain;
}

bool Requirements::CanTrainTree(
        const Character & charData) const
{
    bool canTrain = true;
    std::list<Requirement>::const_iterator it = m_Requires.begin();
    while (canTrain && it != m_Requires.end())
    {
        canTrain = (*it).CanTrainTree(charData);
        ++it;
    }
    if (canTrain
            && HasOneOf())
    {
        canTrain = OneOf().CanTrainTree(charData);
    }
    if (canTrain
            && HasNoneOf())
    {
        canTrain = NoneOf().CanTrainTree(charData);
    }
    return canTrain;
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

