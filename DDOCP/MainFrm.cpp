// MainFrm.cpp
//
#include "stdafx.h"
#include "MainFrm.h"

#include "BreakdownsView.h"
#include "ClassAndFeatView.h"
#include "CustomDockablePane.h"
#include "DDOCP.h"
#include "DCView.h"
#include "EnhancementsView.h"
#include "EpicDestiniesView.h"
#include "EquipmentView.h"
#include "LevelUpView.h"
#include "NotesView.h"
#include "ReaperEnhancementsView.h"
#include "SelfAndPartyBuffsView.h"
#include "SpecialFeatsView.h"
#include "SpellsView.h"
#include "SkillsView.h"
#include "StancesView.h"
#include "afxdatarecovery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
    ON_WM_CREATE()
    ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
    ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
    ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
    ON_WM_CLOSE()
    ON_UPDATE_COMMAND_UI(ID_DOCK_BREAKDOWNS, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_LEVELUP, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_SPECIALFEATS, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_ENHANCEMENTS, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_EQUIPMENT, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_SPELLS, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_REAPER, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_EPICDESTINIES, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_STANCES, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_SELFANDPARTYBUFFS, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_NOTES, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_DC, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_SKILLS, OnUpdateDockPane)
    ON_UPDATE_COMMAND_UI(ID_DOCK_CLASSFEATS, OnUpdateDockPane)
    ON_COMMAND(ID_DOCK_BREAKDOWNS, OnDockPane)
    ON_COMMAND(ID_DOCK_LEVELUP, OnDockPane)
    ON_COMMAND(ID_DOCK_SPECIALFEATS, OnDockPane)
    ON_COMMAND(ID_DOCK_ENHANCEMENTS, OnDockPane)
    ON_COMMAND(ID_DOCK_EQUIPMENT, OnDockPane)
    ON_COMMAND(ID_DOCK_SPELLS, OnDockPane)
    ON_COMMAND(ID_DOCK_REAPER, OnDockPane)
    ON_COMMAND(ID_DOCK_EPICDESTINIES, OnDockPane)
    ON_COMMAND(ID_DOCK_STANCES, OnDockPane)
    ON_COMMAND(ID_DOCK_SELFANDPARTYBUFFS, OnDockPane)
    ON_COMMAND(ID_DOCK_NOTES, OnDockPane)
    ON_COMMAND(ID_DOCK_DC, OnDockPane)
    ON_COMMAND(ID_DOCK_SKILLS, OnDockPane)
    ON_COMMAND(ID_DOCK_CLASSFEATS, OnDockPane)
    ON_COMMAND(ID_VIEW_RESETSCREENLAYOUT, OnResetScreenLayout)
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction
CMainFrame::CMainFrame() :
    m_pDocument(NULL),
    m_pCharacter(NULL)
{
    CopyDefaultIniToDDOBuilderIni();
    theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
    for (size_t x = 0; x < m_dockablePanes.size(); x++)
    {
        delete m_dockablePanes[x];
    }
    m_dockablePanes.clear();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    BOOL bNameValid;
    // set the visual manager and style based on persisted value
    OnApplicationLook(theApp.m_nAppLook);

    CMDITabInfo mdiTabParams;
    mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
    mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
    mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI tabs
    mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-colouring of MDI tabs
    mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
    EnableMDITabbedGroups(TRUE, mdiTabParams);

    if (!m_wndMenuBar.Create(this))
    {
        return -1;      // fail to create
    }

    m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

    // prevent the menu bar from taking the focus on activation
    CMFCPopupMenu::SetForceMenuFocus(FALSE);

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
    {
        return -1;      // fail to create
    }

    m_menuToolbar.LoadToolBar(IDR_MENUICONS_TOOLBAR);

    CString strToolBarName;
    bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
    ASSERT(bNameValid);
    m_wndToolBar.SetWindowText(strToolBarName);

    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);
    m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

    // Allow user-defined toolbars operations:
    InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

    if (!m_wndStatusBar.Create(this))
    {
        return -1;      // fail to create
    }
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

    // TODO: Delete these five lines if you don't want the toolbar and menu bar to be dockable
    m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndMenuBar);
    DockPane(&m_wndToolBar);

    // enable Visual Studio 2005 style docking window behaviour
    CDockingManager::SetDockingMode(DT_SMART);

    // enable Visual Studio 2005 style docking window auto-hide behaviour
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // Enable enhanced windows management dialog
    //EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

    // Enable toolbar and docking window menu replacement
    EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

    // enable quick (Alt+drag) toolbar customization
    CMFCToolBar::EnableQuickCustomization();

    if (CMFCToolBar::GetUserImages() == NULL)
    {
        // load user-defined toolbar images
        if (m_UserImages.Load(_T(".\\UserImages.bmp")))
        {
            CMFCToolBar::SetUserImages(&m_UserImages);
        }
    }

    // enable menu personalization (most-recently used commands)
    // TODO: define your own basic commands, ensuring that each pull down menu has at least one basic command.
    CList<UINT, UINT> lstBasicCommands;

    lstBasicCommands.AddTail(ID_FILE_NEW);
    lstBasicCommands.AddTail(ID_FILE_OPEN);
    lstBasicCommands.AddTail(ID_FILE_SAVE);
    lstBasicCommands.AddTail(ID_FILE_PRINT);
    lstBasicCommands.AddTail(ID_APP_EXIT);
    lstBasicCommands.AddTail(ID_EDIT_CUT);
    lstBasicCommands.AddTail(ID_EDIT_PASTE);
    lstBasicCommands.AddTail(ID_EDIT_UNDO);
    lstBasicCommands.AddTail(ID_APP_ABOUT);
    lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
    lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
    lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

    CMFCToolBar::SetBasicCommands(lstBasicCommands);

    // Switch the order of document name and application name on the window title bar. This
    // improves the usability of the task bar because the document name is visible with the thumbnail.
    ModifyStyle(0, FWS_PREFIXTITLE);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWndEx::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers
void CMainFrame::OnWindowManager()
{
    ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
    CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
    pDlgCust->EnableUserDefinedToolbars();
    pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
    LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
    if (lres == 0)
    {
        return 0;
    }

    CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
    ASSERT_VALID(pUserToolbar);

    BOOL bNameValid;
    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
    return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
    CWaitCursor wait;

    theApp.m_nAppLook = id;

    switch (theApp.m_nAppLook)
    {
    case ID_VIEW_APPLOOK_WIN_2000:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
        break;

    case ID_VIEW_APPLOOK_OFF_XP:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
        break;

    case ID_VIEW_APPLOOK_WIN_XP:
        CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
        break;

    case ID_VIEW_APPLOOK_OFF_2003:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    case ID_VIEW_APPLOOK_VS_2005:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    case ID_VIEW_APPLOOK_VS_2008:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    case ID_VIEW_APPLOOK_WINDOWS_7:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
        CDockingManager::SetDockingMode(DT_SMART);
        break;

    default:
        switch (theApp.m_nAppLook)
        {
        case ID_VIEW_APPLOOK_OFF_2007_BLUE:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_BLACK:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_SILVER:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_AQUA:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
            break;
        }

        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
        CDockingManager::SetDockingMode(DT_SMART);
    }

    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
    // base class does the real work
    if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
    {
        return FALSE;
    }

    // enable customization button for all user toolbars
    BOOL bNameValid;
    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    for (int i = 0; i < iMaxUserToolbars; i ++)
    {
        CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
        if (pUserToolbar != NULL)
        {
            pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
        }
    }

    return TRUE;
}

CCustomDockablePane* CMainFrame::CreateDockablePane(
        const char* paneTitle, 
        CDocument* doc, 
        CRuntimeClass* runtimeClass,
        UINT viewID)
{
    CCreateContext createContext;
    createContext.m_pCurrentDoc = doc;
    createContext.m_pNewViewClass = runtimeClass;

    CCustomDockablePane* pane = new CCustomDockablePane;
    // Assorted functionality is applied to all panes
    m_dockablePanes.push_back(pane);

    pane->Create(
            _T(paneTitle),
            this,
            CRect(0,0,400,400),
            TRUE,
            viewID, 
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI, 
            32UL,
            15UL,
            &createContext);
    pane->EnableDocking(CBRS_ALIGN_ANY);
    DockPane(pane);

    return pane;
}

BOOL CMainFrame::OnCmdMsg(
        UINT nID,
        int nCode,
        void * pExtra,
        AFX_CMDHANDLERINFO * pHandlerInfo)
{
    BOOL bReturn = FALSE;

    // see if a child pane can handle it
    if (FALSE == bReturn)
    {
        // Offer command to child views by looking in each docking window which
        // holds a view until we get the 1st view that handles the message.
        for (size_t x = 0; bReturn == FALSE && x < m_dockablePanes.size(); x++)
        {
            CView * pView = m_dockablePanes[x]->GetView();
            if (pView != NULL)
            {
                bReturn = pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
            }
        }
    }

    if (FALSE == bReturn)
    {
        // also offer the message to any views for the active open document
        CDocument * pDoc = GetActiveDocument();
        if (pDoc != NULL)
        {
            POSITION pos = pDoc->GetFirstViewPosition();
            while (pos != NULL && bReturn == FALSE)
            {
                CView * pView = pDoc->GetNextView(pos);
                if (pView != NULL)
                {
                    bReturn = pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
                }
            }
        }
    }

    // try to handle the message ourselves
    if (FALSE == bReturn)
    {
        bReturn = CMDIFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    }

    return bReturn;
}

void CMainFrame::CreateViews()
{
    // create the floating views
    CCustomDockablePane * pBreakdownsPane = CreateDockablePane(
            "Breakdowns",
            GetActiveDocument(),
            RUNTIME_CLASS(CBreakdownsView),
            ID_DOCK_BREAKDOWNS);
    pBreakdownsPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pLevelUpPane = CreateDockablePane(
            "Level Up",
            GetActiveDocument(),
            RUNTIME_CLASS(CLevelUpView),
            ID_DOCK_LEVELUP);
    pLevelUpPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pSpecialFeatsPane = CreateDockablePane(
            "Past Lives and Special Feats",
            GetActiveDocument(),
            RUNTIME_CLASS(CSpecialFeatsView),
            ID_DOCK_SPECIALFEATS);
    pSpecialFeatsPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pEnhancementsPane = CreateDockablePane(
            "Enhancements",
            GetActiveDocument(),
            RUNTIME_CLASS(CEnhancementsView),
            ID_DOCK_ENHANCEMENTS);
    pEnhancementsPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pEquipmentPane = CreateDockablePane(
            "Equipment",
            GetActiveDocument(),
            RUNTIME_CLASS(CEquipmentView),
            ID_DOCK_EQUIPMENT);
    pEquipmentPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pSpellsPane = CreateDockablePane(
            "Spells",
            GetActiveDocument(),
            RUNTIME_CLASS(CSpellsView),
            ID_DOCK_SPELLS);
    pSpellsPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pReaperPane = CreateDockablePane(
            "Reaper Enhancements",
            GetActiveDocument(),
            RUNTIME_CLASS(CReaperEnhancementsView),
            ID_DOCK_REAPER);
    pReaperPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pEpicDestiniesPane = CreateDockablePane(
            "Epic Destinies",
            GetActiveDocument(),
            RUNTIME_CLASS(CEpicDestiniesView),
            ID_DOCK_EPICDESTINIES);
    pEpicDestiniesPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pStancesPane = CreateDockablePane(
            "Stances",
            GetActiveDocument(),
            RUNTIME_CLASS(CStancesView),
            ID_DOCK_STANCES);
    pStancesPane->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pBuffs = CreateDockablePane(
            "Self and Party Buffs",
            GetActiveDocument(),
            RUNTIME_CLASS(CSelfAndPartyBuffsView),
            ID_DOCK_SELFANDPARTYBUFFS);
    pBuffs->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pNotes = CreateDockablePane(
            "Notes",
            GetActiveDocument(),
            RUNTIME_CLASS(CNotesView),
            ID_DOCK_NOTES);
    pNotes->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pDCs = CreateDockablePane(
            "DCs",
            GetActiveDocument(),
            RUNTIME_CLASS(CDCView),
            ID_DOCK_DC);
    pDCs->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pSkills = CreateDockablePane(
            "Skills",
            GetActiveDocument(),
            RUNTIME_CLASS(CSkillsView),
            ID_DOCK_SKILLS);
    pSkills->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // create the floating views
    CCustomDockablePane * pClassAndLevel = CreateDockablePane(
            "Class and Levels",
            GetActiveDocument(),
            RUNTIME_CLASS(CClassAndFeatView),
            ID_DOCK_CLASSFEATS);
    pClassAndLevel->SetDocumentAndCharacter(GetActiveDocument(), NULL);

    // next window id is 1014 if you add one
}

void CMainFrame::SetActiveDocumentAndCharacter(CDocument * pDoc, Character * pCharacter)
{
    if (m_pCharacter != pCharacter)
    {
        m_pDocument = pDoc;
        m_pCharacter = pCharacter;
        // update all the floating windows with the new active character
        for (size_t x = 0; x < m_dockablePanes.size(); x++)
        {
            m_dockablePanes[x]->SetDocumentAndCharacter(pDoc, pCharacter);
        }
        if (m_pCharacter != NULL)
        {
            pCharacter->NowActive();
        }
    }
}

// this is the exact same code from CFrameWnd::OnClose() except for a fix to stop
// access to a closed pDocument pointer which has since been deleted.
void CMainFrame::OnClose()
{
    if (m_lpfnCloseProc != NULL)
    {
        // if there is a close proc, then defer to it, and return
        // after calling it so the frame itself does not close.
        (*m_lpfnCloseProc)(this);
        return;
    }

    // Note: only queries the active document
    CDocument* pDocument = GetActiveDocument();
    if (pDocument != NULL && !pDocument->CanCloseFrame(this))
    {
        // document can't close right now -- don't close it
        return;
    }
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL && pApp->m_pMainWnd == this)
    {
        CDataRecoveryHandler *pHandler = pApp->GetDataRecoveryHandler();
        if ((pHandler != NULL) && (pHandler->GetShutdownByRestartManager()))
        {
            // If the application is being shut down by the Restart Manager, do
            // a final auto save.  This will mark all the documents as not dirty,
            // so the SaveAllModified call below won't prompt for save.
            pHandler->AutosaveAllDocumentInfo();
            pHandler->SaveOpenDocumentList();
        }

        // attempt to save all documents
        if (pDocument == NULL && !pApp->SaveAllModified())
            return;     // don't close it

        if ((pHandler != NULL) && (!pHandler->GetShutdownByRestartManager()))
        {
            // If the application is not being shut down by the Restart Manager,
            // delete any auto saved documents since everything is now fully saved.
            pHandler->DeleteAllAutosavedFiles();
        }

        // hide the application's windows before closing all the documents
        pApp->HideApplication();

        // close all documents first
        pApp->CloseAllDocuments(FALSE);
        pDocument = GetActiveDocument();        // bug fix, pDocument may be NULL now

        // don't exit if there are outstanding component objects
        if (!AfxOleCanExitApp())
        {
            // take user out of control of the app
            AfxOleSetUserCtrl(FALSE);

            // don't destroy the main window and close down just yet
            //  (there are outstanding component (OLE) objects)
            return;
        }

        // there are cases where destroying the documents may destroy the
        //  main window of the application.
        if (!afxContextIsDLL && pApp->m_pMainWnd == NULL)
        {
            AfxPostQuitMessage(0);
            return;
        }
    }

    // detect the case that this is the last frame on the document and
    // shut down with OnCloseDocument instead.
    if (pDocument != NULL && pDocument->m_bAutoDelete)
    {
        BOOL bOtherFrame = FALSE;
        POSITION pos = pDocument->GetFirstViewPosition();
        while (pos != NULL)
        {
            CView* pView = pDocument->GetNextView(pos);
            ENSURE_VALID(pView);
            if (pView->GetParentFrame() != this)
            {
                bOtherFrame = TRUE;
                break;
            }
        }
        if (!bOtherFrame)
        {
            pDocument->OnCloseDocument();
            return;
        }

        // allow the document to cleanup before the window is destroyed
        pDocument->PreCloseFrame(this);
    }
    // let the base class do its thing now
    CMDIFrameWndEx::OnClose();
}

void CMainFrame::SetStatusBarPromptText(const CString & text)
{
    m_wndStatusBar.SetPaneText(
            0, 
            text, 
            TRUE);
}

MouseHook * CMainFrame::GetMouseHook()
{
    return &m_mouseHook;
}

void CMainFrame::OnUpdateDockPane(CCmdUI* pCmdUI)
{
    size_t index = pCmdUI->m_nID - ID_DOCK_BREAKDOWNS;  // now 0...n
    ASSERT(index < m_dockablePanes.size());
    pCmdUI->SetCheck(m_dockablePanes[index]->IsVisible());
    pCmdUI->Enable(TRUE);           // always enabled
}

void CMainFrame::OnDockPane()
{
    const MSG * pMsg = GetCurrentMessage();
    size_t index = LOWORD(pMsg->wParam) - ID_DOCK_BREAKDOWNS;  // now 0...n
    ASSERT(index < m_dockablePanes.size());
    m_dockablePanes[index]->ShowPane(!m_dockablePanes[index]->IsVisible(), FALSE, TRUE);
}

BreakdownItem * CMainFrame::FindBreakdown(BreakdownType type)
{
    CWnd * pWnd = m_dockablePanes[0]->GetView(); // it is the first created
    CBreakdownsView * pView = dynamic_cast<CBreakdownsView*>(pWnd);
    if (pView != NULL)
    {
        return pView->FindBreakdown(type);
    }
    return NULL;
}

void CMainFrame::CopyDefaultIniToDDOBuilderIni()
{
    // before any settings are read from the ini file, we need to check to see if it
    // exists or not. If it does not exist, we copy the "Default.ini" file as
    // the ini file to be used. This allows new and existing users to get good
    // default settings on startup (new users get the defaults, old users get
    // their previous settings)
    char fullPath[MAX_PATH];
    ::GetModuleFileName(
            NULL,
            fullPath,
            MAX_PATH);

    char drive[_MAX_DRIVE];
    char folder[_MAX_PATH];
    _splitpath_s(fullPath,
            drive, _MAX_DRIVE,
            folder, _MAX_PATH,
            NULL, 0,        // filename
            NULL, 0);       // extension

    char path[_MAX_PATH];
    _makepath_s(path, _MAX_PATH, drive, folder, NULL, NULL);
    std::string filename = path;
    filename += "DDOBuilder.ini";
    bool exists = false;
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(filename.c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        exists = true;
    }
    if (!exists)
    {
        // copy the "Default.ini" file to "DDOBuilder.ini"
        std::string defaultIni = path;
        defaultIni += "Default.ini";
        // does Default.ini file exist?
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(defaultIni.c_str(), &findFileData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FindClose(hFind);
            exists = true;
        }
        if (exists)
        {
            // we can do the copy step
            ::CopyFile(defaultIni.c_str(), filename.c_str(), TRUE);
        }
    }
}

const CDCView * CMainFrame::GetDCView()
{
    const CDCView * pDCView = NULL;
    for (size_t i = 0; pDCView == NULL && i < m_dockablePanes.size(); ++i)
    {
        pDCView = dynamic_cast<CDCView *>(m_dockablePanes[i]->GetView());
    }
    return pDCView;
}

const CSLAControl * CMainFrame::GetSLAControl()
{
    const CSLAControl * slaControl = NULL;
    // the SLA control is in the CSpellsView
    CSpellsView * pSpellsView = NULL;
    for (size_t i = 0; pSpellsView == NULL && i < m_dockablePanes.size(); ++i)
    {
        pSpellsView = dynamic_cast<CSpellsView *>(m_dockablePanes[i]->GetView());
    }
    if (pSpellsView != NULL)
    {
        slaControl = pSpellsView->GetSLAControl();
    }
    return slaControl;
}

const CStancesView * CMainFrame::GetStancesView()
{
    const CStancesView * pStancesView = NULL;
    for (size_t i = 0; pStancesView == NULL && i < m_dockablePanes.size(); ++i)
    {
        pStancesView = dynamic_cast<CStancesView *>(m_dockablePanes[i]->GetView());
    }
    return pStancesView;
}

CSpellsControl * CMainFrame::GetSpellsControl(ClassType classType)
{
    CSpellsControl * pSC = NULL;
    CSpellsView * pSpellsView = NULL;
    for (size_t i = 0; pSpellsView == NULL && i < m_dockablePanes.size(); ++i)
    {
        pSpellsView = dynamic_cast<CSpellsView *>(m_dockablePanes[i]->GetView());
    }
    if (pSpellsView != NULL)
    {
        pSC = pSpellsView->GetSpellsControl(classType);
    }
    return pSC;
}

void CMainFrame::OnResetScreenLayout()
{
    // if the user has messed up their screen layout and lost windows of the
    // sides they need to be able to recover these. We need to do a layout reset
    // of all window locations. This is done by loading the layout profile
    // from "DefaultWorkspace" in the DDOBuilder.ini file
    CWinApp * pApp = AfxGetApp();
    CWinAppEx * pAppEx = dynamic_cast<CWinAppEx*>(pApp);
    if (pApp != NULL)
    {
        ShowWindow(SW_HIDE);    // hide windows while update occurs
        pAppEx->LoadState(this, "DefaultWorkspace");
        ShowWindow(SW_SHOW);
        pAppEx->SaveState(this, "Workspace");
    }
}

