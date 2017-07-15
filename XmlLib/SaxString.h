// SaxString.h
//
#pragma once

#include "XmlLibExports.h"
#include <string>
#include <ostream>

namespace XmlLib
{
    class SaxString
    {
        public:
            SaxString();
            SaxString(const wchar_t * start, const wchar_t * end); // start up to not including end
            SaxString(const wchar_t * start, size_t length);
            SaxString(const wchar_t * start); // null terminated

            explicit SaxString(const std::wstring & str);
            explicit SaxString(const std::string & str);
            explicit SaxString(const char * start, const char * end); // start up to not including end
            explicit SaxString(const char * start, size_t length);
            explicit SaxString(const char * start); // null terminated

            SaxString(const SaxString & copy);
            const SaxString & operator=(const SaxString & copy);

            operator std::string () const;
            operator std::wstring () const;

            const wchar_t * start() const;
            const wchar_t * c_str() const;
            size_t size() const;

            SaxString Trim() const;

            bool operator < (const SaxString & other) const;
            bool operator == (const SaxString & other) const;
            bool operator != (const SaxString & other) const;

        private:
            void SetOwnBufferFromAscii(const char * begin, const char * end);

            const wchar_t * m_start;
            size_t m_length;
            mutable std::wstring m_ownBuffer; // holds the string in wide when needed
    };
}

std::ostream & operator << (std::ostream & o, const XmlLib::SaxString & str);

