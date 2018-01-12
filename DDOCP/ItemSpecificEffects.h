// ItemSpecificEffects.h
//
// An XML object wrapper that holds information on dice to be rolled
#pragma once
#include "XmlLib\DLMacros.h"
#include "Effect.h"

class ItemSpecificEffects :
    public XmlLib::SaxContentElement
{
    public:
        ItemSpecificEffects(void);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        // this gives us Number d Sides. e.g. 3d6
        #define ItemSpecificEffects_PROPERTIES(_) \
                DL_OBJECT_VECTOR(_, Effect, Effects)

        DL_DECLARE_ACCESS(ItemSpecificEffects_PROPERTIES)
        DL_DECLARE_VARIABLES(ItemSpecificEffects_PROPERTIES)
};
