// SAXContentHandlerImpl.h
//
// Based on sample code from MSDN

#pragma once

#import <msxml3.dll>

#include "XmlLibExports.h"
#include "SaxString.h"


namespace XmlLib
{
    class SaxAttributes;
    class SaxString;

    class SAXContentHandlerImpl :
        public MSXML2::ISAXContentHandler  
    {
        public:
            SAXContentHandlerImpl();
            virtual ~SAXContentHandlerImpl();

            // Calls to be overridden by derived classes, C++ friendly

            virtual void StartDocument(
                    ) {}
            virtual void EndDocument(
                    ) {}
            virtual void StartPrefixMapping(
                    const SaxString & ,
                    const SaxString & 
                    ) {}
            virtual void EndPrefixMapping(
                    const SaxString & 
                    ) {}
            virtual void StartElement(
                    const SaxString & ,
                    const SaxString & ,
                    const SaxString & ,
                    const SaxAttributes & 
                    ) {}
            virtual void EndElement(
                    const SaxString & ,
                    const SaxString & ,
                    const SaxString & 
                    ) {}
            virtual void Characters(
                    const SaxString & 
                    ) {}
            virtual void IgnorableWhitespace(
                    const SaxString & 
                    ) {}
            virtual void ProcessingInstruction(
                    const SaxString & ,
                    const SaxString & 
                    ) {}
            virtual void SkippedEntity(
                    const SaxString & 
                    ) {}

            // IUnknown interface
            // This must be correctly implemented id using COM, but this class doesn't
            STDMETHOD(QueryInterface)(const struct _GUID &,void **);
            STDMETHOD_(unsigned long, AddRef)(void);
            STDMETHOD_(unsigned long, Release)(void);
            // ISAXContentHandler interface
            STDMETHOD(raw_putDocumentLocator)( 
                    MSXML2::ISAXLocator __RPC_FAR *pLocator);
            STDMETHOD(raw_startDocument)();
            STDMETHOD(raw_endDocument)();
            STDMETHOD(raw_startPrefixMapping)( 
                    unsigned short __RPC_FAR *pwchPrefix,
                    int cchPrefix,
                    unsigned short __RPC_FAR *pwchUri,
                    int cchUri);
            STDMETHOD(raw_endPrefixMapping)( 
                    unsigned short __RPC_FAR *pwchPrefix,
                    int cchPrefix);
            STDMETHOD(raw_startElement)( 
                    unsigned short __RPC_FAR *pwchNamespaceUri,
                    int cchNamespaceUri,
                    unsigned short __RPC_FAR *pwchLocalName,
                    int cchLocalName,
                    unsigned short __RPC_FAR *pwchRawName,
                    int cchRawName,
                    MSXML2::ISAXAttributes __RPC_FAR *pAttributes);
            STDMETHOD(raw_endElement)( 
                    unsigned short __RPC_FAR *pwchNamespaceUri,
                    int cchNamespaceUri,
                    unsigned short __RPC_FAR *pwchLocalName,
                    int cchLocalName,
                    unsigned short __RPC_FAR *pwchRawName,
                    int cchRawName);
            STDMETHOD(raw_characters)( 
                    unsigned short __RPC_FAR *pwchChars,
                    int cchChars);
            STDMETHOD(raw_ignorableWhitespace)( 
                    unsigned short __RPC_FAR *pwchChars,
                    int cchChars);
            STDMETHOD(raw_processingInstruction)( 
                    unsigned short __RPC_FAR *pwchTarget,
                    int cchTarget,
                    unsigned short __RPC_FAR *pwchData,
                    int cchData);
            STDMETHOD(raw_skippedEntity)( 
                    unsigned short __RPC_FAR *pwchName,
                    int cchName);
    };
}
