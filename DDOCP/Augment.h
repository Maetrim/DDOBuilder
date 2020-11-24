// Augment.h
//
// An XML object wrapper that holds information on which equipment slot(s)
// and item can be equipped to.
#pragma once
#include "XmlLib\DLMacros.h"
#include "RareEffects.h"
#include "WeaponClassTypes.h"
#include "Stance.h"

class Augment :
    public XmlLib::SaxContentElement
{
    public:
        Augment(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool IsCompatibleWithSlot(const std::string & augmentType) const;
        void AddImage(CImageList * pIL) const;
        bool operator<(const Augment & other) const;

        void VerifyObject() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Augment_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Description) \
                DL_SIMPLE(_, size_t, MinLevel, 0) \
                DL_OPTIONAL_STRING(_, Icon) \
                DL_STRING_LIST(_, Type) \
                DL_STRING_LIST(_, AddAugment) \
                DL_OPTIONAL_STRING(_, GrantAugment) \
                DL_OPTIONAL_STRING(_, GrantConditionalAugment) \
                DL_OPTIONAL_ENUM(_, WeaponClassType, WeaponClass, WeaponClass_Unknown, weaponClassTypeMap) \
                DL_FLAG(_, DualValues) \
                DL_FLAG(_, EnterValue) \
                DL_OBJECT_LIST(_, Stance, StanceData) \
                DL_OBJECT_LIST(_, Effect, Effects) \
                DL_OPTIONAL_OBJECT(_, RareEffects, Rares)

        DL_DECLARE_ACCESS(Augment_PROPERTIES)
        DL_DECLARE_VARIABLES(Augment_PROPERTIES)
};
