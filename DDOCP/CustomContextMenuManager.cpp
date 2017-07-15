// CustomContextMenuManager.cpp
//
#include "stdafx.h"
#include "CustomContextMenuManager.h"

CCustomContextMenuManager::CCustomContextMenuManager() :
    m_bPopupShown(false)
{
}

CCustomContextMenuManager::~CCustomContextMenuManager()
{
}

UINT CCustomContextMenuManager::TrackPopupMenu(
        HMENU hmenuPopup,
        int x,
        int y,
        CWnd * pWndOwner,
        BOOL bRightAlign)
{
    m_bPopupShown = true;
    UINT ret = CContextMenuManager::TrackPopupMenu(
            hmenuPopup,
            x,
            y,
            pWndOwner,
            bRightAlign);
    m_bPopupShown = false;
    return ret;
}

bool CCustomContextMenuManager::PopupMenuActive() const
{
    return m_bPopupShown;
}

