// DC.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "Dice.h"

#include "AbilityTypes.h"
#include "ClassTypes.h"
#include "TacticalTypes.h"

class DC :
    public XmlLib::SaxContentElement
{
    public:
        DC(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool VerifyObject(std::stringstream * ss) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        // note that all the optional XML elements can be
        // used by a variety of feat effects as different interpretations
        #define DC_PROPERTIES(_) \
                DL_OPTIONAL_STRING(_, DisplayName) \
                DL_OPTIONAL_ENUM(_, DCVersusType, Versus, DCVersusType_Unknown, DCVersusTypeMap) \
                DL_OPTIONAL_VECTOR(_, int, AmountVector) \
                DL_ENUM_LIST(_, AbilityType, FullAbility, Ability_Unknown, abilityTypeMap) \
                DL_ENUM_LIST(_, AbilityType, ModAbility, Ability_Unknown, abilityTypeMap) \
                DL_OPTIONAL_ENUM(_, TacticalType, Tactical, Tactical_Unknown, tacticalTypeMap) \
                DL_OPTIONAL_ENUM(_, ClassType, ClassLevel, Class_Unknown, classTypeMap) \
                DL_OPTIONAL_ENUM(_, ClassType, HalfClassLevel, Class_Unknown, classTypeMap)

        DL_DECLARE_ACCESS(DC_PROPERTIES)
        DL_DECLARE_VARIABLES(DC_PROPERTIES)

};
