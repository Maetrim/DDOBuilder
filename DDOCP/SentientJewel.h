// SentientJewel.h
//
// An XML object wrapper that holds information on dice to be rolled
#pragma once
#include "XmlLib\DLMacros.h"
#include "WeaponFiligree.h"
#include "ArtifactFiligree.h"

class SentientJewel :
    public XmlLib::SaxContentElement
{
    public:
        SentientJewel(void);
        void Write(XmlLib::SaxWriter * writer) const;

        void SetNumFiligrees(size_t count);
        void SetPersonality(const std::string & name);

        std::string GetFiligree(size_t fi) const;
        bool IsRareFiligree(size_t fi) const;

        void SetFiligree(size_t fi, const std::string & name);
        void SetFiligreeRare(size_t fi, bool isRare);

        std::string GetArtifactFiligree(size_t fi) const;
        bool IsRareArtifactFiligree(size_t fi) const;

        void SetArtifactFiligree(size_t fi, const std::string & name);
        void SetArtifactFiligreeRare(size_t fi, bool isRare);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SentientJewel_PROPERTIES(_) \
                DL_OPTIONAL_STRING(_, Personality) \
                DL_SIMPLE(_, size_t, NumFiligrees, 0) \
                DL_OBJECT_LIST(_, WeaponFiligree, Filigrees) \
                DL_OBJECT_LIST(_, ArtifactFiligree, ArtifactFiligrees) \
                /* all following is for backwards compatibility */ \
                DL_FLAG(_, SentientSpark) \
                DL_OPTIONAL_STRING(_, Filigree1) \
                DL_OPTIONAL_STRING(_, Filigree2) \
                DL_OPTIONAL_STRING(_, Filigree3) \
                DL_OPTIONAL_STRING(_, Filigree4) \
                DL_OPTIONAL_STRING(_, Filigree5) \
                DL_OPTIONAL_STRING(_, Filigree6) \
                DL_OPTIONAL_STRING(_, Filigree7) \
                DL_OPTIONAL_STRING(_, Filigree8) \
                DL_FLAG(_, RareFiligree1) \
                DL_FLAG(_, RareFiligree2) \
                DL_FLAG(_, RareFiligree3) \
                DL_FLAG(_, RareFiligree4) \
                DL_FLAG(_, RareFiligree5) \
                DL_FLAG(_, RareFiligree6) \
                DL_FLAG(_, RareFiligree7) \
                DL_FLAG(_, RareFiligree8)

        DL_DECLARE_ACCESS(SentientJewel_PROPERTIES)
        DL_DECLARE_VARIABLES(SentientJewel_PROPERTIES)

    private:
        bool Deprecated_HasFiligree(size_t fi) const;
        std::string Deprecated_Filigree(size_t fi) const;
        bool Deprecated_IsRareFiligree(size_t fi) const;
        void Deprecated_ClearFiligree(size_t fi);

        friend class CItemSelectDialog;
};
