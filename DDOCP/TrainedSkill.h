// TrainedSkill.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "SkillTypes.h"

class TrainedSkill :
    public XmlLib::SaxContentElement
{
    public:
        TrainedSkill(void);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define TrainedSkill_PROPERTIES(_) \
                DL_ENUM(_, SkillType, Skill, Skill_Unknown, skillTypeMap)

        DL_DECLARE_ACCESS(TrainedSkill_PROPERTIES)
        DL_DECLARE_VARIABLES(TrainedSkill_PROPERTIES)

        friend class LevelTraining;
};
