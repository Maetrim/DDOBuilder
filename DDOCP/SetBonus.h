// SetBonus.h
//
// An XML object wrapper that holds information on an SetBonus
#pragma once
#include "XmlLib\DLMacros.h"
#include "Effect.h"
#include "Stance.h"

class SetBonus :
    public XmlLib::SaxContentElement
{
    public:
        SetBonus(void);
        void Write(XmlLib::SaxWriter * writer) const;

        void VerifyObject() const;
        bool operator<(const SetBonus& other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SetBonus_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Icon) \
                DL_STRING(_, Description) \
                DL_OBJECT_VECTOR(_, Effect, Effects) \
                DL_OBJECT_LIST(_, Stance, Stances)

        DL_DECLARE_ACCESS(SetBonus_PROPERTIES)
        DL_DECLARE_VARIABLES(SetBonus_PROPERTIES)
};
