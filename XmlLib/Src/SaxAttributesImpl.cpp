// SaxAttributesImpl.cpp
//
#include "stdafx.h"
#include "SaxAttributesImpl.h"

using XmlLib::SaxAttributesImpl;
using XmlLib::SaxString;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

SaxAttributesImpl::SaxAttributesImpl(const XmlLib::SaxAttributes & attr) :
    m_attr(attr)
{
}


STDMETHODIMP SaxAttributesImpl::QueryInterface(const struct _GUID & riid,void ** ppvObject)
{
    if (riid == __uuidof(IUnknown) || riid == __uuidof(MSXML2::ISAXAttributes))
    {
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG) SaxAttributesImpl::AddRef()
{
    return 1;
}

STDMETHODIMP_(ULONG) SaxAttributesImpl::Release()
{
    return 1;
}


HRESULT SaxAttributesImpl::raw_getLength(
        int * pnLength)
{
    HRESULT hr = S_OK;
    if (pnLength)
    {
        *pnLength = m_attr.Size();
    }
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getURI(
        int nIndex,
        unsigned short * * ppwchUri,
        int * pcchUri )
{
    HRESULT hr = S_OK;
    if (nIndex >= int(m_attr.Size()))
    {
        hr = E_INVALIDARG;
    }
    else
    {
        // this class doesn't support URIs on attributes
        *ppwchUri = USHORTPTR(L"");
        *pcchUri = 0;
    }
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getLocalName (
        int nIndex,
        unsigned short * * ppwchLocalName,
        int * pcchLocalName )
{
    HRESULT hr = S_OK;
    if (nIndex >= int(m_attr.Size()))
    {
        hr = E_INVALIDARG;
    }
    else
    {
        *ppwchLocalName = USHORTPTR(const_cast<wchar_t*>(m_attr.Key(nIndex).c_str()));
        *pcchLocalName = m_attr.Key(nIndex).size();
    }
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getQName (
        int nIndex,
        unsigned short * * ppwchQName,
        int * pcchQName )
{
    HRESULT hr = S_OK;
    if (nIndex >= int(m_attr.Size()))
    {
        hr = E_INVALIDARG;
    }
    else
    {
        *ppwchQName = USHORTPTR(const_cast<wchar_t*>(m_attr.Key(nIndex).c_str()));
        *pcchQName = m_attr.Key(nIndex).size();
    }
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getName (
        int nIndex,
        unsigned short * * ppwchUri,
        int * pcchUri,
        unsigned short * * ppwchLocalName,
        int * pcchLocalName,
        unsigned short * * ppwchQName,
        int * pcchQName )
{
    HRESULT hr = S_OK;
    if (nIndex >= int(m_attr.Size()))
    {
        hr = E_INVALIDARG;
    }
    else
    {
        // this class doesn't support URIs on attributes
        *ppwchUri = USHORTPTR(L"");
        *pcchUri = 0;
        *ppwchLocalName = USHORTPTR(const_cast<wchar_t*>(m_attr.Key(nIndex).c_str()));
        *pcchLocalName = m_attr.Key(nIndex).size();
        *ppwchQName = USHORTPTR(const_cast<wchar_t*>(m_attr.Key(nIndex).c_str()));
        *pcchQName = m_attr.Key(nIndex).size();
    }
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getIndexFromName (
        unsigned short * pwchUri,
        int cchUri,
        unsigned short * pwchLocalName,
        int cchLocalName,
        int * pnIndex )
{
    HRESULT hr = E_INVALIDARG;
 
    SaxString searchKey(CWCHARTPTR(pwchLocalName), CWCHARTPTR(cchLocalName));
    for (size_t index = 0; index < m_attr.Size(); ++index)
    {
        if (m_attr.Key(index) == searchKey)
        {
            *pnIndex = index;
            hr = S_OK;
            break;
        }
    }

    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getIndexFromQName (
        unsigned short * pwchQName,
        int cchQName,
        int * pnIndex )
{
    HRESULT hr = E_INVALIDARG;

    SaxString searchKey(CWCHARTPTR(pwchQName), CWCHARTPTR(cchQName));
    for (size_t index = 0; index < m_attr.Size(); ++index)
    {
        if (m_attr.Key(index) == searchKey)
        {
            *pnIndex = index;
            hr = S_OK;
            break;
        }
    }

    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getType (
        int nIndex,
        unsigned short * * ppwchType,
        int * pcchType )
{
    HRESULT hr = S_OK;
    if (nIndex >= int(m_attr.Size()))
    {
        hr = E_INVALIDARG;
    }
    else
    {
        // this class doesn't support types on attributes
        *ppwchType = USHORTPTR(L"");
        *pcchType = 0;
    }
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getTypeFromName (
        unsigned short * pwchUri,
        int cchUri,
        unsigned short * pwchLocalName,
        int cchLocalName,
        unsigned short * * ppwchType,
        int * pcchType )
{
    HRESULT hr = S_OK;
    // this class doesn't support types on attributes
    *ppwchType = USHORTPTR(L"");
    *pcchType = 0;
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getTypeFromQName (
        unsigned short * pwchQName,
        int cchQName,
        unsigned short * * ppwchType,
        int * pcchType )
{
    HRESULT hr = S_OK;
    // this class doesn't support types on attributes
    *ppwchType = USHORTPTR(L"");
    *pcchType = 0;
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getValue (
        int nIndex,
        unsigned short * * ppwchValue,
        int * pcchValue )
{
    HRESULT hr = S_OK;
    if (nIndex >= int(m_attr.Size()))
    {
        hr = E_INVALIDARG;
    }
    else
    {
        *ppwchValue = USHORTPTR(const_cast<wchar_t*>(m_attr.Value(nIndex).c_str()));
        *pcchValue = m_attr.Value(nIndex).size();
    }
    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getValueFromName (
        unsigned short * pwchUri,
        int cchUri,
        unsigned short * pwchLocalName,
        int cchLocalName,
        unsigned short * * ppwchValue,
        int * pcchValue )
{
    HRESULT hr = E_INVALIDARG;

    SaxString searchKey(CWCHARTPTR(pwchLocalName), CWCHARTPTR(cchLocalName));
    for (size_t index = 0; index < m_attr.Size(); ++index)
    {
        if (m_attr.Key(index) == searchKey)
        {
            *ppwchValue = USHORTPTR(const_cast<wchar_t*>(m_attr.Value(index).c_str()));
            *pcchValue = m_attr.Value(index).size();
            hr = S_OK;
            break;
        }
    }

    return hr;
}

STDMETHODIMP SaxAttributesImpl::raw_getValueFromQName (
        unsigned short * pwchQName,
        int cchQName,
        unsigned short * * ppwchValue,
        int * pcchValue )
{
    HRESULT hr = E_INVALIDARG;

    SaxString searchKey(CWCHARTPTR(pwchQName), CWCHARTPTR(cchQName));
    for (size_t index = 0; index < m_attr.Size(); ++index)
    {
        if (m_attr.Key(index) == searchKey)
        {
            *ppwchValue = USHORTPTR(const_cast<wchar_t*>(m_attr.Value(index).c_str()));
            *pcchValue = m_attr.Value(index).size();
            hr = S_OK;
            break;
        }
    }

    return hr;
}
