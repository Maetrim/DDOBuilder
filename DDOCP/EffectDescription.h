// EffectDescription.h
//
#pragma once
#include "XmlLib\DLMacros.h"

// this class is used in two ways:
// 1: As an element in the EffectDescriptions.xml file, in which case only
//    the EffectName and Description elements are used.
// 2: An an element in an Item object, in which case all elements except
//    the description field will be present, as that is looked up from the global
//    list of effects from EffectDescriptions.

class EffectDescription :
    public XmlLib::SaxContentElement
{
    public:
        EffectDescription(void);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define EffectDescription_PROPERTIES(_) \
                DL_STRING(_, EffectName) \
                DL_OPTIONAL_STRING(_, Description) \
                DL_OPTIONAL_STRING(_, Value1) \
                DL_OPTIONAL_STRING(_, Value2) \
                DL_OPTIONAL_STRING(_, Value3) \
                DL_OPTIONAL_STRING(_, Value4) \
                DL_OPTIONAL_STRING(_, Value5)

        DL_DECLARE_ACCESS(EffectDescription_PROPERTIES)
        DL_DECLARE_VARIABLES(EffectDescription_PROPERTIES)
};
