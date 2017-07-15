// SelectorExclusion.h
//
#pragma once
#include "XmlLib\DLMacros.h"

class SelectorExclusion :
    public XmlLib::SaxContentElement
{
    public:
        SelectorExclusion(void);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SelectorExclusion_PROPERTIES(_) \
                DL_STRING(_, InternalName)

        DL_DECLARE_ACCESS(SelectorExclusion_PROPERTIES)
        DL_DECLARE_VARIABLES(SelectorExclusion_PROPERTIES)
};
