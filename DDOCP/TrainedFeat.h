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
        bool operator==(const TrainedFeat & other) const;

        // these are used for forum export only
        size_t Count() const;
        void IncrementCount();

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define TrainedFeat_PROPERTIES(_) \
                DL_STRING(_, FeatName) \
                DL_ENUM(_, TrainableFeatTypes, Type, TFT_Unknown, trainableFeatTypesMap) \
                DL_SIMPLE(_, size_t, LevelTrainedAt, 0) \
                DL_FLAG(_, FeatSwapWarning)

        DL_DECLARE_ACCESS(TrainedFeat_PROPERTIES)
        DL_DECLARE_VARIABLES(TrainedFeat_PROPERTIES)

        size_t m_count;

        friend class Character;
        friend class FeatsListObject;
};
