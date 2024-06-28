// Spell.h
//
// An XML object wrapper that holds information on a spell
#pragma once
#include "XmlLib\DLMacros.h"
#include "ClassTypes.h"
#include "EnergyTypes.h"
#include "SpellSchoolTypes.h"
#include "Effect.h"
#include "Stance.h"
#include "DC.h"

class Character;

class Spell :
    public XmlLib::SaxContentElement
{
    public:
        Spell(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool operator<(const Spell & other) const;
        void AddImage(CImageList * pIL) const;

        size_t SpellLevel(ClassType ct) const;
        std::vector<std::string> Metamagics() const;

        void VerifyObject() const;

        size_t SpellDC(const Character & charData, ClassType ct, size_t spellLevel, size_t maxSpellLevel) const;
        std::vector<Effect> UpdatedEffects(size_t castingLevel) const;

        int DynamicCasterLevel(const Character & charData, ClassType ct) const;
        int DynamicMaxCasterLevel(const Character & charData, ClassType ct) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Spell_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Description) \
                DL_STRING(_, Icon) \
                DL_OPTIONAL_ENUM(_, SpellSchoolType, School, SpellSchool_Unknown, spellSchoolTypeMap) \
                DL_OPTIONAL_STRING(_, SPCost) \
                DL_OPTIONAL_ENUM(_, EnergyType, Energy, Energy_Unknown, energyTypeMap) \
                DL_OBJECT_LIST(_, Stance, StanceData) \
                DL_OBJECT_VECTOR(_, Effect, Effects) \
                DL_OBJECT_LIST(_, DC, EffectDC) \
                DL_FLAG(_, Accelerate) \
                DL_FLAG(_, Embolden) \
                DL_FLAG(_, Empower) \
                DL_FLAG(_, EmpowerHealing) \
                DL_FLAG(_, Enlarge) \
                DL_FLAG(_, Extend) \
                DL_FLAG(_, Heighten) \
                DL_FLAG(_, Intensify) \
                DL_FLAG(_, Maximize) \
                DL_FLAG(_, Quicken) \
                DL_OPTIONAL_SIMPLE(_, int, AcolyteOfTheSkin, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Alchemist, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Artificer, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Bard, 0) \
                DL_OPTIONAL_SIMPLE(_, int, BlightCaster, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Stormsinger, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Cleric, 0) \
                DL_OPTIONAL_SIMPLE(_, int, DarkApostate, 0) \
                DL_OPTIONAL_SIMPLE(_, int, DarkHunter, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Druid, 0) \
                DL_OPTIONAL_SIMPLE(_, int, FavoredSoul, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Paladin, 0) \
                DL_OPTIONAL_SIMPLE(_, int, SacredFist, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Ranger, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Sorcerer, 0) \
                DL_OPTIONAL_SIMPLE(_, int, WildMage, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Warlock, 0) \
                DL_OPTIONAL_SIMPLE(_, int, Wizard, 0) \
                DL_OPTIONAL_SIMPLE(_, int, MaxCasterLevel, 0)

        DL_DECLARE_ACCESS(Spell_PROPERTIES)
        DL_DECLARE_VARIABLES(Spell_PROPERTIES)
};
