// Selector.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "EnhancementSelection.h"

class EnhancementTree;
class Feat;

class Selector :
    public XmlLib::SaxContentElement
{
    public:
        Selector(void);
        void Write(XmlLib::SaxWriter * writer) const;
        std::string SelectedIcon(const std::string & selectionName) const;
        void RenderIcon(const std::string & selection, CDC * pDC, const CRect & itemRect) const;
        std::string Selector::DisplayName(const std::string & selection) const;

        std::list<Effect> Effects(const std::string & selection) const;
        std::list<DC> EffectDCs(const std::string & selection) const;
        std::list<Stance> Stances(const std::string & selection) const;

        size_t MinSpent(const std::string& selection, size_t defaultMinSpent) const;
        bool CostVaries(const std::string& selection) const;
        size_t Cost(const std::string& selection, size_t rank) const;
        const std::vector<size_t>& ItemCosts(const std::string& selection) const;
        bool IsSelectionClickie(const std::string & selection) const;
        bool RequiresEnhancement(const std::string& name, const std::string& selection, const std::string& subSelection) const;
        bool HasTrainableOption(const Character & charData, size_t spentInTree) const;

        bool VerifyObject(
                std::stringstream * ss,
                const std::list<EnhancementTree> & trees,
                const std::list<Feat> & feats) const;
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define Selector_PROPERTIES(_) \
                DL_STRING_LIST(_, Exclude) \
                DL_OBJECT_LIST(_, EnhancementSelection, Selections)

        DL_DECLARE_ACCESS(Selector_PROPERTIES)
        DL_DECLARE_VARIABLES(Selector_PROPERTIES)

        friend class CEnhancementEditorDialog;
};
