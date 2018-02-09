// Dice.h
//
// An XML object wrapper that holds information on dice to be rolled
#pragma once
#include "XmlLib\DLMacros.h"
#include "EnergyTypes.h"
#include "SpellPowerTypes.h"

class Dice :
    public XmlLib::SaxContentElement
{
    public:
        Dice(void);
        void Write(XmlLib::SaxWriter * writer) const;

        size_t Number(size_t index) const;
        size_t Sides(size_t index) const;
        int Bonus(size_t index) const;
        std::string Description(size_t numStacks) const;

        bool operator==(const Dice & other) const;
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        // this gives us Number d Sides. e.g. (3d6 + 10) Fire damage - scales with Fire spell power
        #define Dice_PROPERTIES(_) \
                DL_VECTOR(_, size_t, Number) \
                DL_VECTOR(_, size_t, Sides) \
                DL_OPTIONAL_VECTOR(_, int, Bonus) \
                DL_OPTIONAL_ENUM(_, EnergyType, Energy, Energy_Unknown, energyTypeMap) \
                DL_FLAG(_, ScalesWithMeleePower) \
                DL_FLAG(_, ScalesWithRangedPower) \
                DL_FLAG(_, ScalesWithSpellPower) \
                DL_OPTIONAL_ENUM(_, SpellPowerType, SpellPower, SpellPower_Unknown, spellPowerTypeMap)

        DL_DECLARE_ACCESS(Dice_PROPERTIES)
        DL_DECLARE_VARIABLES(Dice_PROPERTIES)
};
