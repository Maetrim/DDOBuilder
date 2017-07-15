// SaxWriter.cpp
//
#include "stdafx.h"
#include "XmlLib\SaxWriter.h"
#include "XmlLib\SaxAttributes.h"
#include <algorithm>
#include <string>

using XmlLib::SaxWriter;
using XmlLib::SaxString;
using XmlLib::SaxAttributes;

namespace
{
    // All characters with an int value >= 128 must be mapped and replaced with special
    // characters when writing otherwise the XML sax reader will not understand them.

    // Lookup table starts at 0x0080
    int f_characterTranslate = 0x0080;

}

SaxWriter::SaxWriter() :
    m_imxEmptyElement(false),
    m_file(INVALID_HANDLE_VALUE)
{
}

SaxWriter::~SaxWriter()
{
    if (m_file != INVALID_HANDLE_VALUE)
    {
        // file handle was not closed correctly
        // correctly cleanup to stop files being "locked" as they are
        // opened in exclusive mode
        CloseHandle(m_file);
        m_file = INVALID_HANDLE_VALUE;
    }
}

void SaxWriter::StartDocument(const SaxString & rootNodeName)
{
    SaxAttributes attributes;
    StartElement(rootNodeName, attributes);
}

void SaxWriter::Close()
{
    if (m_file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_file);
        m_file = INVALID_HANDLE_VALUE;
    }
}

void SaxWriter::EndDocument()
{
    EndElement();

    Close();
}

void SaxWriter::StartElement(const SaxString & nodeName)
{
    static SaxAttributes emptyAttributes;
    StartElement(nodeName, emptyAttributes);
}

void SaxWriter::StartElement(const SaxString & nodeName, const SaxAttributes & attributes)
{
    SaxAttributes fullAttributes = attributes;

    std::wstring id; // must survive until attributes written as SaxString points to buffer

    if (m_imxEmptyElement) // last element needs to be started
    {
        OutputElementStart(m_imxElementHeader);
    }
    m_imxEmptyElement = true; // starting a new element
    m_imxElementHeader = nodeName;
    m_imxElementHeader += fullAttributes.Format();
    m_elementStack.push_back(nodeName);
}

void SaxWriter::EndElement()
{
    if (m_imxEmptyElement) // last element needs to be started
    {
        OutputEmptyElement(m_imxElementHeader.c_str());
        m_imxEmptyElement = false; // no current unwritten element
    }
    else
    {
        OutputElementEnd(m_elementStack.back());
    }
    // that's the end of that element
    m_elementStack.pop_back();
}

void SaxWriter::Characters(const SaxString & chars)
{
    if (chars.size() > 0)
    {
        if (m_imxEmptyElement) // last element needs to be started
        {
            OutputElementStart(m_imxElementHeader);
            m_imxEmptyElement = false; // no current unwritten element
        }
        OutputCharacters(chars);
    }
}

bool SaxWriter::Open(const std::string & filename)
{
    m_file = ::CreateFile(
            filename.c_str(),
            GENERIC_WRITE,
            0, // no sharing
            NULL, // no special security
            CREATE_ALWAYS, // overwrite if exists
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    if (m_file == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        LPVOID lpMsgBuf;

        ::FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                error,
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
            ss << "Unrecognised error code: 0x" << std::hex << error;
            messageText = ss.str();
        }

        // Free the buffer.
        ::LocalFree(lpMsgBuf);

        // FormatMessage seems to put CRLF on the end of the string. 
        // Strip it off if there is one
        // If there isn't one, rfind will return npos, so substr will give the whole string
        m_errorMessage = messageText.substr(0, messageText.rfind("\r\n"));
    }

    return (m_file != INVALID_HANDLE_VALUE);
}

std::string SaxWriter::Text() const
{
    return m_ostream.str();
}

std::string SaxWriter::ErrorMessage() const
{
    return m_errorMessage;
}

void SaxWriter::WriteEmptyElement(const SaxString & elementName)
{
    StartElement(elementName);
    EndElement();
}

void SaxWriter::WriteComment(const SaxString & comment)
{
    std::string commentStart("<!--");
    Write(commentStart.c_str(), commentStart.length());
    Characters(comment);
    std::string commentEnd("-->");
    Write(commentEnd.c_str(), commentEnd.length());
}

void SaxWriter::WriteSimpleElement(const SaxString & elementName, double t, size_t precision)
{
    StartElement(elementName);
    const size_t formatSize = 10;
    char format[formatSize];
    const size_t valueSize = 50;
    char value[valueSize];
    sprintf_s(format, formatSize, "%%.%uf", precision);
    sprintf_s(value, valueSize, format, t);
    Characters(SaxString(value));
    EndElement();
}

void SaxWriter::OutputElementStart(const std::wstring & header)
{
    std::string toWrite;
    toWrite = "<";
    toWrite += bstr_t(header.c_str());
    toWrite += ">";
    Write(toWrite.c_str(), toWrite.size());
}

void SaxWriter::OutputElementEnd(const std::wstring & name)
{
    std::string toWrite;
    toWrite = "</";
    toWrite += bstr_t(name.c_str());
    toWrite += ">";
    Write(toWrite.c_str(), toWrite.size());
}

void SaxWriter::OutputEmptyElement(const std::wstring & header)
{
    std::string toWrite;
    toWrite = "<";
    toWrite += bstr_t(header.c_str());
    toWrite += "/>";
    Write(toWrite.c_str(), toWrite.size());
}

void SaxWriter::OutputCharacters(const std::wstring & chars)
{
    // convert special XML chars
    // > &gt;
    // < &lt;
    // " &quot; not used by MSXML so not changed here
    // ' &apos; not used by MSXML so not changed here
    // & &amp;

    std::string toWrite;
    for (std::wstring::const_iterator iter = chars.begin(); iter != chars.end(); iter++)
    {
        // In the normal character map
        if (*iter < f_characterTranslate)
        {
            if (*iter == '\x0A')
            {
                // Carriage return line feed
                toWrite += '\x0D';
                toWrite += '\x0A';
            }
            else if (*iter == '>')
            {
                toWrite += "&gt;";
            }
            else if (*iter == '<')
            {
                toWrite += "&lt;";
            }
            else if (*iter == '&')
            {
                toWrite += "&amp;";
            }
            else if (*iter == '\x09' || *iter > '\x1F')
            {
                // Tab and other chars included
                toWrite += char(*iter);
            }
        }
        else
        {
            // non standard characters are written out as "&#<asciicode>;"
            // e.g. °C if "&#156;C"
            std::stringstream ss;
            ss << "&#" << *iter << ";";
            // Get it out of the lookup table
            toWrite += ss.str();
        }
    }

    Write(toWrite.c_str(), toWrite.size());
}

void SaxWriter::Write(const char * text, int length)
{
    if (m_file == INVALID_HANDLE_VALUE)
    {
        m_ostream.write(text, length);
    }
    else
    {
        DWORD lenWritten = 0;
        BOOL success = ::WriteFile(
                m_file,
                text,
                length,
                &lenWritten,
                NULL);
        if (!success)
        {
            DWORD error = GetLastError();
            LPVOID lpMsgBuf;

            ::FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    error,
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
                ss << "Unrecognised error code: 0x" << std::hex << error;
                messageText = ss.str();
            }

            // Free the buffer.
            ::LocalFree(lpMsgBuf);

            // FormatMessage seems to put CRLF on the end of the string. 
            // Strip it off if there is one
            // If there isn't one, rfind will return npos, so substr will give the whole string
            m_errorMessage = messageText.substr(0, messageText.rfind("\r\n"));
            throw m_errorMessage;
        }
    }
}
