// TrainedSpell.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "ClassTypes.h"

class TrainedSpell :
    public XmlLib::SaxContentElement
{
    public:
        TrainedSpell(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool operator==(const TrainedSpell & other) const;
        bool operator<(const TrainedSpell & other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define TrainedSpell_PROPERTIES(_) \
                DL_ENUM(_, ClassType, Class, Class_Unknown, classTypeMap) \
                DL_SIMPLE(_, size_t, Level, 0) \
                DL_STRING(_, SpellName)

        DL_DECLARE_ACCESS(TrainedSpell_PROPERTIES)
        DL_DECLARE_VARIABLES(TrainedSpell_PROPERTIES)

        friend class Character;
};
