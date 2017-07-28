// GuildBuff.h
//
// An XML object wrapper that holds information on an item that can be equipped
#pragma once
#include "XmlLib\DLMacros.h"
#include "Effect.h"

class GuildBuff :
    public XmlLib::SaxContentElement
{
    public:
        GuildBuff(void);
        GuildBuff(const XmlLib::SaxString & objectName);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define GuildBuff_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Description) \
                DL_SIMPLE(_, size_t, Level, 0) \
                DL_OBJECT_LIST(_, Effect, Effects)

        DL_DECLARE_ACCESS(GuildBuff_PROPERTIES)
        DL_DECLARE_VARIABLES(GuildBuff_PROPERTIES)
};
