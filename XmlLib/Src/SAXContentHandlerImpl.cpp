// SAXContentHandlerImpl.cpp: implementation of the SAXContentHandlerImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlLib\SAXContentHandlerImpl.h"
#include "XmlLib\SaxAttributes.h"

using XmlLib::SAXContentHandlerImpl;
using XmlLib::SaxAttributes;


SAXContentHandlerImpl::SAXContentHandlerImpl()
{
}

SAXContentHandlerImpl::~SAXContentHandlerImpl()
{
}



STDMETHODIMP SAXContentHandlerImpl::raw_putDocumentLocator( 
        MSXML2::ISAXLocator __RPC_FAR *pLocator)
{
    return S_OK;
}
        
STDMETHODIMP SAXContentHandlerImpl::raw_startDocument()
{
    HRESULT hr = S_OK;
    try
    {
        StartDocument();
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        

        
STDMETHODIMP SAXContentHandlerImpl::raw_endDocument()
{
    HRESULT hr = S_OK;
    try
    {
        EndDocument();
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        
        
STDMETHODIMP SAXContentHandlerImpl::raw_startPrefixMapping( 
        unsigned short __RPC_FAR *pwchPrefix,
        int cchPrefix,
        unsigned short __RPC_FAR *pwchUri,
        int cchUri)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString prefix(CWCHARTPTR(pwchPrefix), cchPrefix);
        SaxString uri(CWCHARTPTR(pwchUri), cchUri);
        StartPrefixMapping(prefix, uri);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        
        
STDMETHODIMP SAXContentHandlerImpl::raw_endPrefixMapping( 
        unsigned short __RPC_FAR *pwchPrefix,
        int cchPrefix)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString prefix(CWCHARTPTR(pwchPrefix), cchPrefix);
        EndPrefixMapping(prefix);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        

        
STDMETHODIMP SAXContentHandlerImpl::raw_startElement( 
        unsigned short __RPC_FAR *pwchNamespaceUri,
        int cchNamespaceUri,
        unsigned short __RPC_FAR *pwchLocalName,
        int cchLocalName,
        unsigned short __RPC_FAR *pwchRawName,
        int cchRawName,
        MSXML2::ISAXAttributes __RPC_FAR *pAttributes)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString namespaceUri(CWCHARTPTR(pwchNamespaceUri), cchNamespaceUri);
        SaxString localName(CWCHARTPTR(pwchLocalName), cchLocalName);
        SaxString rawName(CWCHARTPTR(pwchRawName), cchRawName);
        SaxAttributes attributes;
        HRESULT hr = S_OK;
        int numAttributes = pAttributes->getLength();
        for (int i=0; i<numAttributes; i++ )
        {
            wchar_t * wcName;
            int lenName;
            pAttributes->getLocalName(i, USHORTPTRPTR(&wcName), &lenName); 
            SaxString name(wcName, lenName);
            wchar_t * wcValue;
            int lenValue;
            pAttributes->getValue(i, USHORTPTRPTR(&wcValue), &lenValue);
            SaxString value(wcValue, lenValue);
            attributes[name] = value;
        }

        StartElement(namespaceUri, localName, rawName, attributes);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        
       
STDMETHODIMP SAXContentHandlerImpl::raw_endElement( 
        unsigned short __RPC_FAR *pwchNamespaceUri,
        int cchNamespaceUri,
        unsigned short __RPC_FAR *pwchLocalName,
        int cchLocalName,
        unsigned short __RPC_FAR *pwchRawName,
        int cchRawName)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString namespaceUri(CWCHARTPTR(pwchNamespaceUri), cchNamespaceUri);
        SaxString localName(CWCHARTPTR(pwchLocalName), cchLocalName);
        SaxString rawName(CWCHARTPTR(pwchRawName), cchRawName);
        EndElement(namespaceUri, localName, rawName);
    }
    catch (...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        
STDMETHODIMP SAXContentHandlerImpl::raw_characters( 
        unsigned short __RPC_FAR *pwchChars,
        int cchChars)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString chars(CWCHARTPTR(pwchChars), cchChars);
        Characters(chars);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        

STDMETHODIMP SAXContentHandlerImpl::raw_ignorableWhitespace( 
        unsigned short __RPC_FAR *pwchChars,
        int cchChars)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString chars(CWCHARTPTR(pwchChars), cchChars);
        IgnorableWhitespace(chars);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        

STDMETHODIMP SAXContentHandlerImpl::raw_processingInstruction( 
        unsigned short __RPC_FAR *pwchTarget,
        int cchTarget,
        unsigned short __RPC_FAR *pwchData,
        int cchData)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString target(CWCHARTPTR(pwchTarget), cchTarget);
        SaxString data(CWCHARTPTR(pwchData), cchData);
        ProcessingInstruction(target, data);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        
        
STDMETHODIMP SAXContentHandlerImpl::raw_skippedEntity( 
        unsigned short __RPC_FAR *pwchVal,
        int cchVal)
{
    HRESULT hr = S_OK;
    try
    {
        SaxString val(CWCHARTPTR(pwchVal), cchVal);
        SkippedEntity(val);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}


STDMETHODIMP SAXContentHandlerImpl::QueryInterface(const struct _GUID &,void **)
{
    // hack-hack-hack!
    return 0;
}

STDMETHODIMP_(unsigned long) SAXContentHandlerImpl::AddRef()
{
    // hack-hack-hack!
    return 0;
}

STDMETHODIMP_(unsigned long) SAXContentHandlerImpl::Release()
{
    // hack-hack-hack!
    return 0;
}
