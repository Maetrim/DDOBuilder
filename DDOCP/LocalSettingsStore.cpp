#include "StdAfx.h"
#include "LocalSettingsStore.h"
#include <sstream>
#include <algorithm>

IMPLEMENT_DYNCREATE(CLocalSettingsStore, CSettingsStore)

CLocalSettingsStore::CLocalSettingsStore(void)
{
}

CLocalSettingsStore::~CLocalSettingsStore(void)
{
}

BOOL CLocalSettingsStore::Write(LPCTSTR lpszValueName, DWORD dwVal)
{
    // format the DWORD as a string, and save as a private profile value
    std::stringstream ss;
    ss << dwVal;
    BOOL ret = ::WritePrivateProfileString(
            m_key.c_str(),
            lpszValueName,
            ss.str().c_str(),
            IniFilename().c_str());
    return ret;
}

BOOL CLocalSettingsStore::Write(LPCTSTR lpszValueName, LPCTSTR lpszVal)
{
    BOOL ret = ::WritePrivateProfileString(
            m_key.c_str(),
            lpszValueName,
            lpszVal,
            IniFilename().c_str());
    return ret;
}

BOOL CLocalSettingsStore::Write(LPCTSTR lpszValueName, LPBYTE pData, UINT nBytes)
{
    std::string dataAsString;
    std::stringstream ss;
    for (size_t i = 0; i < nBytes; ++i)
    {
        if (i > 0)
        {
            ss << ",";
        }
        CString text;
        text.Format("%x", pData[i]);
        ss << (LPCTSTR)text;
    }
    dataAsString = ss.str();
    BOOL ret = ::WritePrivateProfileString(
            m_key.c_str(),
            lpszValueName,
            dataAsString.c_str(),
            IniFilename().c_str());
    return ret;
}

BOOL CLocalSettingsStore::Read(LPCTSTR lpszValueName, DWORD & dwValue)
{
    char buffer[16364];
    BOOL ret = ::GetPrivateProfileString(
            m_key.c_str(),
            lpszValueName,
            "",
            buffer,
            16364,
            IniFilename().c_str());
    if (ret != 0)
    {
        dwValue = atoi(buffer);
    }
    return ret;
}

BOOL CLocalSettingsStore::Read(LPCTSTR lpszValueName, CString& strValue)
{
    char buffer[16364];
    BOOL ret = ::GetPrivateProfileString(
            m_key.c_str(),
            lpszValueName,
            "",
            buffer,
            16364,
            IniFilename().c_str());
    if (ret != 0)
    {
        strValue = buffer;
    }
    return ret;
}


BOOL CLocalSettingsStore::Read(LPCTSTR lpszValueName, BYTE** ppData, UINT* pcbData)
{
    char buffer[16364];
    BOOL ret = ::GetPrivateProfileString(
            m_key.c_str(),
            lpszValueName,
            "",
            buffer,
            16364,
            IniFilename().c_str());
    // now convert back to data
    char * p = buffer;
    char * end = NULL;
    // count how many ',' characters we have in the data to determine how many
    // entries there will be
    if (ret != 0)
    {
        *pcbData = std::count(buffer, buffer + strlen(buffer), ',') + 1;
        if (*pcbData > 0)
        {
            *ppData = new BYTE[*pcbData];       // allocate buffer
            for (size_t i = 0; i < *pcbData; ++i)
            {
                long data = strtol(p, &end, 16);
                (*ppData)[i] = (BYTE)(data & 0x000000ff);
                p = end + 1;
            }
        }
        else
        {
            *ppData = NULL; // no data read
        }
    }
    else
    {
        *ppData = NULL; // no data read
    }

    return ret;
}

std::string CLocalSettingsStore::IniFilename()
{
    if (m_iniFilename.empty())
    {
        char fullPath[MAX_PATH];
        ::GetModuleFileName(NULL, fullPath, MAX_PATH);

        char drive[_MAX_DRIVE];
        char folder[_MAX_PATH];
        _splitpath_s(fullPath, drive, _MAX_DRIVE, folder, _MAX_PATH, NULL, 0, NULL, 0);

        char path[_MAX_PATH];
        _makepath_s(path, _MAX_PATH, drive, folder, NULL, NULL);

        m_iniFilename = path;
        m_iniFilename += "DDOBuilder.ini";
    }
    return m_iniFilename;
}

BOOL CLocalSettingsStore::CreateKey(LPCTSTR lpszPath)
{
    m_key = lpszPath;
    return TRUE;
}

BOOL CLocalSettingsStore::Open(LPCTSTR lpszPath)
{
    m_key = lpszPath;
    return TRUE;
}

void CLocalSettingsStore::Close()
{
}

BOOL CLocalSettingsStore::DeleteValue(LPCTSTR lpszValue)
{
    BOOL ret = ::WritePrivateProfileString(
            m_key.c_str(),
            lpszValue,
            NULL,           // NULL to delete value
            IniFilename().c_str());
    return ret;
}

BOOL CLocalSettingsStore::DeleteKey(LPCTSTR lpszPath, BOOL bAdmin)
{
    // have to check to see if key exists before try and delete
    char buffer[16364];
    BOOL ret = ::GetPrivateProfileString(
            m_key.c_str(),
            NULL,
            NULL,
            buffer,
            16364,
            IniFilename().c_str());
    if (ret != 0)
    {
        ret = ::WritePrivateProfileString(
                lpszPath,
                NULL,           // NULL to delete key
                NULL,           // NULL to delete value
                IniFilename().c_str());
    }
    return ret;
}

