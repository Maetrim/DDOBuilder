// CustomDockablePane.cpp
//
#include "stdafx.h"
#include "CustomDockablePane.h"
#include "CustomContextMenuManager.h"
#include "DDOCP.h"

const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
//---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CCustomDockablePane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()
#pragma warning(pop)

//---------------------------------------------------------------------------
CCustomDockablePane::CCustomDockablePane() :
    CDockablePane(),
    m_view(NULL),
    m_resizeViewWithPane(false),
    m_pCharacter(NULL),
    m_document(NULL)
{
}

//---------------------------------------------------------------------------
int CCustomDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    CCreateContext* createContext = (CCreateContext*)(lpCreateStruct->lpCreateParams);
    CRuntimeClass* viewRuntimeClass = createContext->m_pNewViewClass;

    CView* theView = (CView*)(viewRuntimeClass->CreateObject());

    CRect r;
    GetClientRect(r);
    theView->Create(
            0,
            0,
            WS_CHILD | WS_VISIBLE,
            r,
            this,
            0,
            createContext);
    SetView(theView, true);
    theView->SendMessage(WM_INITIALUPDATE, 0, 0);

    return 0;
}

//---------------------------------------------------------------------------
void CCustomDockablePane::SetView(CView* v, bool r)
{
    m_view = v;
    m_resizeViewWithPane = r;
}

void CCustomDockablePane::SetDocumentAndCharacter(
        CDocument * pDoc,
        Character * pCharacter)
{
    m_document = pDoc;
    m_pCharacter = pCharacter;
    m_view->SendMessage(UWM_NEW_DOCUMENT, (WPARAM)pDoc, (LPARAM)pCharacter);
}

//---------------------------------------------------------------------------
void CCustomDockablePane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
}

//---------------------------------------------------------------------------
void CCustomDockablePane::AdjustLayout()
{
    if (GetSafeHwnd())
    {
        // If we have a contained view, resize it as well
        if (m_view && m_resizeViewWithPane)
        {
            CRect r;
            GetClientRect(r);

            if (m_view->GetSafeHwnd()
                    && r.Height() > 0
                    && r.Width() > 0)
            {
                m_view->SetWindowPos(
                        this,
                        r.left,
                        r.top,
                        r.Width(),
                        r.Height(),
                        SWP_NOACTIVATE | SWP_NOZORDER);
            }
        }

    //    // The MFC2010 dockable panes seem to have a border redraw bug, where only the borders of the pane
    //    // resized get redrawn. Other panes which received knock-on resize messages do not seem to redraw 
    //    // their borders properly, and old content shows through the cracks. So force it to redraw all panes.
    //    CDDOCPApp* theApp = dynamic_cast<CDDOCPApp*> (AfxGetApp());
    //    if (theApp != NULL)
    //    {
    //        AfxGetMainWnd()->RedrawWindow(
    //                0,
    //                0,
    //                RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
    //    }
    }
}

BOOL CCustomDockablePane::OnCmdMsg(
        UINT nID,
        int nCode,
        void* pExtra,
        AFX_CMDHANDLERINFO* pHandlerInfo)
{
    BOOL handled = false;

    if (IsWindow(m_view->GetSafeHwnd()))
    {
        handled = m_view->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    }

    if (!handled)
    {
        handled =  CDockablePane::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    }

    return handled;
}

void CCustomDockablePane::OnContextMenu(CWnd* pWnd, CPoint point)
{
    //bool bCallBaseClass = true;  // assume
    //// dockable pane context menus can erroneously display a pop-up menu after a 
    //// child window has also displayed a pop-up menu on a right click. We need to stop
    //// this happening if the child window is displaying a pop-up window of its own
    //CWinApp * pApp = AfxGetApp();
    //CDDOCPApp * pSimApp = dynamic_cast<CDDOCPApp*>(pApp);
    //if (pSimApp != NULL)
    //{
    //    CContextMenuManager * pManager = pSimApp->GetContextMenuManager();
    //    // try and cast it up to the type that lets us track whether a pop-up is being shown already
    //    CCustomContextMenuManager * pCustomManager = dynamic_cast<CCustomContextMenuManager *>(pManager);
    //    if (pCustomManager != NULL)
    //    {
    //        // if there is a popup active, don't call the base class
    //        bCallBaseClass = !pCustomManager->PopupMenuActive();
    //    }
    //}
    //if (bCallBaseClass)
    //{
    //    CDockablePane::OnContextMenu(pWnd, point);
    //}
}

int CCustomDockablePane::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    // this is required to stop an assert in CView::OnMouseActivate when a dockable
    // pane is set to floating mode.
    CFrameWnd* pParentFrame = GetParentFrame();

    if( (pParentFrame == pDesktopWnd) || 
        (pDesktopWnd->IsChild(pParentFrame)))
    {
        return CDockablePane::OnMouseActivate(pDesktopWnd, nHitTest, message);
    }

    return MA_NOACTIVATE;
}
