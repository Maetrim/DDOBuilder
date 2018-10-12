// EnhancementSelection.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "DC.h"
#include "EnhancementSelection.h"
#include "Effect.h"
#include "Requirements.h"
#include "Stance.h"

class Character;
class EnhancementTree;
class Feat;

class EnhancementSelection :
    public XmlLib::SaxContentElement
{
    public:
        EnhancementSelection(void);
        void Write(XmlLib::SaxWriter * writer) const;
        void RenderIcon(CDC * pDC, const CRect & itemRect) const;
        void CreateRequirementStrings(
                const Character & charData,
                std::vector<CString> * requirements,
                std::vector<bool> * met,
                size_t level) const;

        bool VerifyObject(
                std::stringstream * ss,
                const std::list<EnhancementTree> & trees,
                const std::list<Feat> & feats) const;
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define EnhancementSelection_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, Description) \
                DL_STRING(_, Icon) \
                DL_SIMPLE(_, size_t, Cost, 0) \
                DL_FLAG(_, Clickie) \
                DL_OPTIONAL_OBJECT(_, Requirements, RequirementsToTrain) \
                DL_OBJECT_LIST(_, Stance, Stances) \
                DL_OBJECT_LIST(_, Effect, Effects) \
                DL_OBJECT_LIST(_, DC, EffectDC)

        DL_DECLARE_ACCESS(EnhancementSelection_PROPERTIES)
        DL_DECLARE_VARIABLES(EnhancementSelection_PROPERTIES)

        mutable bool m_bImageLoaded;
        mutable CImage m_image;
        friend class CEnhancementEditorDialog;
};
