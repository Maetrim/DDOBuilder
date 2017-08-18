// PersistantSize.h : implementation file
//
#pragma once

#include "SubclassWnd.h"

class CPersistantSize : 
    protected CSubclassWnd
{
    public:
        CPersistantSize();
        ~CPersistantSize();

        void Hook(HWND hWnd, LPCTSTR szEntry);
        void SetIsMainWindow(bool is);

    private:
        virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
        void OnDestroy();
        void OnPaint();

        bool ReadWindowPlacement(WINDOWPLACEMENT * wp);

        CString m_sSection;
        CString m_sEntry;
        bool m_bRestored;
        bool m_bIsMainWindow;
};
