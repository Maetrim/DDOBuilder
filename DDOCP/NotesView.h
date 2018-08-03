// NotesView.h
//
#pragma once
#include "Resource.h"

class Character;

class CNotesView :
    public CFormView
{
    public:
        enum { IDD = IDD_NOTES };
        DECLARE_DYNCREATE(CNotesView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CNotesView();           // protected constructor used by dynamic creation
        virtual ~CNotesView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnNotesChanged();
        DECLARE_MESSAGE_MAP()

    private:
        CDocument * m_pDocument;
        Character * m_pCharacter;

        CEdit m_editNotes;
};
