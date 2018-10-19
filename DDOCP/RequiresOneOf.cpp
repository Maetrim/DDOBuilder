// RequiresOneOf.cpp
//
#include "StdAfx.h"
#include "RequiresOneOf.h"
#include "XmlLib\SaxWriter.h"
#include "Requirement.h"

#define DL_ELEMENT RequiresOneOf

namespace
{
    const wchar_t f_saxElementName[] = L"RequiresOneOf";
    DL_DEFINE_NAMES(RequiresOneOf_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

RequiresOneOf::RequiresOneOf() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(RequiresOneOf_PROPERTIES)
}

DL_DEFINE_ACCESS(RequiresOneOf_PROPERTIES)

XmlLib::SaxContentElementInterface * RequiresOneOf::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(RequiresOneOf_PROPERTIES)

    return subHandler;
}

void RequiresOneOf::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(RequiresOneOf_PROPERTIES)
}

void RequiresOneOf::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(RequiresOneOf_PROPERTIES)
    writer->EndElement();
}

bool RequiresOneOf::Met(
        const Character & charData, 
        const std::vector<size_t> & classLevels,
        size_t totalLevel,
        const std::list<TrainedFeat> & currentFeats,
        bool includeTomes) const
{
    // one or more of the requirements must be met
    bool canTrain = false;
    std::list<Requirement>::const_iterator it = m_Requirements.begin();
    while (it != m_Requirements.end())
    {
        canTrain |= (*it).Met(
                charData,
                classLevels,
                totalLevel,
                currentFeats,
                includeTomes);
        ++it;
    }
    return canTrain;
}

bool RequiresOneOf::CanTrainEnhancement(
        const Character & charData,
        size_t trainedRanks) const
{
    bool canTrain = false;
    std::list<Requirement>::const_iterator it = m_Requirements.begin();
    while (it != m_Requirements.end())
    {
        canTrain |= (*it).CanTrainEnhancement(
                charData,
                trainedRanks);
        ++it;
    }
    return canTrain;
}

bool RequiresOneOf::IsAllowed(
        const Character & charData,
        size_t trainedRanks) const
{
    bool canTrain = false;
    std::list<Requirement>::const_iterator it = m_Requirements.begin();
    while (it != m_Requirements.end())
    {
        canTrain |= (*it).IsAllowed(
                charData,
                trainedRanks);
        ++it;
    }
    return canTrain;
}

bool RequiresOneOf::CanTrainTree(
        const Character & charData) const
{
    // one or more of the requirements must be met
    bool canTrain = false;
    std::list<Requirement>::const_iterator it = m_Requirements.begin();
    while (it != m_Requirements.end())
    {
        canTrain |= (*it).CanTrainTree(charData);
        ++it;
    }
    return canTrain;
}

void RequiresOneOf::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met,
        size_t level) const
{
    std::list<Requirement>::const_iterator it = m_Requirements.begin();
    std::vector<CString> localRequirements;
    std::vector<bool> localMet;
    while (it != m_Requirements.end())
    {
        (*it).CreateRequirementStrings(charData, &localRequirements, &localMet, level);
        ++it;
    }
    if (HasDisplayDescription())
    {
        // if we have a display description, this replaces all the local strings with
        // a single entry which has a single met bool.
        // This is because some items can have so many inclusions that it would
        // take up several lines of screen space
        CString description = "Requires one of: ";
        description += m_DisplayDescription.c_str();
        bool wasMet = false;
        for (int i = 0; i < (int)localRequirements.size(); ++i)
        {
            wasMet |= localMet[i];
        }
        requirements->push_back(description);
        met->push_back(wasMet);        // these are required things
    }
    else
    {
        // determine any common header on all requirements. We do this because some
        // requirements may be a list of many almost identical items e.g.:
        // Improved Critical: Slashing, Improved Critical: Piercing,.... or Improved Critical: Ranged
        // this can produce a very long requirements line. It can be cut down to:
        // Improved Critical: Slashing, Piercing, ... or Ranged
        if (localRequirements.size() > 0
                && localRequirements[0].Find(':', 10) >= 0)
        {
            // looks like we may have a possible set of duplicate text
            int pos = localRequirements[0].Find(':', 10) + 2;
            CString dupText = localRequirements[0].Left(pos);
            bool isDuplicated = true;
            for (size_t i = 1; i < localRequirements.size(); ++i)
            {
                if (localRequirements[i].Left(pos) != dupText)
                {
                    isDuplicated = false;
                }
            }
            if (isDuplicated)
            {
                // we can remove the duplicate text from items 1...n but keep for item [0]
                for (size_t i = 1; i < localRequirements.size(); ++i)
                {
                    localRequirements[i] = localRequirements[i].Right(localRequirements[i].GetLength() - pos);
                }
            }
        }
        // re-package the local requirements to a single line
        if (localRequirements.size() > 0)
        {
            CString description = "Requires one of: ";
            bool wasMet = false;
            for (int i = 0; i < (int)localRequirements.size(); ++i)
            {
                localRequirements[i].Replace("Requires: ", "");    // take of leading text
                description += localRequirements[i];
                if (i <= (int)localRequirements.size() - 3)
                {
                    description += ", ";
                }
                else if (i == (int)localRequirements.size() - 2)
                {
                    description += " or ";
                }
                wasMet |= localMet[i];
            }
            requirements->push_back(description);
            met->push_back(wasMet);        // these are required things
        }
    }
}

bool RequiresOneOf::VerifyObject(
        std::stringstream * ss,
        const std::list<Feat> & allFeats) const
{
    bool ok = true;
    // check all the individual requirements
    std::list<Requirement>::const_iterator it = m_Requirements.begin();
    while (it != m_Requirements.end())
    {
        ok &= (*it).VerifyObject(ss, allFeats);
        ++it;
    }
    return ok;
}
