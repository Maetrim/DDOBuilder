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
                int startX,
                size_t startIndex,
                const std::vector<CDialog *> & dialogs,
                int * maxX,
                int * yPos,
                bool bMoveDownALine = true);
        std::vector<CDialog *> m_heroicSelectionViews;
        std::vector<CDialog *> m_racialSelectionViews;
        std::vector<CDialog *> m_iconicSelectionViews;
        std::vector<CDialog *> m_epicSelectionViewsArcane;
        std::vector<CDialog *> m_epicSelectionViewsDivine;
        std::vector<CDialog *> m_epicSelectionViewsMartial;
        std::vector<CDialog *> m_epicSelectionViewsPrimal;
        std::vector<CDialog *> m_specialSelectionViews;
        std::vector<CDialog *> m_favorSelectionViews;

        CStatic m_staticHeroic;
        CStatic m_staticRacial;
        CStatic m_staticIconic;
        CStatic m_staticEpicArcane;
        CStatic m_staticEpicDivine;
        CStatic m_staticEpicMartial;
        CStatic m_staticEpicPrimal;
        CStatic m_staticSpecial;
        CStatic m_staticFavor;
        CFont m_staticFont;
};
