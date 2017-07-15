// ActiveStances.h
//
// An XML object wrapper that holds information on an affect that a feat has.
#pragma once
#include "XmlLib\DLMacros.h"

class ActiveStances :
    public XmlLib::SaxContentElement
{
    public:
        ActiveStances();
        void Write(XmlLib::SaxWriter * writer) const;

        bool IsStanceActive(const std::string & name) const;
        void AddActiveStance(const std::string & name);
        void RevokeStance(const std::string & name);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define ActiveStances_PROPERTIES(_) \
                DL_STRING_LIST(_, Stances)

        DL_DECLARE_ACCESS(ActiveStances_PROPERTIES)
        DL_DECLARE_VARIABLES(ActiveStances_PROPERTIES)

        friend class Character;
};
