// LevelTraining.h
//
// An XML object wrapper that holds information on an affect that a LevelTraining has.
#pragma once
#include "XmlLib\DLMacros.h"
#include "ClassTypes.h"
#include "FeatsListObject.h"
#include "TrainedSkill.h"

class LevelTraining :
    public XmlLib::SaxContentElement
{
    public:
        LevelTraining(void);
        void Write(XmlLib::SaxWriter * writer) const;

        void TrainFeat(const std::string & featName, TrainableFeatTypes type, size_t level);
        std::string RevokeFeat(TrainableFeatTypes type);
        void RevokeAllFeats(TrainableFeatTypes type);

        void TrainSkill(SkillType skill);
        void RevokeSkill(SkillType skill);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define LevelTraining_PROPERTIES(_) \
                DL_OPTIONAL_ENUM(_, ClassType, Class, Class_Unknown, classTypeMap) \
                DL_SIMPLE(_, size_t, SkillPointsAvailable, 0) \
                DL_SIMPLE(_, size_t, SkillPointsSpent, 0) \
                DL_OBJECT(_, FeatsListObject, AutomaticFeats) \
                DL_OBJECT(_, FeatsListObject, TrainedFeats) \
                DL_OBJECT_LIST(_, TrainedSkill, TrainedSkills)

        DL_DECLARE_ACCESS(LevelTraining_PROPERTIES)
        DL_DECLARE_VARIABLES(LevelTraining_PROPERTIES)

        friend class Character;
};
