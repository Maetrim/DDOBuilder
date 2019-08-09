// SlotUpgrade.h
//
// An XML object wrapper that holds information on which equipment slot(s)
// and item can be equipped to.
#pragma once
#include "XmlLib\DLMacros.h"
#include "Effect.h"

class SlotUpgrade :
    public XmlLib::SaxContentElement
{
    public:
        SlotUpgrade(void);
        void Write(XmlLib::SaxWriter * writer) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SlotUpgrade_PROPERTIES(_) \
                DL_STRING(_, Type) \
                DL_STRING_VECTOR(_, UpgradeType)

        DL_DECLARE_ACCESS(SlotUpgrade_PROPERTIES)
        DL_DECLARE_VARIABLES(SlotUpgrade_PROPERTIES)

        friend class CItemSelectDialog;
        friend class CFindGearDialog;
};
