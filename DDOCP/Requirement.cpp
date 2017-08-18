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

bool Requirement::CanTrainFeat(
        const Character & charData, 
        const std::vector<size_t> & classLevels,
        size_t totalLevel,  // this is 0 based
        const std::list<TrainedFeat> & currentFeats) const
{
    bool canTrain = true;
    if (HasRace())
    {
        // must be a required race
        canTrain &= (charData.Race() == Race()
                || Race() == Race_All);
    }
    if (HasClass())
    {
        // must be a specific level/min level of a given class
        size_t classLevel = classLevels[Class()];
        if (HasMinLevel())
        {
            canTrain &= (classLevel >= MinLevel());
        }
        if (HasLevel())
        {
            canTrain &= (classLevel >= Level());
        }
    }
    else
    {
        // no class means the level field specifies a required level
        if (HasMinLevel())
        {
            // minimum overall level
            canTrain &= (totalLevel + 1 >= MinLevel());
        }
        if (HasLevel())
        {
            // specific level
            canTrain &= (totalLevel + 1 == Level());
        }
    }
    if (HasEnhancement())
    {
        // this enhancement must be trained
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "");
        canTrain &= (te != NULL);
    }

    if (HasSkill())
    {
        // must have this number of ranks in the skill at the current level to train
        canTrain &= (charData.SkillAtLevel(Skill(), totalLevel, true) >= Amount());
    }
    if (HasBAB())
    {
        // must have at least this BAB at the current total level to train
        canTrain &= (charData.BaseAttackBonus(totalLevel) >= BAB());
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
        canTrain = (count >= numNeeded);
    }
    if (HasAbility())
    {
        // must have this specific base ability value to train (Base + Tome + Level up only)
        ASSERT(HasAmount());
        size_t value= charData.AbilityAtLevel(Ability(), totalLevel);
        canTrain = (value >= Amount());
    }
    if (HasAlignment())
    {
        // must have a specific alignment
        canTrain &= (charData.Alignment() == m_Alignment);
    }
    return canTrain;
}

bool Requirement::CanTrainEnhancement(
        const Character & charData,
        size_t trainedRanks) const
{
    bool canTrain = true;
    if (HasRace())
    {
        // must be a required race
        canTrain &= (charData.Race() == Race()
                || Race() == Race_All);
    }
    if (HasClass())
    {
        // must be a specific level/min level of a given class
        std::vector<size_t> classLevels = charData.ClassLevels(MAX_LEVEL);
        size_t classLevel = classLevels[Class()];
        if (HasMinLevel())
        {
            canTrain &= (classLevel >= MinLevel());
        }
        if (HasLevel())
        {
            if (HasTier())
            {
                // only deny if truing to train the tier that this enhancement applies to
                if (trainedRanks +1 == Tier())
                {
                    canTrain &= (classLevel >= Level());
                }
                else
                {
                    canTrain = true;    // its a higher tier requirement, assume yes
                    // enhancement requirement becomes active once a tier has been trained
                }
            }
            else
            {
                canTrain &= (classLevel >= Level());
            }
        }
    }
    //?? this is not in the right place as it causes enhancements to be shown with a Red X
    if (HasEnhancement())
    {
        // this enhancement must be trained
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "");
        canTrain &= (te != NULL);
        if (te != NULL)
        {
            canTrain &= (trainedRanks < te->Ranks());
        }
    }

    if (HasSkill())
    {
        // must have this number of ranks in the skill at the current level to train
        canTrain &= (charData.SkillAtLevel(Skill(), MAX_LEVEL, true) >= Amount());
    }
    if (HasBAB())
    {
        // must have at least this BAB at the current total level to train
        canTrain &= (charData.BaseAttackBonus(MAX_LEVEL) >= BAB());
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
        canTrain = (count >= numNeeded);
    }
    if (HasAbility())
    {
        // must have this specific base ability value to train (Base + Tome + Level up only)
        ASSERT(HasAmount());
        size_t value= charData.AbilityAtLevel(Ability(), MAX_LEVEL);
        canTrain = (value >= Amount());
    }
    return canTrain;
}

bool Requirement::CanTrainTree(
        const Character & charData) const
{
    // when it comes to tree requirements, we only need to match either a race or a class
    bool canTrain = true;
    if (HasRace())
    {
        // must be a required race
        canTrain &= (charData.Race() == Race()
                || Race() == Race_All);
    }
    if (HasClass())
    {
        // must have a specific class present
        std::vector<size_t> classLevels = charData.ClassLevels(MAX_LEVEL);
        size_t classLevel = classLevels[Class()];
        canTrain = (classLevel > 0);
    }
    if (HasEnhancement())
    {
        // this enhancement must be trained to gain access to this tree
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "");
        canTrain &= (te != NULL);
    }
    return canTrain;
}

void Requirement::CreateRequirementStrings(
        const Character & charData,
        std::vector<CString> * requirements,
        std::vector<bool> * met) const
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
            if (HasTier())
            {
                extra.Format("(Tier %d)", Tier());
                description += extra;
            }
            met->push_back(classLevel >= MinLevel());
        }
        if (HasLevel())
        {
            CString extra;
            extra.Format("(%d)", Level());
            description += extra;
            if (HasTier())
            {
                extra.Format("(Tier %d)", Tier());
                description += extra;
            }
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
            met->push_back(true);   // we only ever consider this from character level 30
        }
        if (HasLevel())
        {
            // specific level
            description.Format("Requires: Level(%d)", Level());
            requirements->push_back(description);
            met->push_back(true);   // we only ever consider this from character level 30
        }
    }
    if (HasEnhancement())
    {
        // this enhancement must be trained
        const TrainedEnhancement * te = charData.IsTrained(
                Enhancement(),
                HasSelection() ? Selection() : "");
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
        met->push_back(charData.SkillAtLevel(Skill(), MAX_LEVEL, true) >= Amount());
    }
    if (HasBAB())
    {
        // must have at least this BAB at the current total level to train
        description.Format("Requires: BAB(%d)", BAB());
        requirements->push_back(description);
        met->push_back(charData.BaseAttackBonus(MAX_LEVEL) >= BAB());
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
        std::list<TrainedFeat> currentFeats = charData.CurrentFeats(MAX_LEVEL);
        met->push_back(TrainedCount(currentFeats, Feat()) > 0);
    }
    if (HasAbility())
    {
        // must have this specific base ability value to train (Base + Tome + Level up only)
        ASSERT(HasAmount());
        description.Format("Requires: %s(%d)", EnumEntryText(Ability(), abilityTypeMap), Amount());
        requirements->push_back(description);
        met->push_back(charData.AbilityAtLevel(Ability(), MAX_LEVEL) >= Amount());
    }
}
