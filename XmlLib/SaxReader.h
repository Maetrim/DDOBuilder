// SaxReader.h
//
#pragma once

#include "XmlLibExports.h"
#include "SAXContentHandlerImpl.h"
#include "SAXErrorHandlerImpl.h"
#include "SaxContentElement.h"
#include "SaxString.h"
#include <vector>

namespace CryptLib
{
    class CryptProvider;
}

namespace XmlLib
{
    class SaxReader :
        public SAXContentHandlerImpl,
        public SAXErrorHandlerImpl
    {
        public:

            SaxReader(SaxContentElement * root, const SaxString & rootName);
            virtual ~SaxReader();

            bool Open(const std::string & pathname);
            bool ParseText(const std::string & xmlText);

            bool ReadError() const;
            std::string ErrorMessage() const;

            // from SAXContentHandlerImpl
            void StartElement(
                    const SaxString & namespaceUri,
                    const SaxString & localName,
                    const SaxString & rawName,
                    const SaxAttributes & attributes);
            void EndElement(
                    const SaxString & uri,
                    const SaxString & localName,
                    const SaxString & rawName);
            void Characters(
                    const SaxString & chars);

            // from SAXErrorHandlerImpl
            void Error(
                    int lineNumber,
                    int columnNumber,
                    const SaxString & errorMessage,
                    HRESULT errorCode);
            void FatalError(
                    int lineNumber,
                    int columnNumber,
                    const SaxString & errorMessage,
                    HRESULT errorCode);

        private:
            void HandleComError(const _com_error & e);
            
            // the address and name of the root entity - used as the handler unless the
            // name does not match in which case parsing is aborted and it's an error
            SaxContentElement * m_root;
            SaxString m_rootName;

            // this holds the stack of element handlers
            // start element pushes and end element pops
            std::vector<SaxContentElementInterface *> m_handlers;

            // if an element is not being handled this counts the depth of sub-elements
            /// so that the handling can restart when the skipped element ends
            size_t m_subElementDepth;

            bool m_errorReported;
            std::string m_errorMessage;

            MSXML2::ISAXXMLReaderPtr m_rdr;
    };
}
