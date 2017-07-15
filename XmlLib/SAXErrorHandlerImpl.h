// SAXErrorHandlerImpl.h
//
// Based on sample code from MSDN

#pragma once

#import <msxml3.dll>

#include "XmlLibExports.h"
#include "SaxString.h"

namespace XmlLib
{
    class SAXErrorHandlerImpl :
        public MSXML2::ISAXErrorHandler  
    {
        public:
            SAXErrorHandlerImpl();
            virtual ~SAXErrorHandlerImpl();

            // Calls to be overridden by derived classes, C++ friendly

            virtual void Error(
                int ,
                int ,
                const SaxString & ,
                HRESULT ) {}
            virtual void FatalError(
                int ,
                int ,
                const SaxString & ,
                HRESULT ) {}
            virtual void IgnorableWarning(
                int ,
                int ,
                const SaxString & ,
                HRESULT ) {}

            // IUnknown interface
            // This must be correctly implemented id using COM, but this class doesn't
            STDMETHOD(QueryInterface)(const struct _GUID &,void **);
            STDMETHOD_(unsigned long, AddRef)(void);
            STDMETHOD_(unsigned long, Release)(void);
            // ISAXErrorHandler interface
            STDMETHOD(raw_error)( 
                    MSXML2::ISAXLocator __RPC_FAR *pLocator,
                    unsigned short * pwchErrorMessage,
                    HRESULT errCode);
            STDMETHOD(raw_fatalError)( 
                    MSXML2::ISAXLocator __RPC_FAR *pLocator,
                    unsigned short * pwchErrorMessage,
                    HRESULT errCode);
            STDMETHOD(raw_ignorableWarning)( 
                    MSXML2::ISAXLocator __RPC_FAR *pLocator,
                    unsigned short * pwchErrorMessage,
                    HRESULT errCode);
    };
}
