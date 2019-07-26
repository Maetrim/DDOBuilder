// ClassAndFeatView.h
//
#pragma once
#include "Resource.h"
#include "Character.h"
#include "FeatsClassControl.h"

class CClassAndFeatView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_CLASS_AND_FEAT_VIEW };
        DECLARE_DYNCREATE(CClassAndFeatView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();

    protected:
        CClassAndFeatView();           // protected constructor used by dynamic creation
        virtual ~CClassAndFeatView();

        virtual void UpdateClassChoiceChanged(Character * charData) override;

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnHScroll(UINT, UINT, CScrollBar*);
        afx_msg void OnVScroll( UINT, UINT, CScrollBar* );
        DECLARE_MESSAGE_MAP()

    private:
        CDocument * m_pDocument;
        Character * m_pCharacter;
        CFeatsClassControl m_featsAndClasses;
};
