// AugmentsFile.h
//

#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "Augment.h"

class AugmentsFile :
    public XmlLib::SaxContentElement
{
    public:
        AugmentsFile(const std::string & filename);
        ~AugmentsFile(void);

        void Read();
        const std::list<Augment> & Augments() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();

        std::string m_filename;
        std::list<Augment> m_loadedAugments;
        size_t m_loadTotal;
};

