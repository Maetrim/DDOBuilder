// SAXErrorHandlerImpl.cpp
//

#include "stdafx.h"
#include "XmlLib\SAXErrorHandlerImpl.h"

using XmlLib::SAXErrorHandlerImpl;


SAXErrorHandlerImpl::SAXErrorHandlerImpl()
{

}

SAXErrorHandlerImpl::~SAXErrorHandlerImpl()
{

}

STDMETHODIMP SAXErrorHandlerImpl::raw_error( 
        MSXML2::ISAXLocator __RPC_FAR *pLocator,
        unsigned short * pwchErrorMessage,
        HRESULT errCode)
{
    HRESULT hr = S_OK;
    try
    {
        int lineNumber = 0;
        int columnNumber = 0;
        if (pLocator != NULL)
        {
            lineNumber = pLocator->getLineNumber();
            columnNumber = pLocator->getColumnNumber();
        }
        SaxString errorMessage = CWCHARTPTR(pwchErrorMessage);
        Error(lineNumber, columnNumber, errorMessage, errCode);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        
STDMETHODIMP SAXErrorHandlerImpl::raw_fatalError( 
        MSXML2::ISAXLocator __RPC_FAR *pLocator,
        unsigned short * pwchErrorMessage,
        HRESULT errCode)
{
    HRESULT hr = S_OK;
    try
    {
        int lineNumber = 0;
        int columnNumber = 0;
        if (pLocator != NULL)
        {
            lineNumber = pLocator->getLineNumber();
            columnNumber = pLocator->getColumnNumber();
        }
        SaxString errorMessage = CWCHARTPTR(pwchErrorMessage);
        FatalError(lineNumber, columnNumber, errorMessage, errCode);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}
        
STDMETHODIMP SAXErrorHandlerImpl::raw_ignorableWarning( 
        MSXML2::ISAXLocator __RPC_FAR *pLocator,
        unsigned short * pwchErrorMessage,
        HRESULT errCode)
{
    HRESULT hr = S_OK;
    try
    {
        int lineNumber = 0;
        int columnNumber = 0;
        if (pLocator != NULL)
        {
            lineNumber = pLocator->getLineNumber();
            columnNumber = pLocator->getColumnNumber();
        }
        SaxString errorMessage = CWCHARTPTR(pwchErrorMessage);
        IgnorableWarning(lineNumber, columnNumber, errorMessage, errCode);
    }
    catch(...)
    {
        hr = E_FAIL; // abort parse
    }
    return hr;
}

STDMETHODIMP SAXErrorHandlerImpl::QueryInterface(const struct _GUID &,void **)
{
    // hack-hack-hack!
    return 0;
}

STDMETHODIMP_(unsigned long) SAXErrorHandlerImpl::AddRef()
{
    // hack-hack-hack!
    return 0;
}

STDMETHODIMP_(unsigned long) SAXErrorHandlerImpl::Release()
{
    // hack-hack-hack!
    return 0;
}
