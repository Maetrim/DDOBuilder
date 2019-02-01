// EnhacementTree.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "EnhancementTreeItem.h"
#include "Requirements.h"
#include <list>
#include <map>

class Character;
class Feat;

class EnhancementTree :
    public XmlLib::SaxContentElement
{
    public:
        EnhancementTree(void);
        void Write(XmlLib::SaxWriter * writer) const;
        const EnhancementTreeItem * FindEnhancementItem(const std::string & enhancementName) const;
        bool MeetRequirements(const Character & charData) const;

        bool operator==(const EnhancementTree & other) const;
        void VerifyObject(
                std::map<std::string, int> * names,
                const std::list<EnhancementTree> & trees,
                const std::list<Feat> & feats) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define EnhancementTree_PROPERTIES(_) \
                DL_STRING(_, Name) \
                DL_SIMPLE(_, size_t, Version, 0) \
                DL_OBJECT(_, Requirements, RequirementsToTrain) \
                DL_FLAG(_, IsEpicDestiny) \
                DL_FLAG(_, IsRacialTree) \
                DL_FLAG(_, IsReaperTree) \
                DL_STRING(_, Icon) \
                DL_STRING(_, Background) \
                DL_OBJECT_LIST(_, EnhancementTreeItem, Items)

        DL_DECLARE_ACCESS(EnhancementTree_PROPERTIES)
        DL_DECLARE_VARIABLES(EnhancementTree_PROPERTIES)

        friend class CEnhancementEditorDialog;
};
