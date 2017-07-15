// TwistOfFate.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "TrainedEnhancement.h"

class TwistOfFate :
    public XmlLib::SaxContentElement
{
    public:
        TwistOfFate(void);
        explicit TwistOfFate(const std::string & name, const std::string & icon);
        void Write(XmlLib::SaxWriter * writer) const;

        void IncrementTier();
        void DecrementTier();

        bool VerifyObject(std::stringstream * ss) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define TwistOfFate_PROPERTIES(_) \
                DL_SIMPLE(_, size_t, Tier, 0) \
                DL_OPTIONAL_OBJECT(_, TrainedEnhancement, Twist)

        DL_DECLARE_ACCESS(TwistOfFate_PROPERTIES)
        DL_DECLARE_VARIABLES(TwistOfFate_PROPERTIES)

        friend class Character;
};
