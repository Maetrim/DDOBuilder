// TrainedFeat.h
//
// An XML object wrapper that holds information on an affect that a feat has.
#pragma once
#include "XmlLib\DLMacros.h"
#include "FeatTypes.h"

class TrainedFeat :
    public XmlLib::SaxContentElement
{
    public:
        TrainedFeat();
        void Write(XmlLib::SaxWriter * writer) const;
        bool operator<(const TrainedFeat & other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define TrainedFeat_PROPERTIES(_) \
                DL_STRING(_, FeatName) \
                DL_ENUM(_, TrainableFeatTypes, Type, TFT_Unknown, trainableFeatTypesMap) \
                DL_SIMPLE(_, size_t, LevelTrainedAt, 0)

        DL_DECLARE_ACCESS(TrainedFeat_PROPERTIES)
        DL_DECLARE_VARIABLES(TrainedFeat_PROPERTIES)

        friend class Character;
        friend class FeatsListObject;
};
