// WinClasses.cpp: implementation of the CWinClasses class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinClasses.h"
#include "wclassdefines.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// helper function for everyone to use
#ifdef _DEBUG
void TRACEWND(LPCTSTR szFunctionName, HWND hWnd)
{
    if (hWnd)
    {
        CString sText;
        //		pWnd->GetWindowText(sText);
        CString debugText;
        debugText.Format(_T("%s(%s, %s, id = %d)\n"), szFunctionName, CWinClasses::GetClass(hWnd), sText, GetDlgCtrlID(hWnd));
        TRACE (debugText);
    }
}
#else
void TRACEWND(LPCTSTR, HWND) {}
#endif

#define ADDCONTROL(ctrlname) { CString sCtrl(ctrlname); sCtrl.MakeLower(); s_mapCtrlClasses[sCtrl] = NULL; }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapStringToPtr CWinClasses::s_mapCtrlClasses;

CString CWinClasses::GetClass(HWND hWnd)
{
    static CString sWndClass;
    sWndClass.Empty(); // reset each time

    if (hWnd)
    {
        ::GetClassName(hWnd, sWndClass.GetBuffer(128), 128);

        sWndClass.ReleaseBuffer();
        sWndClass.MakeLower();
    }

    return sWndClass;
}

BOOL CWinClasses::IsClass(HWND hWnd, LPCTSTR szClass)
{
    if (hWnd)
        return IsClass(szClass, GetClass(hWnd));

    return FALSE;
}

BOOL CWinClasses::IsClassEx(HWND hWnd, LPCTSTR szClass)
{
    if (hWnd)
    {
        CString sClass = GetClassEx(hWnd);

        if (IsClass(sClass, szClass))
            return TRUE;
        else
        {
            CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);

            if (pWnd)
            {
                if (sClass == WC_MFCWND)
                    return TRUE;

                else if (IsClass(sClass, WC_MFCFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
                    return TRUE;

                else if (IsClass(sClass, WC_MFCVIEW) && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
                    return TRUE;

                else if (IsClass(sClass, WC_MFCMDIFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
                    return TRUE;

                else if (IsClass(sClass, WC_MFCMDICHILD) && pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
                    return TRUE;

                else if (IsClass(sClass, WC_MFCMINIDOCKFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
                    return TRUE;

                else if (IsClass(sClass, WC_MFCMINIFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
                    return TRUE;

                else if (IsClass(sClass, WC_MFCSPLITTER) && pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
                    return TRUE;

                else if (IsClass(sClass, WC_MFCDIALOGBAR) && pWnd->IsKindOf(RUNTIME_CLASS(CDialogBar)))
                    return TRUE;

                else if (IsClass(sClass, WC_CONTROLBAR) && pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)))
                    return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL CWinClasses::IsClassEx(LPCTSTR szClass, LPCTSTR szWndClass)
{
    if (IsClass(szClass, szWndClass)) // string comparison
        return TRUE;

    else if (IsClass(szWndClass, WC_MFCFRAME)) // handle frame derivatives
    {
        if (IsClass(szClass, WC_MFCMDIFRAME) || IsClass(szClass, WC_MFCMDICHILD) ||
            IsClass(szClass, WC_MFCMINIDOCKFRAME) || IsClass(szClass, WC_MFCMINIFRAME))
            return TRUE;
    }

    return FALSE;
}

CString CWinClasses::GetClassEx(HWND hWnd)
{
    CString sClass = GetClass(hWnd);

    // handle CCheckListBox
    if (IsClass(sClass, WC_LISTBOX))
    {
        CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);

        if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CCheckListBox)))
        {
            sClass = WC_CHECKLISTBOX;
            sClass.MakeLower();
        }
    }
    else if (!IsControlClass(sClass))
    {
        // can do the check if pWnd is permanent else mfc will not yet
        // have hooked up
        CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);

        if (pWnd)
        {
            // must do the check in order of most derived class first
            if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
                sClass = WC_MFCVIEW;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
                sClass = WC_MFCMDIFRAME;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
                sClass = WC_MFCMDICHILD;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
                sClass = WC_MFCMINIDOCKFRAME;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
                sClass = WC_MFCMINIFRAME;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) // this is the catch all for frame wnds
                sClass = WC_MFCFRAME;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
                sClass = WC_MFCSPLITTER;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialogBar)))
                sClass = WC_MFCDIALOGBAR;

            else if (pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)))
                sClass = WC_CONTROLBAR;

            sClass.MakeLower();
        }
    }

    return sClass;
}

BOOL CWinClasses::IsControlClass(HWND hWnd)
{
    return IsControlClass(GetClass(hWnd));
}

BOOL CWinClasses::IsControlClass(LPCTSTR szClass)
{
    // init
    if (!s_mapCtrlClasses.GetCount())
    {
        ADDCONTROL(WC_BUTTON);
        ADDCONTROL(WC_STATIC);
        ADDCONTROL(WC_EDIT);
        ADDCONTROL(WC_COMBOBOX);
        ADDCONTROL(WC_COMBOLBOX);
        ADDCONTROL(WC_LISTBOX);
        ADDCONTROL(WC_COMBOBOXEX);
        ADDCONTROL(WC_SCROLLBAR);
        ADDCONTROL(WC_TOOLBAR);
        ADDCONTROL(WC_SPIN);
        ADDCONTROL(WC_PROGRESS);
        ADDCONTROL(WC_SLIDER);
        ADDCONTROL(WC_HOTKEY);
        ADDCONTROL(WC_SHELLDLLDEFVIEW);
        ADDCONTROL(WC_STATUSBAR);
        ADDCONTROL(WC_ANIMATE);
        ADDCONTROL(WC_RICHEDIT);
        ADDCONTROL(WC_RICHEDIT20);
        ADDCONTROL(WC_RICHEDIT50);
        ADDCONTROL(WC_DATETIMEPICK);
        ADDCONTROL(WC_MONTHCAL);
        ADDCONTROL(WC_REBAR);
        ADDCONTROL(WC_TOOLTIPS);
        ADDCONTROL(WC_THUMBNAILVIEW);
        ADDCONTROL(WC_IE);
        ADDCONTROL(WC_SHDOCOBJVW);
        ADDCONTROL(WC_SHELLEMBEDDING);
    }

    void* pTemp;
    CString sClass(szClass);
    sClass.MakeLower();

    return s_mapCtrlClasses.Lookup(sClass, pTemp);
}

BOOL CWinClasses::IsEditControl(HWND hWnd)
{
    return IsEditControl(GetClass(hWnd));
}

BOOL CWinClasses::IsEditControl(LPCTSTR szClass)
{
    return (IsRichEditControl(szClass) || IsClass(szClass, WC_EDIT));
}

BOOL CWinClasses::IsRichEditControl(HWND hWnd)
{
    return IsRichEditControl(GetClass(hWnd));
}

BOOL CWinClasses::IsRichEditControl(LPCTSTR szClass)
{
    return (IsClass(szClass, WC_RICHEDIT) ||
            IsClass(szClass, WC_RICHEDIT20) ||
            IsClass(szClass, WC_RICHEDIT50));
}

BOOL CWinClasses::IsDialog(HWND hWnd)
{
    return IsClass(hWnd, WC_DIALOGBOX);
}

BOOL IsChild(HWND hWnd)
{
    return (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD);
}
