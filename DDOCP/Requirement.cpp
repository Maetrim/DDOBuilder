// Requirement.cpp
//
#include "StdAfx.h"
#include "Requirement.h"
#include "XmlLib\SaxWriter.h"
#include "Character.h"
#include "Feat.h"
#include "GlobalSupportFunctions.h"
#include "TrainedFeat.h"

#define DL_ELEMENT Requirement

namespace
{
    const wchar_t f_saxElementName[] = L"Requirement";
    DL_DEFINE_NAMES(Requirement_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Requirement::Requirement() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Requirement_PROPERTIES)
}

DL_DEFINE_ACCESS(Requirement_PROPERTIES)

XmlLib::SaxContentElementInterface * Requirement::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Requirement_PROPERTIES)

    return subHandler;
}

void Requirement::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Requirement_PROPERTIES)
}

void Requirement::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Requirement_PROPERTIES)
    writer->EndElement();
}

bool Requirement::VerifyObject(
        std::stringstream * ss,
        const std::list<::Feat> & allFeats) const // compiler fails without additional :: for some reason
{
    bool ok = true;
    if (HasFeat())
    {
        ok = (FindFeat(m_Feat).Name() == m_Feat);
        if (!ok)
        {
            (*ss) << "Requirement referenced unknown feat \"" << m_Feat << "\"\n";
        }
    }
    return ok;
}

bool Requirement::Met(
        const Character & charData, 
        const std::vector<size_t> & classLevels,
        size_t totalLevel,  // this is 0 based
        const std::list<TrainedFeat> & currentFeats,
        bool includeTomes) const
{
    bool met = true;
    if (HasRace())
    {
        // must be a required race
        met &= (charData.Race() == Race()
                || Race() == Race_All);
    }
    if (HasClass())
    {
        // must be a specific level/min level of a given class
        size_t classLevel = classLevels[Class()];
        if (HasMinLevel())
        {
            met &= (classLevel >= MinLevel());
        }
        if (HasLevel())
        {
            met &= (classLevel >= Level());
        }
    }
    else
    {
        // no class means the level field specifies a required level
        if (HasMinLevel())
        {
            // minimum overall level
            met &= (totalLevel + 1 >= MinLevel());
        }
        if (HasLevel())
        {
            // specific level
            met &= (totalLevel + 1 == Level());
        }
    }
    if (HasEnhancement())
    {
        // this enhancement must be trained
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "",
                TT_unknown);    // just check them all
        met &= (te != NULL);
    }

    if (HasSkill())
    {
        // must have this number of ranks in the skill at the current level to train
        met &= (charData.SkillAtLevel(Skill(), totalLevel, includeTomes) >= Amount());
    }
    if (HasBAB())
    {
        // must have at least this BAB at the current total level to train
        met &= (charData.BaseAttackBonus(totalLevel) >= BAB());
    }
    if (HasFeat())
    {
        // must have this feat previously trained at the current level to train
        size_t count = TrainedCount(currentFeats, Feat());
        size_t numNeeded = 1;
        if (HasAmount())
        {
            numNeeded = Amount();
        }
        met = (count >= numNeeded);
        // this feature is only used when a feat needs to be listed when it is
        // excluded by another feat trained at the same level
        if (met && HasListAtSameFeatLevel())
        {
            // this can override if the feat is trained at the current level
            // find it in the trained feat list and check its level against the current
            std::list<TrainedFeat>::const_iterator tfit = currentFeats.begin();
            while (tfit != currentFeats.end())
            {
                if ((*tfit).FeatName() == Feat())
                {
                    break;
                }
                ++tfit;
            }
            if (tfit != currentFeats.end())
            {
                if ((*tfit).LevelTrainedAt() == totalLevel)
                {
                    met = false;
                }
            }
        }
    }
    if (HasAbility())
    {
        // must have this specific base ability value to train (Base + Tome + Level up only)
        ASSERT(HasAmount());
        size_t value = charData.AbilityAtLevel(Ability(), totalLevel, includeTomes);
        met = (value >= Amount());
    }
    if (HasAlignment())
    {
        // must have a specific alignment
        met &= (charData.Alignment() == m_Alignment);
    }
    return met;
}

bool Requirement::CanTrainEnhancement(
        const Character & charData,
        size_t trainedRanks) const
{
    bool met = true;
    if (HasRace())
    {
        // must be a required race
        met &= (charData.Race() == Race()
                || Race() == Race_All);
    }
    if (HasClass())
    {
        // must be a specific level/min level of a given class
        std::vector<size_t> classLevels = charData.ClassLevels(MAX_LEVEL);
        size_t classLevel = classLevels[Class()];
        if (HasMinLevel())
        {
            met &= (classLevel >= MinLevel());
        }
        if (HasLevel())
        {
            met &= (classLevel >= Level());
        }
    }
    if (HasEnhancement())
    {
        // this enhancement must be trained
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "",
                TT_unknown);
        met &= (te != NULL);
        if (te != NULL)
        {
            const EnhancementTreeItem * pItem = FindEnhancement(Enhancement());
            if (pItem->Ranks() == 1)
            {
                // if the source enhancement only has a single rank, then
                // allow this one to be trained to its max ranks
            }
            else
            {
                // we must have the same of less ranks than the item in question
                met &= (trainedRanks < te->Ranks());
            }
        }
    }

    if (HasSkill())
    {
        // must have this number of ranks in the skill at the current level to train
        met &= (charData.SkillAtLevel(Skill(), MAX_LEVEL, true) >= Amount());
    }
    if (HasBAB())
    {
        // must have at least this BAB at the current total level to train
        met &= (charData.BaseAttackBonus(MAX_LEVEL) >= BAB());
    }
    if (HasFeat())
    {
        // must have this feat previously trained at the current level to train
        std::list<TrainedFeat> feats = charData.CurrentFeats(MAX_LEVEL);
        size_t count = TrainedCount(feats, Feat());
        size_t numNeeded = 1;
        if (HasAmount())
        {
            numNeeded = Amount();
        }
        met = (count >= numNeeded);
    }
    if (HasAbility())
    {
        // must have this specific base ability value to train (Base + Tome + Level up only)
        ASSERT(HasAmount());
        size_t value = charData.AbilityAtLevel(Ability(), MAX_LEVEL, true);
        met = (value >= Amount());
    }
    return met;
}

bool Requirement::IsAllowed(
        const Character & charData,
        size_t trainedRanks) const
{
    bool met = true;
    if (HasRace())
    {
        // must be a required race
        met &= (charData.Race() == Race()
                || Race() == Race_All);
    }
    if (HasClass())
    {
        // must be a specific level/min level of a given class
        std::vector<size_t> classLevels = charData.ClassLevels(MAX_LEVEL);
        size_t classLevel = classLevels[Class()];
        if (HasMinLevel())
        {
            met &= (classLevel >= MinLevel());
        }
        if (HasLevel())
        {
            met &= (classLevel >= Level());
        }
    }

    if (HasSkill())
    {
        // must have this number of ranks in the skill at the current level to train
        met &= (charData.SkillAtLevel(Skill(), MAX_LEVEL, true) >= Amount());
    }
    if (HasBAB())
    {
        // must have at least this BAB at the current total level to train
        met &= (charData.BaseAttackBonus(MAX_LEVEL) >= BAB());
    }
    if (HasFeat())
    {
        // must have this feat previously trained at the current level to train
        std::list<TrainedFeat> feats = charData.CurrentFeats(MAX_LEVEL);
        size_t count = TrainedCount(feats, Feat());
        size_t numNeeded = 1;
        if (HasAmount())
        {
            numNeeded = Amount();
        }
        met = (count >= numNeeded);
    }
    if (HasAbility())
    {
        // must have this specific base ability value to train (Base + Tome + Level up only)
        ASSERT(HasAmount());
        size_t value = charData.AbilityAtLevel(Ability(), MAX_LEVEL, true);
        met = (value >= Amount());
    }
    return met;
}

bool Requirement::CanTrainTree(
        const Character & charData) const
{
    // when it comes to tree requirements, we only need to match either a race or a class
    bool met = true;
    if (HasRace())
    {
        // must be a required race
        met &= (charData.Race() == Race()
                || Race() == Race_All);
    }
    if (HasClass())
    {
        // must have a specific class present
        std::vector<size_t> classLevels = charData.ClassLevels(MAX_LEVEL);
        size_t classLevel = classLevels[Class()];
        met = (classLevel > 0);
    }
    if (HasEnhancement())
    {
        // this enhancement must be trained to gain access to this tree
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "",
                TT_unknown);
        met &= (te != NULL);
    }
    if (HasFeat())
    {
        // must have this feat previously trained to access this tree
        std::list<TrainedFeat> feats = charData.CurrentFeats(MAX_LEVEL);
        size_t count = TrainedCount(feats, Feat());
        size_t numNeeded = 1;
        if (HasAmount())
        {
            numNeeded = Amount();
        }
        met = (count >= numNeeded);
    }
    return met;
}

void Requirement::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met,
        size_t level) const
{
    CString description;
    bool doneMinLevel = false;
    if (HasRace())
    {
        // must be a required race
        description = "Requires: ";
        description += EnumEntryText(Race(), raceTypeMap);
        if (HasMinLevel())
        {
            // append the required min level
            CString minLevel;
            minLevel.Format("(%d)", MinLevel());
            description += minLevel;
            doneMinLevel = true;
        }
        requirements->push_back(description);
        met->push_back(charData.Race() == Race());
    }
    if (HasClass())
    {
        // must be a specific level/min level of a given class
        description = "Requires: ";
        description += EnumEntryText(Class(), classTypeMap);
        std::vector<size_t> classLevels = charData.ClassLevels(MAX_LEVEL);
        size_t classLevel = classLevels[Class()];
        if (HasMinLevel())
        {
            CString extra;
            extra.Format("(%d)", MinLevel());
            description += extra;
            met->push_back(classLevel >= MinLevel());
        }
        if (HasLevel())
        {
            CString extra;
            extra.Format("(%d)", Level());
            description += extra;
            met->push_back(classLevel >= Level());
        }
        requirements->push_back(description);
    }
    else
    {
        // no class means the level field specifies a required level
        if (HasMinLevel() && !doneMinLevel)
        {
            // minimum overall level
            description.Format("Requires: Minimum level(%d)", MinLevel());
            requirements->push_back(description);
            met->push_back(level + 1 >= MinLevel());
        }
        if (HasLevel())
        {
            // specific level
            description.Format("Requires: Level(%d)", Level());
            requirements->push_back(description);
            met->push_back(level + 1 >= MinLevel());
        }
    }
    if (HasEnhancement())
    {
        // this enhancement must be trained
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "",
                TT_unknown);
        std::string name = "Unknown enhancement";
        const EnhancementTreeItem * item = FindEnhancement(Enhancement());
        if (item != NULL)
        {
            name = item->Name();
        }
        if (HasSelection())
        {
            description.Format(
                    "Requires: %s(%s)",
                    name.c_str(),
                    Selection().c_str());
        }
        else
        {
            description.Format(
                    "Requires: %s",
                    name.c_str());
        }
        requirements->push_back(description);
        met->push_back(te != NULL);
    }

    if (HasSkill())
    {
        // must have this number of ranks in the skill at the current level to train
        description.Format("Requires: %s(%d)", EnumEntryText(Skill(), skillTypeMap), Amount());
        requirements->push_back(description);
        met->push_back(charData.SkillAtLevel(Skill(), level, true) >= Amount());
    }
    if (HasBAB())
    {
        // must have at least this BAB at the current total level to train
        description.Format("Requires: BAB(%d)", BAB());
        requirements->push_back(description);
        met->push_back(charData.BaseAttackBonus(level) >= BAB());
    }
    if (HasFeat())
    {
        // must have this feat previously trained at the current level to train
        if (HasAmount())
        {
            description.Format("Requires: %s(%d)", Feat().c_str(), Amount());
        }
        else
        {
            description.Format("Requires: %s", Feat().c_str());
        }
        requirements->push_back(description);
        std::list<TrainedFeat> currentFeats = charData.CurrentFeats(level);
        met->push_back(TrainedCount(currentFeats, Feat()) > 0);
    }
    if (HasAbility())
    {
        // must have this specific base ability value to train (Base + Tome + Level up only)
        ASSERT(HasAmount());
        description.Format("Requires: %s(%d)", EnumEntryText(Ability(), abilityTypeMap), Amount());
        requirements->push_back(description);
        met->push_back(charData.AbilityAtLevel(Ability(), level, true) >= Amount());
    }
}
