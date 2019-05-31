// Feat.h
//
// An XML object wrapper that holds information on an affect that a feat has.
#pragma once
#include "XmlLib\DLMacros.h"
#include "AutomaticAcquisition.h"
#include "DC.h"
#include "Effect.h"
#include "FeatGroup.h"
#include "FeatTypes.h"
#include "Requirements.h"
#include "Stance.h"
#include <list>

class Character;
class EnhancementTree;

class Feat :
    public XmlLib::SaxContentElement
{
    public:
        Feat(void);
        void Write(XmlLib::SaxWriter * writer) const;
        void CreateRequirementStrings(
                const Character & charData,
                std::vector<CString> * requirements,
                std::vector<bool> * met,
                size_t level) const;

        void VerifyObject(
                const std::list<EnhancementTree> & allTrees,
                const std::list<Feat> & allfeats) const;
        bool operator<(const Feat & other) const;
        void AddImage(CImageList * pIL) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Feat_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Description) \
                DL_OBJECT_LIST(_, Stance, StanceData) \
                DL_OPTIONAL_SIMPLE(_, size_t, MaxTimesAcquire, 1) \
                DL_OPTIONAL_OBJECT(_, FeatGroup, Group) \
                DL_OPTIONAL_STRING(_, Sphere) \
                DL_OBJECT(_, Requirements, RequirementsToTrain) \
                DL_ENUM(_, FeatAcquisitionType, Acquire, FeatAcquisition_Unknown, featAcquisitionMap) \
                DL_OBJECT_LIST(_, AutomaticAcquisition, AutomaticAcquireAt) \
                DL_STRING(_, Icon) \
                DL_OBJECT_LIST(_, Effect, Effects) \
                DL_OBJECT_LIST(_, DC, EffectDC)

        DL_DECLARE_ACCESS(Feat_PROPERTIES)
        DL_DECLARE_VARIABLES(Feat_PROPERTIES)

        friend class CLevelUpView;
        friend class CFeatsClassControl;
};
