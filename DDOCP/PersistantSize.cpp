// PersistantSize.cpp
//

#include "stdafx.h"
#include "PersistantSize.h"

namespace
{
    const char f_strFormat[] = "%u,%u,%d,%d,%d,%d,%d,%d,%d,%d";
}

CPersistantSize::CPersistantSize() :
    m_bRestored(false),
    m_bIsMainWindow(false)
{
}

CPersistantSize::~CPersistantSize()
{
}

void CPersistantSize::Hook(HWND hWnd, LPCTSTR szEntry)
{
    // Constructor
    // All that we do here is save the section name which will be used
    // later when persisting the windows position.

    m_sSection = "Settings";
    m_sEntry = szEntry;

    // hook the window to handle the WM_DESTROY message so that we can
    // write out the position of the window at that point.
    HookWindow(hWnd);

    WINDOWPLACEMENT wp;
    if (ReadWindowPlacement(&wp))
    {
        SetWindowPos(
                hWnd,
                0,
                wp.rcNormalPosition.left, 
                wp.rcNormalPosition.top, 
                wp.rcNormalPosition.right - wp.rcNormalPosition.left, 
                wp.rcNormalPosition.bottom - wp.rcNormalPosition.top, 
                SWP_DEFERERASE | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOZORDER);

        HWND hParent = GetParent();
        if (hParent != NULL)
        {
            // it's a child window so allow to be max/min-imised
            ShowWindow(hWnd, wp.showCmd);
        }
    }
    else
    {
        ShowWindow(hWnd, SW_SHOW);
    }
}

void CPersistantSize::SetIsMainWindow(bool is)
{
    m_bIsMainWindow = is;
}

LRESULT CPersistantSize::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    bool unhook = false;
    switch (msg)
    {
        case WM_DESTROY:
            OnDestroy();
            unhook = true;
            break;
        case WM_PAINT:
            OnPaint();
            break;
    }
    LRESULT ret = CSubclassWnd::WindowProc(hRealWnd, msg, wp, lp);
    if (unhook)
    {
        HookWindow(NULL);
    }
    return ret;
}

void CPersistantSize::OnDestroy()
{
    HWND hWnd = GetHwnd();

    WINDOWPLACEMENT wp;
    wp.length = sizeof wp;
    BOOL ok = GetWindowPlacement(hWnd, &wp);
    if (ok)
    {
        wp.flags = 0;
        if (IsZoomed(hWnd))
        {
            wp.flags |= WPF_RESTORETOMAXIMIZED;
        }

        TCHAR szBuffer[sizeof("-32767")*8 + sizeof("65535")*2];

        wsprintf(
                szBuffer,
                f_strFormat,
                wp.flags,
                wp.showCmd,
                wp.ptMinPosition.x,
                wp.ptMinPosition.y,
                wp.ptMaxPosition.x, 
                wp.ptMaxPosition.y,
                wp.rcNormalPosition.left, 
                wp.rcNormalPosition.top,
                wp.rcNormalPosition.right, 
                wp.rcNormalPosition.bottom);

        AfxGetApp()->WriteProfileString(m_sSection, m_sEntry, szBuffer);
    }
}

void CPersistantSize::OnPaint()
{
    if (m_bIsMainWindow)
    {
        if (!m_bRestored)
        {
            m_bRestored = true;
            WINDOWPLACEMENT wp;
            wp.length = sizeof wp;
            if (ReadWindowPlacement(&wp))
            {
                AfxGetMainWnd()->SetWindowPlacement(&wp);
            }
        }
    }
}

bool CPersistantSize::ReadWindowPlacement(WINDOWPLACEMENT * wp)
{
    bool readOk = false;
    CString strBuffer = AfxGetApp()->GetProfileString(m_sSection, m_sEntry);
    if (!strBuffer.IsEmpty())
    {
        int nRead = sscanf_s(
                strBuffer,
                f_strFormat,
                &wp->flags, 
                &wp->showCmd,
                &wp->ptMinPosition.x, 
                &wp->ptMinPosition.y,
                &wp->ptMaxPosition.x, 
                &wp->ptMaxPosition.y,
                &wp->rcNormalPosition.left, 
                &wp->rcNormalPosition.top,
                &wp->rcNormalPosition.right, 
                &wp->rcNormalPosition.bottom);

        if (nRead == 10)
        {
            readOk = true;
        }
    }
    return readOk;
}
