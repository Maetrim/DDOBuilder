// SaxWriter.h
//
#pragma once

#include "XmlLibExports.h"
#include "SaxString.h"
#include "SaxContentElement.h"
#include "SaxAttributes.h"
#include <list>

#import <msxml3.dll>

namespace CryptLib
{
    class CryptProvider;
}

namespace XmlLib
{
    class SaxWriter
    {
        public:
            SaxWriter();
            virtual ~SaxWriter();

            void StartDocument(const SaxString & rootNodeName);
            void EndDocument();
            void StartElement(const SaxString & nodeName);
            void StartElement(const SaxString & nodeName, const XmlLib::SaxAttributes & attributes);
            void EndElement();
            void Characters(const SaxString & chars);

            bool Open(const std::string & filename);
            std::string Text() const; // produced if written to without opening a file first
            void Close();

            std::string ErrorMessage() const;

            // helper functions

            void WriteEmptyElement(const SaxString & elementName);
            void WriteComment(const SaxString & comment);

            template <typename T>
            void WriteSimpleElement(const SaxString & elementName, const T & t)
            {
                StartElement(elementName);
                std::stringstream ss;
                ss << t;
                Characters(SaxString(ss.str()));
                EndElement();
            }

            template <>
            void WriteSimpleElement(const SaxString & elementName, const std::string & t);

            void WriteSimpleElement(const SaxString & elementName, double t, size_t precision);

            template <typename T>
            void WriteEnumElement(const SaxString & elementName, const T & t, const enumMapEntry<T> * m)
            {
                StartElement(elementName);
                const enumMapEntry<T> * p;
                for (p = m; p->name != NULL; ++p)
                {
                    if (p->value == t)
                    {
                        break;
                    }
                }
                if (p->name == NULL) // stopped at end of list
                {
                    throw "WriteEnumElement failed to find entry";
                }
                Characters(p->name);
                EndElement();
            }

        private:
            // write to file
            void OutputElementStart(const std::wstring & header);
            void OutputElementEnd(const std::wstring & name);
            void OutputEmptyElement(const std::wstring & header);
            void OutputCharacters(const std::wstring & chars);

            void Write(const char * text, int length);

            std::vector<std::wstring> m_elementStack;

            std::wstring m_imxElementHeader;
            bool m_imxEmptyElement;
            HANDLE m_file;
            std::ostringstream m_ostream;

            mutable std::string m_errorMessage;
    };

    template <>
    void SaxWriter::WriteSimpleElement(const SaxString & elementName, const std::string & t)
    {
        StartElement(elementName);
        Characters(SaxString(t));
        EndElement();
    }
}
