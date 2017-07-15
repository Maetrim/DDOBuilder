// Augment.h
//
// An XML object wrapper that holds information on which equipment slot(s)
// and item can be equipped to.
#pragma once
#include "XmlLib\DLMacros.h"
#include "AugmentTypes.h"
#include "Effect.h"

class Augment :
    public XmlLib::SaxContentElement
{
    public:
        Augment(void);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Augment_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Description) \
                DL_SIMPLE(_, size_t, MinLevel, 0) \
                DL_ENUM(_, AugmentType, Type, Augment_Unknown, augmentTypeMap) \
                DL_OBJECT_LIST(_, Effect, Effects)

        DL_DECLARE_ACCESS(Augment_PROPERTIES)
        DL_DECLARE_VARIABLES(Augment_PROPERTIES)
};
