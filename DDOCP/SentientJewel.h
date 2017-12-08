// SentientJewel.h
//
// An XML object wrapper that holds information on dice to be rolled
#pragma once
#include "XmlLib\DLMacros.h"

class SentientJewel :
    public XmlLib::SaxContentElement
{
    public:
        SentientJewel(void);
        void Write(XmlLib::SaxWriter * writer) const;

        std::string Filigree(size_t fi) const;
        void SetFiligree(size_t fi, const std::string & name);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        // this gives us Number d Sides. e.g. 3d6
        #define SentientJewel_PROPERTIES(_) \
                DL_OPTIONAL_STRING(_, Personality) \
                DL_OPTIONAL_STRING(_, Filigree1) \
                DL_OPTIONAL_STRING(_, Filigree2) \
                DL_OPTIONAL_STRING(_, Filigree3) \
                DL_OPTIONAL_STRING(_, Filigree4) \
                DL_OPTIONAL_STRING(_, Filigree5) \
                DL_OPTIONAL_STRING(_, Filigree6) \
                DL_OPTIONAL_STRING(_, Filigree7)

        DL_DECLARE_ACCESS(SentientJewel_PROPERTIES)
        DL_DECLARE_VARIABLES(SentientJewel_PROPERTIES)

        friend class CItemSelectDialog;
};
