// EnhancementsFile.h
//

#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "EnhancementTree.h"

class EnhancementsFile :
    public XmlLib::SaxContentElement
{
    public:
        EnhancementsFile(const std::string & filename);
        ~EnhancementsFile(void);

        void Read();
        const std::list<EnhancementTree> & EnhancementTrees() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();
        void Save();

        std::string m_filename;
        std::list<EnhancementTree> m_loadedTrees;
        size_t m_loadTotal;

        friend class CEnhancementEditorDialog;
};

