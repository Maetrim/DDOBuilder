// SelectorExclusions.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "SelectorExclusion.h"

class EnhancementTree;
class Feat;

class SelectorExclusions :
    public XmlLib::SaxContentElement
{
    public:
        SelectorExclusions(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool VerifyObject(
                std::stringstream * ss,
                const std::list<EnhancementTree> & trees,
                const std::list<Feat> & feats) const;
    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SelectorExclusions_PROPERTIES(_) \
                DL_OBJECT_LIST(_, SelectorExclusion, Exclude)

        DL_DECLARE_ACCESS(SelectorExclusions_PROPERTIES)
        DL_DECLARE_VARIABLES(SelectorExclusions_PROPERTIES)
};
