// MainFrm.h
//
#pragma once
#include "BreakdownTypes.h"
#include "MouseHook.h"
#include <vector>
#include "ClassTypes.h"

class BreakdownItem;
class CCustomDockablePane;
class CDCView;
class Character;
class CSLAControl;
class CSpellsControl;
class CStancesView;

class CMainFrame :
    public CMDIFrameWndEx
{
        DECLARE_DYNAMIC(CMainFrame)
    public:
        CMainFrame();
        virtual ~CMainFrame();
        CCustomDockablePane* CreateDockablePane(
                const char* paneTitle, 
                CDocument* doc, 
                CRuntimeClass* runtimeClass,
                UINT viewID);
        void CreateViews();
#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

        void SetStatusBarPromptText(const CString & text);
        void SetActiveDocumentAndCharacter(CDocument * pDoc, Character * pCharacter);
        BreakdownItem * FindBreakdown(BreakdownType type);

        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
        virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

        MouseHook * GetMouseHook();

        const CDCView * GetDCView();
        const CSLAControl * GetSLAControl();
        const CStancesView * GetStancesView();
        const CSpellsControl * GetSpellsControl(ClassType classType);

    protected:
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnWindowManager();
        afx_msg void OnViewCustomize();
        afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
        afx_msg void OnApplicationLook(UINT id);
        afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
        afx_msg void OnClose();
        afx_msg void OnUpdateDockPane(CCmdUI* pCmdUI);
        afx_msg void OnDockPane();
        afx_msg void OnResetScreenLayout();
        DECLARE_MESSAGE_MAP()

        void CopyDefaultIniToDDOBuilderIni();

    protected:  // control bar embedded members
        CMFCMenuBar       m_wndMenuBar;
        CMFCToolBar       m_wndToolBar;
        CMFCStatusBar     m_wndStatusBar;
        CMFCToolBarImages m_UserImages;
        std::vector<CCustomDockablePane*> m_dockablePanes;
        // never display these toolbars, just to get icons into UI
        CMFCToolBar       m_menuToolbar;

        CDocument * m_pDocument;
        Character * m_pCharacter;

        MouseHook m_mouseHook;
};
