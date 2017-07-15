// SaxContentElement.cpp
//
#include "stdafx.h"
#include "XmlLib\SaxContentElement.h"

using XmlLib::SaxString;
using XmlLib::SaxAttributes;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// SaxContentElementInterface

using XmlLib::SaxContentElementInterface;

SaxContentElementInterface::SaxContentElementInterface() :
    m_readErrorHandler(NULL)
{
}

SaxContentElementInterface::~SaxContentElementInterface()
{
}

SaxContentElementInterface * SaxContentElementInterface::StartElement(
        const SaxString & name,
        const SaxAttributes & attributes)
{
    return NULL;
}

void SaxContentElementInterface::Characters(const SaxString & chars)
{
}

void SaxContentElementInterface::EndElement()
{
}

SaxContentElementInterface * SaxContentElementInterface::SaxReadErrorHandler() const
{
    return m_readErrorHandler;
}

void SaxContentElementInterface::SetSaxReadErrorHandler(SaxContentElementInterface * handler)
{
    m_readErrorHandler = handler;
}

void SaxContentElementInterface::ClearSaxReadErrorHandler()
{
    m_readErrorHandler = NULL;
}

void SaxContentElementInterface::ReportSaxReadError(const std::string & errorDescription)
{
    // propagate or die
    if (m_readErrorHandler != NULL)
    {
        m_readErrorHandler->ReportSaxReadError(errorDescription);
    }
    else
    {
        throw errorDescription;
    }
}

//////////////////////////////////////////////////////////////////////
// SaxSimpleElement<std::string>

XmlLib::SaxSimpleElement<std::string> XmlLib::SaxSimpleElement<std::string>::s_singleton;

XmlLib::SaxSimpleElement<std::string> * XmlLib::SaxSimpleElement<std::string>::Handle(std::string * t)
{
    t->erase();
    s_singleton.m_t = t;
    return &s_singleton;
}

void XmlLib::SaxSimpleElement<std::string>::Characters(const SaxString & chars)
{
    // ensure the capacity grows faster than adding it bit by bit as that
    // takes a loooooooong time for big strings
    if (m_t->capacity() < m_t->size() + chars.size() + 1)
    {
        m_t->reserve(m_t->capacity() * 2 + chars.size() + 1);
    }

    *m_t += chars;
}

//////////////////////////////////////////////////////////////////////
// SaxSimpleElement<std::wstring>

XmlLib::SaxSimpleElement<std::wstring> XmlLib::SaxSimpleElement<std::wstring>::s_singleton;

XmlLib::SaxSimpleElement<std::wstring> * XmlLib::SaxSimpleElement<std::wstring>::Handle(std::wstring * t)
{
    t->erase();
    s_singleton.m_t = t;
    return &s_singleton;
}

void XmlLib::SaxSimpleElement<std::wstring>::Characters(const SaxString & chars)
{
    *m_t += chars;
}

//////////////////////////////////////////////////////////////////////
// SaxContentElement

using XmlLib::SaxContentElement;

const XmlLib::SaxString f_saxVersionAttributeName = L"version";

SaxContentElement::SaxContentElement(const SaxString & elementName) :
    m_elementName(elementName),
    m_elementVersion(0),
    m_elementHandlingVersion(0),
    m_readErrorMode(REM_terminate)
{
}

SaxContentElement::SaxContentElement(const SaxString & elementName, unsigned version) :
    m_elementName(elementName),
    m_elementVersion(version),
    m_elementHandlingVersion(0),
    m_readErrorMode(REM_terminate)
{
}

const XmlLib::SaxString & SaxContentElement::ElementName() const
{
    return m_elementName;
}

bool SaxContentElement::SaxElementIsSelf(
        const SaxString & name,
        const SaxAttributes & attributes)
{
    bool self = (name == m_elementName);
    if (self)
    {
        SaxSetAttributes(attributes);
    }
    return self;
}

void SaxContentElement::SaxSetAttributes(const SaxAttributes & attributes)
{
    // store the id attribute (if there is one)
    if (attributes.HasAttribute(f_saxVersionAttributeName))
    {
        std::wstringstream wssSax(attributes[f_saxVersionAttributeName]);
        wssSax >> m_elementHandlingVersion;
        if (wssSax.fail())
        {
            std::stringstream ss;
            ss << "XML element " << m_elementName << " found with invalid version attribute";
            ReportSaxReadError(ss.str());
        }
    }
}

unsigned SaxContentElement::ElementHandlingVersion() const
{
    return m_elementHandlingVersion;
}

SaxAttributes SaxContentElement::VersionAttributes() const
{
    SaxAttributes attributes;
    if (m_elementVersion > 0)
    {
        std::wstringstream wssSax;
        wssSax << m_elementVersion;
        attributes[f_saxVersionAttributeName] = SaxString(wssSax.str());
    }
    return attributes;
}

void SaxContentElement::ReportSaxReadError(const std::string & errorDescription)
{
    if (m_readErrorMode == REM_accumulate)
    {
        // SAX read errors are collected and the owner must check for read errors
        m_saxReadErrors.push_back(errorDescription);
    }
    else
    {
        SaxContentElementInterface::ReportSaxReadError(errorDescription);
    }
}

void SaxContentElement::SetReadErrorMode(SaxContentElement::ReadErrorMode mode)
{
    m_readErrorMode = mode;
    m_saxReadErrors.clear();
}

SaxContentElement::ReadErrorMode SaxContentElement::GetReadErrorMode() const
{
    return m_readErrorMode;
}

bool SaxContentElement::HasReadErrors() const
{
    return !m_saxReadErrors.empty();
}

const std::vector<std::string> & SaxContentElement::ReadErrors() const
{
    return m_saxReadErrors;
}

const SaxContentElement & SaxContentElement::operator=(const SaxContentElement & copy)
{
    // only copy element name if we don't already have one
    if (m_elementName.size() == 0)
    {
        m_elementName = copy.m_elementName;
        m_elementVersion = copy.m_elementVersion;
    }
    m_elementHandlingVersion = copy.m_elementHandlingVersion;
    m_readErrorMode = copy.m_readErrorMode;
    m_saxReadErrors = copy.m_saxReadErrors;
    return *this;
}

//////////////////////////////////////////////////////////////////////
