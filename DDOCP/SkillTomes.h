// SkillTomes.h
//
#pragma once
#include "XmlLib\DLMacros.h"

class SkillTomes :
    public XmlLib::SaxContentElement
{
    public:
        SkillTomes(void);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SkillTomes_PROPERTIES(_) \
                DL_SIMPLE(_, size_t, Balance, 0) \
                DL_SIMPLE(_, size_t, Bluff, 0) \
                DL_SIMPLE(_, size_t, Concentration, 0) \
                DL_SIMPLE(_, size_t, Diplomacy, 0) \
                DL_SIMPLE(_, size_t, DisableDevice, 0) \
                DL_SIMPLE(_, size_t, Haggle, 0) \
                DL_SIMPLE(_, size_t, Heal, 0) \
                DL_SIMPLE(_, size_t, Hide, 0) \
                DL_SIMPLE(_, size_t, Intimidate, 0) \
                DL_SIMPLE(_, size_t, Jump, 0) \
                DL_SIMPLE(_, size_t, Listen, 0) \
                DL_SIMPLE(_, size_t, MoveSilently, 0) \
                DL_SIMPLE(_, size_t, OpenLock, 0) \
                DL_SIMPLE(_, size_t, Perform, 0) \
                DL_SIMPLE(_, size_t, Repair, 0) \
                DL_SIMPLE(_, size_t, Search, 0) \
                DL_SIMPLE(_, size_t, SpellCraft, 0) \
                DL_SIMPLE(_, size_t, Spot, 0) \
                DL_SIMPLE(_, size_t, Swim, 0) \
                DL_SIMPLE(_, size_t, Tumble, 0) \
                DL_SIMPLE(_, size_t, UMD, 0)

        DL_DECLARE_ACCESS(SkillTomes_PROPERTIES)
        DL_DECLARE_VARIABLES(SkillTomes_PROPERTIES)

        friend class Character;
};
