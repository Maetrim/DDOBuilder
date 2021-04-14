// WeaponFiligree.h
//
#pragma once
#include "Filigree.h"

class WeaponFiligree :
    public Filigree
{
    public:
        WeaponFiligree(void);
        void Write(XmlLib::SaxWriter* writer) const;

        bool operator==(const WeaponFiligree & other) const;

    protected:
        XmlLib::SaxContentElementInterface* StartElement(
            const XmlLib::SaxString& name,
            const XmlLib::SaxAttributes& attributes);

        virtual void EndElement();

        #define WeaponFiligree_PROPERTIES(_)

        DL_DECLARE_ACCESS(WeaponFiligree_PROPERTIES)
        DL_DECLARE_VARIABLES(WeaponFiligree_PROPERTIES)
};
