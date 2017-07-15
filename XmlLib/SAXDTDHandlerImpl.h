// SAXDTDHandlerImpl.h
//
// Based on sample code from MSDN

#pragma once

#import <msxml3.dll>

#include "XmlLibExports.h"
namespace XmlLib
{
    class SAXDTDHandlerImpl :
        public MSXML2::ISAXDTDHandler  
    {
        public:
            SAXDTDHandlerImpl();
            virtual ~SAXDTDHandlerImpl();

            // Calls to be overridden by derived classes, C++ friendly



            // IUnknown interface
            // This must be correctly implemented id using COM, but this class doesn't
            STDMETHOD(QueryInterface)(const struct _GUID &,void **);
            STDMETHOD_(unsigned long, AddRef)(void);
            STDMETHOD_(unsigned long, Release)(void);
            // ISAXDTDHandler interface
            STDMETHOD(raw_notationDecl)( 
                    wchar_t __RPC_FAR *pwchName,
                    int cchName,
                    wchar_t __RPC_FAR *pwchPublicId,
                    int cchPublicId,
                    wchar_t __RPC_FAR *pwchSystemId,
                    int cchSystemId);
            STDMETHOD(raw_unparsedEntityDecl)( 
                    wchar_t __RPC_FAR *pwchName,
                    int cchName,
                    wchar_t __RPC_FAR *pwchPublicId,
                    int cchPublicId,
                    wchar_t __RPC_FAR *pwchSystemId,
                    int cchSystemId,
                    wchar_t __RPC_FAR *pwchNotationName,
                    int cchNotationName);
    };
}
