// Stance.h
//
#pragma once
#include "XmlLib\DLMacros.h"

class Stance :
    public XmlLib::SaxContentElement
{
    public:
        Stance(void);
        explicit Stance(
                const std::string & name,
                const std::string & icon,
                const std::string & description);
        void Write(XmlLib::SaxWriter * writer) const;

        bool VerifyObject(std::stringstream * ss) const;
        bool operator==(const Stance & other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Stance_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Icon) \
                DL_STRING(_, Description) \
                DL_STRING_LIST(_, IncompatibleStance) \
                DL_FLAG(_, AutoControlled) \
                DL_FLAG(_, SetBonus)

        DL_DECLARE_ACCESS(Stance_PROPERTIES)
        DL_DECLARE_VARIABLES(Stance_PROPERTIES)

        friend class CStancesView;
        friend class Character;
};
