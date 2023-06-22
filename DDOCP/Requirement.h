// Requirement.h
//
#pragma once
#include "XmlLib\DLMacros.h"

#include "AbilityTypes.h"
#include "AlignmentTypes.h"
#include "ClassTypes.h"
#include "RaceTypes.h"
#include "SkillTypes.h"

class Feat;
class Character;
class TrainedFeat;

class Requirement :
    public XmlLib::SaxContentElement
{
    public:
        Requirement(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool Met(
                const Character & charData,
                const std::vector<size_t> & classLevels,
                size_t totalLevel,
                const std::list<TrainedFeat> & currentFeats,
                bool includeTomes) const;
        bool CanTrainEnhancement(
                const Character & charData,
                size_t trainedRanks) const;
        bool IsAllowed(
                const Character & charData,
                size_t trainedRanks) const;
        bool CanTrainTree(
                const Character & charData) const;
        void CreateRequirementStrings(
                const Character & charData,
                std::vector<CString> * requirements,
                std::vector<bool> * met,
                size_t level) const;

        bool RequiresEnhancement(const std::string& name, const std::string& selection) const;

        bool VerifyObject(
                std::stringstream * ss,
                const std::list<Feat> & allFeats) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Requirement_PROPERTIES(_) \
                DL_OPTIONAL_ENUM(_, RaceType, Race, Race_Unknown, raceTypeMap) \
                DL_OPTIONAL_ENUM(_, ClassType, Class, Class_Unknown, classTypeMap) \
                DL_OPTIONAL_ENUM(_, AbilityType, Ability, Ability_Unknown, abilityTypeMap) \
                DL_OPTIONAL_ENUM(_, SkillType, Skill, Skill_Unknown, skillTypeMap) \
                DL_OPTIONAL_ENUM(_, AlignmentType, Alignment, Alignment_Unknown, alignmentTypeMap) \
                DL_OPTIONAL_SIMPLE(_, size_t, BAB, 0) \
                DL_OPTIONAL_SIMPLE(_, size_t, Level, 0) \
                DL_OPTIONAL_SIMPLE(_, size_t, MinLevel, 0) \
                DL_OPTIONAL_SIMPLE(_, size_t, Amount, 0) \
                DL_OPTIONAL_STRING(_, Enhancement) \
                DL_OPTIONAL_STRING(_, Selection) \
                DL_OPTIONAL_STRING(_, Feat) \
                DL_OPTIONAL_STRING(_, FeatAnySource) \
                DL_FLAG(_, ListAtSameFeatLevel)

        DL_DECLARE_ACCESS(Requirement_PROPERTIES)
        DL_DECLARE_VARIABLES(Requirement_PROPERTIES)

        friend class CEnhancementEditorDialog;
};
