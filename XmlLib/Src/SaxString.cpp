// SaxString.cpp
//
#include "stdafx.h"
#include "XmlLib\SaxString.h"
#include <comdef.h>
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using XmlLib::SaxString;


SaxString::SaxString() :
    m_start(NULL),
    m_length(0)
{
}

SaxString::SaxString(const wchar_t * start, const wchar_t * end) : // start up to not including end
    m_start(start),
    m_length(end - start)
{
}

SaxString::SaxString(const wchar_t * start, size_t length) :
    m_start(start),
    m_length(length)
{
}

SaxString::SaxString(const wchar_t * start) : // null terminated
    m_start(start),
    m_length(wcslen(start))
{
}

SaxString::SaxString(const std::wstring & str)
{
    m_ownBuffer = str;
    m_start = m_ownBuffer.c_str();
    m_length = m_ownBuffer.size();
}

SaxString::SaxString(const std::string & str)
{
    SetOwnBufferFromAscii(
            str.data(),
            str.data() + str.size());
}

SaxString::SaxString(const char * start, const char * end) // start up to not including end
{
    SetOwnBufferFromAscii(start, end);
}

SaxString::SaxString(const char * start, size_t length)
{
    SetOwnBufferFromAscii(start, start + length);
}

SaxString::SaxString(const char * start) // null terminated
{
    SetOwnBufferFromAscii(start, start + strlen(start));
}

SaxString::SaxString(const SaxString & copy)
{
    if (copy.m_start == copy.m_ownBuffer.c_str())
    {
        // is using own buffer
        m_ownBuffer = copy.m_ownBuffer;
        m_start = m_ownBuffer.c_str();
        m_length = m_ownBuffer.size();
    }
    else
    {
        m_start = copy.m_start;
        m_length = copy.m_length;
    }
}

const SaxString & SaxString::operator=(const SaxString & copy)
{
    if (copy.m_start == copy.m_ownBuffer.c_str())
    {
        // is using own buffer
        m_ownBuffer = copy.m_ownBuffer;
        m_start = m_ownBuffer.c_str();
        m_length = m_ownBuffer.size();
    }
    else
    {
        m_ownBuffer.erase();
        m_start = copy.m_start;
        m_length = copy.m_length;
    }
    return *this;
}

SaxString::operator std::string () const
{
    std::vector<char> buf(m_length * 2 + 1); // room for NUL
    std::wstring unistr(m_start, m_length);
    size_t strLength;
    wcstombs_s(&strLength, buf.data(), buf.size(), unistr.c_str(), _TRUNCATE);
    return std::string(buf.data());
}

SaxString::operator std::wstring () const
{
    return std::wstring(m_start, m_length);
}

const wchar_t * SaxString::c_str() const
{
    if (m_ownBuffer.size() != m_length)
    {
        m_ownBuffer.assign(m_start, m_length);
    }
    return m_ownBuffer.c_str();
}

const wchar_t * SaxString::start() const
{
    return m_start;
}

size_t SaxString::size() const
{
    return m_length;
}

SaxString SaxString::Trim() const
{
    // trim out all leading and trailing white space (blanks and new lines)
    const wchar_t * first = m_start;
    const wchar_t * end = m_start + m_length;
    while (first < end)
    {
        if (*first != L' ' && *first != L'\x0a')
        {
            // non-whitespace
            break;
        }
        ++first;
    }
    const wchar_t * last = end - 1;
    while (last > first)
    {
        if (*last != L' ' && *last != L'\x0a')
        {
            // non-whitespace
            break;
        }
        --last;
    }

    return SaxString(first, last+1);
}

bool SaxString::operator < (const SaxString & other) const
{
    bool less;

    int compare = wcsncmp(m_start, other.m_start, min(m_length, other.m_length));
    if (compare < 0)
    {
        less = true; // less over the length covered by both
    }
    else if (compare > 0)
    {
        less = false; // greater over the length covered by both
    }
    else
    {
        if (m_length < other.m_length)
        {
            less = true; // shorter
        }
        else
        {
            less = false; // same over shared length and same length or longer
        }
    }

    return less;
}

bool SaxString::operator == (const SaxString & other) const
{
    bool eq;

    if (m_length != other.m_length)
    {
        eq = false; // different lengths
    }
    else
    {
        // compare (length is the same)
        int compare = wcsncmp(m_start, other.m_start, m_length);
        if (compare != 0)
        {
            eq = false; // different over the length covered by both
        }
        else
        {
            eq = true; // same over shared length and same length or longer
        }
    }

    return eq;
}

bool SaxString::operator != (const SaxString & other) const
{
    return !operator==(other);
}

std::ostream & operator << (std::ostream & o, const XmlLib::SaxString & str)
{
    o << std::string(str);
    return o;
}

void SaxString::SetOwnBufferFromAscii(const char * begin, const char * end)
{
    size_t stringSize = end - begin;
    std::vector<wchar_t> buf(stringSize + 1); // room for NUL
    size_t strLength;
    mbstowcs_s(&strLength, buf.data(), buf.size(), begin, stringSize);
    m_ownBuffer = std::wstring(buf.begin(), buf.begin() + strLength - 1); // not including the NUL
    m_start = m_ownBuffer.c_str();
    m_length = m_ownBuffer.size();
}
