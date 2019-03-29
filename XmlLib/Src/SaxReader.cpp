// SaxReader.cpp
//
#include "stdafx.h"
#include "XmlLib\SaxReader.h"
#include "XmlLib\SaxAttributes.h"
#include <sstream>
#include <algorithm>
#include <exception>

using XmlLib::SaxReader;
using XmlLib::SaxString;
using XmlLib::SaxAttributes;

SaxReader::SaxReader(XmlLib::SaxContentElement * root, const SaxString & rootName) :
    m_root(root),
    m_rootName(rootName),
    m_errorReported(false)
{
    HRESULT hr = m_rdr.CreateInstance(__uuidof(MSXML2::SAXXMLReader));
    if (FAILED(hr))
    {
        std::stringstream ss;
        ss << "CreateInstance MSXML2::SAXXMLReader failed: " << hr;
        throw ss.str();
    }
    m_rdr->putContentHandler(this);
    m_rdr->putErrorHandler(this);
}

SaxReader::~SaxReader()
{
}

bool SaxReader::Open(const std::string & pathname)
{
    m_errorReported = false;
    m_errorMessage.erase();

    try
    {
        std::string url = std::string("file:///") + pathname;
        bstr_t wcUrl(url.c_str());
        m_rdr->parseURL((unsigned short *)(wchar_t*)wcUrl);
    }
    catch (_com_error & e)
    {
        HandleComError(e);
    }
    catch (std::exception & e)
    {
        if (!m_errorReported)
        {
            m_errorMessage = e.what();
            m_errorReported = true;
        }
    }
    catch (...)
    {
        // there's always an exception thrown after a failure, need to ignore it
        if (!m_errorReported)
        {
            m_errorMessage = "Unknown exception";
            m_errorReported = true;
        }
    }

    return !m_errorReported;
}

bool SaxReader::ParseText(const std::string & xmlText)
{
    m_errorReported = false;
    m_errorMessage.erase();

    try
    {
        bstr_t wcXml(xmlText.c_str());
        m_rdr->parse(wcXml);
    }
    catch (_com_error & e)
    {
        HandleComError(e);
    }
    catch (std::exception & e)
    {
        if (!m_errorReported)
        {
            m_errorMessage = e.what();
            m_errorReported = true;
        }
    }
    catch (...)
    {
        // there's always an exception thrown after a failure, need to ignore it
        if (!m_errorReported)
        {
            m_errorMessage = "Unknown exception";
            m_errorReported = true;
        }
    }

    return !m_errorReported;
}

bool SaxReader::ReadError() const
{
    return m_errorReported;
}

std::string SaxReader::ErrorMessage() const
{
    return m_errorMessage;
}

// from SaxReaderImpl
void SaxReader::StartElement(
        const SaxString & namespaceUri,
        const SaxString & localName,
        const SaxString & rawName,
        const SaxAttributes & attributes)
{
    try
    {
        if (m_handlers.empty())
        {
            if (rawName == m_rootName)
            {
                // The root is right so handle the document
                m_root->ClearSaxReadErrorHandler(); // shouldn't be needed, but just in case...
                m_root->SaxSetAttributes(attributes); // make sure we read any versioning info etc
                m_handlers.push_back(m_root);
                m_subElementDepth = 0;
            }
            else
            {
                if (!m_errorReported)
                {
                    m_errorReported = true;
                    std::stringstream ss;
                    ss << "Root element name '" << std::string(rawName)
                            << "' does not match expected root element name '" << std::string(m_rootName) << "'";
                    m_errorMessage = ss.str();
                }
            }
        }
        else
        {
            // skip if top handler declined to start a new element last time
            if (m_subElementDepth != 0)
            {
                ++m_subElementDepth;
            }
            else
            {
                SaxContentElementInterface * oldHandler = m_handlers.back();
                SaxContentElementInterface * newHandler = oldHandler->StartElement(rawName, attributes);
                if (newHandler == NULL)
                {
                    m_subElementDepth = 1;
                }
                else
                {
                    m_handlers.push_back(newHandler);
                    // keep a reporting trail back to the top
                    newHandler->SetSaxReadErrorHandler(oldHandler);
                }
            }
        }
    }
    catch (_com_error & e)
    {
        HandleComError(e);
        throw;
    }
    catch (const std::exception & e)
    {
        if (!m_errorReported)
        {
            m_errorMessage = e.what();
            m_errorReported = true;
        }
        throw;
    }
    catch (const std::string & txt)
    {
        if (!m_errorReported)
        {
            m_errorMessage = txt;
            m_errorReported = true;
        }
        throw;
    }
    catch (...)
    {
        throw;
    }
}

void SaxReader::EndElement(
        const SaxString & uri,
        const SaxString & localName,
        const SaxString & rawName)
{
    try
    {
        if (m_subElementDepth == 0 && !m_handlers.empty())
        {
            // end of the handler
            SaxContentElementInterface * oldHandler = m_handlers.back();
            oldHandler->EndElement();
            m_handlers.pop_back();
        }
        else
        {
            --m_subElementDepth;
        }
    }
    catch (_com_error & e)
    {
        HandleComError(e);
        throw;
    }
    catch (const std::exception & e)
    {
        if (!m_errorReported)
        {
            m_errorMessage = e.what();
            m_errorReported = true;
        }
        throw;
    }
    catch (...)
    {
        throw;
    }
}

void SaxReader::Characters(const SaxString & chars)
{
    try
    {
        if (chars.size() > 0)
        {
            if (m_subElementDepth == 0 && !m_handlers.empty())
            {
                // send the chars to the handler
                m_handlers.back()->Characters(chars);
            }
        }
    }
    catch (_com_error & e)
    {
        HandleComError(e);
        throw;
    }
    catch (const std::exception & e)
    {
        if (!m_errorReported)
        {
            m_errorMessage = e.what();
            m_errorReported = true;
        }
        throw;
    }
    catch (...)
    {
        throw;
    }
}

// from SAXErrorHandlerImpl

void SaxReader::Error(
        int lineNumber,
        int columnNumber,
        const SaxString & errorMessage,
        HRESULT errorCode)
{
    if (!m_errorReported)
    {
        m_errorReported = true;
        m_errorMessage = errorMessage;
    }
}

void SaxReader::FatalError(
        int lineNumber,
        int columnNumber,
        const SaxString & errorMessage,
        HRESULT errorCode)
{
    if (!m_errorReported)
    {
        m_errorReported = true;
        m_errorMessage = errorMessage;
    }
}

void SaxReader::HandleComError(const _com_error & e)
{
    if (!m_errorReported)
    {
        if (e.ErrorInfo() != NULL)
        {
            // error description supplied, use it
            m_errorMessage = e.Description();
        }
        else
        {
            // no error info supplied, format directly from the HRESULT
            LPVOID lpMsgBuf;

            ::FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    e.Error(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    reinterpret_cast<LPTSTR>(&lpMsgBuf),
                    0,
                    NULL);
  
            std::string messageText;
            if (lpMsgBuf) //check that ::FormatMessage has not returned NULL
            {
                messageText = LPCSTR(lpMsgBuf);
            }
            else
            {
                std::stringstream ss;
                ss << "Unrecognised error code: 0x" << std::hex << e.Error();
                messageText = ss.str();
            }

            // Free the buffer.
            ::LocalFree(lpMsgBuf);

            // FormatMessage seems to put CRLF on the end of the string. 
            // Strip it off if there is one
            // If there isn't one, rfind will return npos, so substr will give the whole string
            m_errorMessage = messageText.substr(0, messageText.rfind("\r\n"));
        }
        m_errorReported = true;
    }
}
