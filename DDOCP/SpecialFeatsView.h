// SpecialFeatsView.h
//
#pragma once
#include "Resource.h"
#include <vector>
#include <list>
#include "FeatTypes.h"

class Feat;

class CSpecialFeatsView :
    public CFormView
{
    public:
        enum { IDD = IDD_SPECIAL_FEATS_VIEW };
        DECLARE_DYNCREATE(CSpecialFeatsView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CSpecialFeatsView();           // protected constructor used by dynamic creation
        virtual ~CSpecialFeatsView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

    private:
        void CreateFeatWindows(
                CStatic * groupWindow,
                const std::list<Feat> & featList,
                std::vector<CDialog *> * dialogs,
                TrainableFeatTypes type);
        size_t PositionWindows(
                CStatic * groupWindow,
                size_t startIndex,
                const std::vector<CDialog *> & dialogs,
                int * maxX,
                int * yPos);
        std::vector<CDialog *> m_heroicSelectionViews;
        std::vector<CDialog *> m_racialSelectionViews;
        std::vector<CDialog *> m_iconicSelectionViews;
        std::vector<CDialog *> m_epicSelectionViews;
        std::vector<CDialog *> m_specialSelectionViews;

        CStatic m_staticHeroic;
        CStatic m_staticRacial;
        CStatic m_staticIconic;
        CStatic m_staticEpic;
        CStatic m_staticSpecial;
        CFont m_staticFont;
};
