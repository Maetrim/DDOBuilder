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

        void TrainFeat(
                const std::string & featName,
                TrainableFeatTypes type,
                size_t level,
                bool featSwapWarning);
        std::string RevokeFeat(TrainableFeatTypes type);
        void RevokeAllFeats(TrainableFeatTypes type);
        std::string FeatName(TrainableFeatTypes type) const;

        void TrainSkill(SkillType skill);
        void RevokeSkill(SkillType skill);

        // as not in macros we still need to support the standard macro access functions
        // that would have been generated for us.
        const FeatsListObject & AutomaticFeats() const;
        void Set_AutomaticFeats(const FeatsListObject & newValue);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define LevelTraining_PROPERTIES(_) \
                DL_OPTIONAL_ENUM(_, ClassType, Class, Class_Unknown, classTypeMap) \
                DL_SIMPLE(_, size_t, SkillPointsAvailable, 0) \
                DL_SIMPLE(_, size_t, SkillPointsSpent, 0) \
                DL_OBJECT(_, FeatsListObject, TrainedFeats) \
                DL_OBJECT_LIST(_, TrainedSkill, TrainedSkills)

        DL_DECLARE_ACCESS(LevelTraining_PROPERTIES)
        DL_DECLARE_VARIABLES(LevelTraining_PROPERTIES)

        // if this was included in the macros, they would be saved to the file,
        // we do not want these saved as they are regenerated automatically on load.
        FeatsListObject m_AutomaticFeats;

        friend class Character;
};
