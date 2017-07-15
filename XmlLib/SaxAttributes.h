// SaxAttributes.h
//
#pragma once

#include "SaxString.h"
#include <vector>

namespace XmlLib
{
    class SaxAttributesImpl;

    class SaxAttributes
    {
        public:

            void Insert(const XmlLib::SaxString & key, const XmlLib::SaxString & value);

            bool HasAttribute(const XmlLib::SaxString & key) const;

            XmlLib::SaxString & operator[] (const XmlLib::SaxString & key);
            const XmlLib::SaxString & operator[] (const XmlLib::SaxString & key) const;

            size_t Size() const;
            const XmlLib::SaxString & Key(size_t index) const;
            const XmlLib::SaxString & Value(size_t index) const;

            std::wstring Format() const;

        private:
            std::vector<XmlLib::SaxString> m_keys;
            std::vector<XmlLib::SaxString> m_values;
    };
}