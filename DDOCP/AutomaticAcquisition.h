// AutomaticAcquisition.h
//
// An XML object wrapper that holds information on an affect that a feat has.
#pragma once
#include "XmlLib\DLMacros.h"
#include "ClassTypes.h"
#include "RaceTypes.h"

class AutomaticAcquisition :
    public XmlLib::SaxContentElement
{
    public:
        AutomaticAcquisition(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool VerifyObject(std::stringstream * ss) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define AutomaticAcquisition_PROPERTIES(_) \
                DL_FLAG(_, IgnoreRequirements) \
                DL_OPTIONAL_SIMPLE(_, size_t, BAB, 0) \
                DL_OPTIONAL_ENUM(_, ClassType, Class, Class_Unknown, classTypeMap) \
                DL_OPTIONAL_SIMPLE(_, size_t, Level, 0) \
                DL_OPTIONAL_ENUM(_, RaceType, Race, Race_Unknown, raceTypeMap) \
                DL_OPTIONAL_SIMPLE(_, size_t, SpecificLevel, 0)

        DL_DECLARE_ACCESS(AutomaticAcquisition_PROPERTIES)
        DL_DECLARE_VARIABLES(AutomaticAcquisition_PROPERTIES)
};
