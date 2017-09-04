// EffectDescriptionsFile.h
//

#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "EffectDescription.h"

class EffectDescriptionsFile :
    public XmlLib::SaxContentElement
{
    public:
        EffectDescriptionsFile(const std::string & filename);
        ~EffectDescriptionsFile(void);

        bool Read();
        const std::list<EffectDescription> & EffectDescriptions() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();

    private:
        std::string m_filename;
        std::list<EffectDescription> m_loadedEffectDescriptions;
        size_t m_loadTotal;
};

