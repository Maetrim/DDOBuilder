// SelectedDestinyTrees.h
//
#pragma once
#include "XmlLib\DLMacros.h"

class SelectedDestinyTrees :
    public XmlLib::SaxContentElement
{
    public:
        SelectedDestinyTrees();
        void Write(XmlLib::SaxWriter * writer) const;

        bool IsTreePresent(const std::string & treeName) const;
        const std::string & Tree(size_t index) const;
        void SetTree(size_t index, const std::string & treeName);
        void SetNotSelected(size_t index);
        void SwapTrees(const std::string & tree1, const std::string & tree2);

        static bool IsNotSelected(const std::string & treeName);

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        #define SelectedDestinyTrees_PROPERTIES(_) \
                DL_STRING_VECTOR(_, TreeName)

        DL_DECLARE_ACCESS(SelectedDestinyTrees_PROPERTIES)
        DL_DECLARE_VARIABLES(SelectedDestinyTrees_PROPERTIES)
};
