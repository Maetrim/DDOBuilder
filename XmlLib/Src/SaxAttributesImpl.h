// SaxAttributesImpl.h
//
#pragma once

#import <msxml3.dll>
#include "XmlLib\SaxAttributes.h"

namespace XmlLib
{
    class SaxAttributesImpl :
        public MSXML2::ISAXAttributes
    {
        public:
            SaxAttributesImpl(const XmlLib::SaxAttributes & attr);

            STDMETHOD(QueryInterface)(const struct _GUID &,void ** );
            STDMETHOD_(ULONG, AddRef)();
            STDMETHOD_(ULONG, Release)();

            virtual HRESULT __stdcall raw_getLength (
                int * pnLength );
            virtual HRESULT __stdcall raw_getURI (
                int nIndex,
                unsigned short * * ppwchUri,
                int * pcchUri );
            virtual HRESULT __stdcall raw_getLocalName (
                int nIndex,
                unsigned short * * ppwchLocalName,
                int * pcchLocalName );
            virtual HRESULT __stdcall raw_getQName (
                int nIndex,
                unsigned short * * ppwchQName,
                int * pcchQName );
            virtual HRESULT __stdcall raw_getName (
                int nIndex,
                unsigned short * * ppwchUri,
                int * pcchUri,
                unsigned short * * ppwchLocalName,
                int * pcchLocalName,
                unsigned short * * ppwchQName,
                int * pcchQName );
            virtual HRESULT __stdcall raw_getIndexFromName (
                unsigned short * pwchUri,
                int cchUri,
                unsigned short * pwchLocalName,
                int cchLocalName,
                int * pnIndex );
            virtual HRESULT __stdcall raw_getIndexFromQName (
                unsigned short * pwchQName,
                int cchQName,
                int * pnIndex );
            virtual HRESULT __stdcall raw_getType (
                int nIndex,
                unsigned short * * ppwchType,
                int * pcchType );
            virtual HRESULT __stdcall raw_getTypeFromName (
                unsigned short * pwchUri,
                int cchUri,
                unsigned short * pwchLocalName,
                int cchLocalName,
                unsigned short * * ppwchType,
                int * pcchType );
            virtual HRESULT __stdcall raw_getTypeFromQName (
                unsigned short * pwchQName,
                int cchQName,
                unsigned short * * ppwchType,
                int * pcchType );
            virtual HRESULT __stdcall raw_getValue (
                int nIndex,
                unsigned short * * ppwchValue,
                int * pcchValue );
            virtual HRESULT __stdcall raw_getValueFromName (
                unsigned short * pwchUri,
                int cchUri,
                unsigned short * pwchLocalName,
                int cchLocalName,
                unsigned short * * ppwchValue,
                int * pcchValue );
            virtual HRESULT __stdcall raw_getValueFromQName (
                unsigned short * pwchQName,
                int cchQName,
                unsigned short * * ppwchValue,
                int * pcchValue );
        private:
            const XmlLib::SaxAttributes & m_attr;
    };
}
