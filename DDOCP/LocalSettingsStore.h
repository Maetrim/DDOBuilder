#pragma once
#include "afxsettingsstore.h"
#include <string>

class CLocalSettingsStore :
    public CSettingsStore
{
    public:
        DECLARE_DYNCREATE(CLocalSettingsStore)
        CLocalSettingsStore();
        ~CLocalSettingsStore();

        std::string IniFilename();

        virtual BOOL CreateKey(LPCTSTR lpszPath);
        virtual BOOL Open(LPCTSTR lpszPath);
        virtual void Close();

        virtual BOOL DeleteValue(LPCTSTR lpszValue);
        virtual BOOL DeleteKey(LPCTSTR lpszPath, BOOL bAdmin = FALSE);

        //  The functions that actually need to be overridden are:
        //  DWORD
        //  string
        //  byte pointer
        //  This is not documented, but that's what the source says.  
        virtual BOOL Write(LPCTSTR lpszValueName, DWORD dwVal);
        virtual BOOL Write(LPCTSTR lpszValueName, LPCTSTR lpszVal);
        virtual BOOL Write(LPCTSTR lpszValueName, LPBYTE pData, UINT nBytes);

        virtual BOOL Read(LPCTSTR lpszValueName, DWORD& dwValue);
        virtual BOOL Read(LPCTSTR lpszValueName, CString& strValue);
        virtual BOOL Read(LPCTSTR lpszValueName, BYTE** ppData, UINT* pcbData);

        std::string m_iniFilename;
        std::string m_key;
};
