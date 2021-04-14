// Filigree.h
//
// An XML object wrapper that holds information on a selected Filigree
#pragma once
#include "XmlLib\DLMacros.h"
#include "Filigree.h"

class Filigree :
    public XmlLib::SaxContentElement
{
    public:
        Filigree(const XmlLib::SaxString& elementName, unsigned verCurrent);
        void Write(XmlLib::SaxWriter * writer) const;

        bool operator==(const Filigree & other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Filigree_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_FLAG(_, Rare)

        DL_DECLARE_ACCESS(Filigree_PROPERTIES)
        DL_DECLARE_VARIABLES(Filigree_PROPERTIES)

        friend class SentientJewel;
        friend class WeaponFiligree;
        friend class ArtifactFiligree;
};
