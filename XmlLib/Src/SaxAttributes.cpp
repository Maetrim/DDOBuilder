// SaxAttributes.cpp
//
#include "stdafx.h"
#include "XmlLib\SaxAttributes.h"

using XmlLib::SaxAttributes;
using XmlLib::SaxString;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace
{
    const XmlLib::SaxString SaxEmptyString = L""; // empty string used for returning stuff
}


void SaxAttributes::Insert(const XmlLib::SaxString & key, const XmlLib::SaxString & value)
{
    m_keys.push_back(key);
    m_values.push_back(value);
}

bool SaxAttributes::HasAttribute(const XmlLib::SaxString & key) const
{
    bool hasKey = false;

    for (size_t index = 0; index < m_keys.size(); ++index)
    {
        if (m_keys[index] == key)
        {
            hasKey = true;
            break;
        }
    }

    return hasKey;
}

XmlLib::SaxString & SaxAttributes::operator[] (const XmlLib::SaxString & key)
{
    XmlLib::SaxString * retval = NULL;

    for (size_t index = 0; index < m_keys.size(); ++index)
    {
        if (m_keys[index] == key)
        {
            retval = &m_values[index];
            break;
        }
    }

    if (retval == NULL)
    {
        m_keys.push_back(key);
        m_values.push_back(SaxEmptyString);
        retval = &m_values.back();
    }

    return *retval;
}

const XmlLib::SaxString & SaxAttributes::operator[] (const XmlLib::SaxString & key) const
{
    const XmlLib::SaxString * retval = &SaxEmptyString;

    for (size_t index = 0; index < m_keys.size(); ++index)
    {
        if (m_keys[index] == key)
        {
            retval = &m_values[index];
            break;
        }
    }

    return *retval;
}

size_t SaxAttributes::Size() const
{
    return m_keys.size();
}

const XmlLib::SaxString & SaxAttributes::Key(size_t index) const
{
    return m_keys[index];
}

const XmlLib::SaxString & SaxAttributes::Value(size_t index) const
{
    return m_values[index];
}

std::wstring SaxAttributes::Format() const
{
    std::wstring attributeXml;

    for (size_t index = 0; index < m_keys.size(); ++index)
    {
        attributeXml += L" ";
        attributeXml += m_keys[index];
        attributeXml += L"=\"";
        attributeXml += m_values[index];
        attributeXml += L"\"";
    }

    return attributeXml;
}
