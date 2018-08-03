// OptionalBuff.h
//
// An XML object wrapper that holds information on an item that can be equipped
#pragma once
#include "XmlLib\DLMacros.h"
#include "Effect.h"

class OptionalBuff :
    public XmlLib::SaxContentElement
{
    public:
        OptionalBuff(void);
        OptionalBuff(const XmlLib::SaxString & objectName);
        void Write(XmlLib::SaxWriter * writer) const;

        void VerifyObject() const;
        bool operator<(const OptionalBuff & other) const;
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define OptionalBuff_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_OPTIONAL_STRING(_, Icon) \
                DL_STRING(_, Description) \
                DL_OBJECT_VECTOR(_, Effect, Effects)

        DL_DECLARE_ACCESS(OptionalBuff_PROPERTIES)
        DL_DECLARE_VARIABLES(OptionalBuff_PROPERTIES)

        friend class COptionalBuffSelectDialog;
};
