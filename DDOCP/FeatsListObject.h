// FeatsListObject.h
//
// An XML object wrapper that holds information on an affect that a feat has.
#pragma once
#include "XmlLib\DLMacros.h"
#include "TrainedFeat.h"

class FeatsListObject :
    public XmlLib::SaxContentElement
{
    public:
        FeatsListObject(const XmlLib::SaxString & objectName);
        void Write(XmlLib::SaxWriter * writer) const;

        void TrainFeat(const std::string & featName, TrainableFeatTypes type, size_t level);
        std::string RevokeFeat(TrainableFeatTypes type);
        void RevokeAllFeats(TrainableFeatTypes type);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define FeatsListObject_PROPERTIES(_) \
                DL_OBJECT_LIST(_, TrainedFeat, Feats)

        DL_DECLARE_ACCESS(FeatsListObject_PROPERTIES)
        DL_DECLARE_VARIABLES(FeatsListObject_PROPERTIES)

        friend class Character;
};
