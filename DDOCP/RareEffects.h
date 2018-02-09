// RareEffects.h
//
// An XML object wrapper that holds information on which equipment slot(s)
// and item can be equipped to.
#pragma once
#include "XmlLib\DLMacros.h"
#include "Effect.h"

class RareEffects :
    public XmlLib::SaxContentElement
{
    public:
        RareEffects(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool VerifyObject(std::stringstream * ss) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define RareEffects_PROPERTIES(_) \
                DL_OBJECT_LIST(_, Effect, Effects)

        DL_DECLARE_ACCESS(RareEffects_PROPERTIES)
        DL_DECLARE_VARIABLES(RareEffects_PROPERTIES)
};
