// OptionalBuffsFile.h
//

#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "OptionalBuff.h"

class OptionalBuffsFile :
    public XmlLib::SaxContentElement
{
    public:
        OptionalBuffsFile(const std::string & filename);
        ~OptionalBuffsFile(void);

        bool Read();
        const std::list<OptionalBuff> & OptionalBuffs() const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();

    private:
        std::string m_filename;
        std::list<OptionalBuff> m_loadedBuffs;
        size_t m_loadTotal;
};

