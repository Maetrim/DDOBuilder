// GuildBuffsFile.h
//

#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "GuildBuff.h"

class GuildBuffsFile :
    public XmlLib::SaxContentElement
{
    public:
        GuildBuffsFile(const std::string & filename);
        ~GuildBuffsFile(void);

        bool Read();
        const std::list<GuildBuff> & GuildBuffs() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();

    private:
        std::string m_filename;
        std::list<GuildBuff> m_loadedGuildBuffs;
        size_t m_loadTotal;
};

