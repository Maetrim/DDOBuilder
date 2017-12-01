// CustomDockablePane.h
//
#pragma once

class Character;
//---------------------------------------------------------------------------
class CCustomDockablePane :
        public CDockablePane
{
    public:
        CCustomDockablePane();
        virtual ~CCustomDockablePane() {}

        //MFC
        DECLARE_MESSAGE_MAP()

        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnWindowPosChanging(WINDOWPOS * pos);
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
        afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
        virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    public:
        // If derived classes have a contained view they can get free auto resize
        void SetView(CView*, bool resizeWithPane = true);
        CView* GetView() {return m_view;}

        void SetDocumentAndCharacter(CDocument * pDoc, Character * pCharacter);

    protected:
        CDocument * m_document;
        Character * m_pCharacter;

    private:
        void AdjustLayout();

        // Members
        CView * m_view;
        bool m_resizeViewWithPane;
};
