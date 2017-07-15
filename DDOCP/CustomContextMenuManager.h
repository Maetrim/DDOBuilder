// CustomContextMenuManager.h
//
#pragma once

#include "afxcontextmenumanager.h"

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

class CCustomContextMenuManager :
    public CContextMenuManager
{
    public:
        CCustomContextMenuManager();
        ~CCustomContextMenuManager();

        virtual UINT TrackPopupMenu(
                HMENU hmenuPopup,
                int x,
                int y,
                CWnd* pWndOwner,
                BOOL bRightAlign = FALSE) override;
        bool PopupMenuActive() const;
    private:
        bool m_bPopupShown;
};
