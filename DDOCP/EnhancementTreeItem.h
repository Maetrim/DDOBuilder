// EnhancementTreeItem.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "DC.h"
#include "Effect.h"
#include "Requirements.h"
#include "Selector.h"
#include "Stance.h"

class Character;
class EnhancementTree;
class Feat;

class EnhancementTreeItem :
    public XmlLib::SaxContentElement
{
    public:
        EnhancementTreeItem(void);
        void Write(XmlLib::SaxWriter * writer) const;
        std::string DisplayName(const std::string & selection) const;
        std::string ActiveIcon(const Character & charData) const;
        void RenderIcon(const Character & charData, CDC * pDC, const CRect & itemRect) const;
        bool MeetRequirements(
                const Character & charData,
                const std::string & selection,
                const std::string & treeName) const;
        bool CanTrain(
                const Character & charData,
                const std::string & treeName,
                size_t spentInTree,
                TreeType type) const;
        std::list<Effect> ActiveEffects(const std::string & selection) const;
        std::list<DC> ActiveDCs(const std::string & selection) const;
        void CreateRequirementStrings(
                const Character & charData,
                std::vector<CString> * requirements,
                std::vector<bool> * met,
                size_t level) const;
        std::list<Stance> Stances(const std::string & selection) const;
        size_t Cost(const std::string & selection) const;

        bool VerifyObject(
                std::stringstream * ss,
                const std::list<EnhancementTree> & trees,
                const std::list<Feat> & feats) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define EnhancementTreeItem_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_STRING(_, InternalName) \
                DL_STRING(_, Description) \
                DL_STRING(_, Icon) \
                DL_SIMPLE(_, size_t, XPosition, 0) \
                DL_SIMPLE(_, size_t, YPosition, 0) \
                DL_SIMPLE(_, size_t, Cost, 0) \
                DL_SIMPLE(_, size_t, Ranks, 1) \
                DL_SIMPLE(_, size_t, MinSpent, 0) \
                DL_FLAG(_, Clickie) \
                DL_FLAG(_, ArrowLeft) \
                DL_FLAG(_, ArrowRight) \
                DL_FLAG(_, ArrowUp) \
                DL_FLAG(_, LongArrowUp) \
                DL_FLAG(_, ExtraLongArrowUp) \
                DL_FLAG(_, Tier5) \
                DL_OBJECT_LIST(_, Stance, Stances) \
                DL_OBJECT(_, Requirements, RequirementsToTrain) \
                DL_OPTIONAL_OBJECT(_, Selector, Selections) \
                DL_OBJECT_LIST(_, Effect, Effects) \
                DL_OBJECT_VECTOR(_, DC, EffectDC)

        DL_DECLARE_ACCESS(EnhancementTreeItem_PROPERTIES)
        DL_DECLARE_VARIABLES(EnhancementTreeItem_PROPERTIES)

        mutable bool m_bImageLoaded;
        mutable CImage m_image;
        mutable bool m_bDisabledImageLoaded;
        mutable CImage m_disabledImage;
        friend class CEnhancementEditorDialog;
};
