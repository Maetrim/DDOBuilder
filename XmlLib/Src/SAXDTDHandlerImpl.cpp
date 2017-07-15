// SAXDTDHandlerImpl.cpp

#include "stdafx.h"
#include "XmlLib\SAXDTDHandlerImpl.h"

using XmlLib::SAXDTDHandlerImpl;


SAXDTDHandlerImpl::SAXDTDHandlerImpl()
{
}

SAXDTDHandlerImpl::~SAXDTDHandlerImpl()
{
}


STDMETHODIMP SAXDTDHandlerImpl::raw_notationDecl( 
        wchar_t __RPC_FAR *pwchName,
        int cchName,
        wchar_t __RPC_FAR *pwchPublicId,
        int cchPublicId,
        wchar_t __RPC_FAR *pwchSystemId,
        int cchSystemId)
{
    return S_OK;
}
        
STDMETHODIMP SAXDTDHandlerImpl::raw_unparsedEntityDecl( 
        wchar_t __RPC_FAR *pwchName,
        int cchName,
        wchar_t __RPC_FAR *pwchPublicId,
        int cchPublicId,
        wchar_t __RPC_FAR *pwchSystemId,
        int cchSystemId,
        wchar_t __RPC_FAR *pwchNotationName,
        int cchNotationName)
{
    return S_OK;
}


STDMETHODIMP SAXDTDHandlerImpl::QueryInterface(const struct _GUID &,void **)
{
    // hack-hack-hack!
    return 0;
}

STDMETHODIMP_(unsigned long) SAXDTDHandlerImpl::AddRef()
{
    // hack-hack-hack!
    return 0;
}

STDMETHODIMP_(unsigned long) SAXDTDHandlerImpl::Release()
{
    // hack-hack-hack!
    return 0;
}