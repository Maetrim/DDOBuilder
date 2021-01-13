// SetBonusFile.h
//

#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "SetBonus.h"

class SetBonusFile :
    public XmlLib::SaxContentElement
{
    public:
        SetBonusFile(const std::string & filename);
        ~SetBonusFile(void);

        void Read();
        const std::list<SetBonus> & Sets() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();

        std::string m_filename;
        std::list<SetBonus> m_loadedSetBonuses;
        size_t m_loadTotal;
};

