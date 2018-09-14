// DC.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "Dice.h"

#include "AbilityTypes.h"
#include "ClassTypes.h"
#include "SpellSchoolTypes.h"
#include "SkillTypes.h"
#include "TacticalTypes.h"

class Character;

class DC :
    public XmlLib::SaxContentElement
{
    public:
        DC(void);
        void Write(XmlLib::SaxWriter * writer) const;
        int CalculateDC(const Character * pCharacter) const;
        std::string DCBreakdown(const Character * pCharacter) const;

        bool VerifyObject(std::stringstream * ss) const;
        bool operator==(const DC & other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define DC_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Description) \
                DL_STRING(_, Icon) \
                DL_STRING(_, DCVersus) \
                DL_OPTIONAL_VECTOR(_, int, AmountVector) \
                DL_ENUM_LIST(_, AbilityType, FullAbility, Ability_Unknown, abilityTypeMap) \
                DL_ENUM_LIST(_, AbilityType, ModAbility, Ability_Unknown, abilityTypeMap) \
                DL_ENUM_LIST(_, SpellSchoolType, School, SpellSchool_Unknown, spellSchoolTypeMap) \
                DL_OPTIONAL_ENUM(_, SkillType, Skill, Skill_Unknown, skillTypeMap) \
                DL_OPTIONAL_ENUM(_, TacticalType, Tactical, Tactical_Unknown, tacticalTypeMap) \
                DL_OPTIONAL_ENUM(_, ClassType, ClassLevel, Class_Unknown, classTypeMap) \
                DL_OPTIONAL_ENUM(_, ClassType, HalfClassLevel, Class_Unknown, classTypeMap)

        DL_DECLARE_ACCESS(DC_PROPERTIES)
        DL_DECLARE_VARIABLES(DC_PROPERTIES)

        size_t m_stacks;
};
