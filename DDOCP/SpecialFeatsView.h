// SpecialFeatsView.h
//
#pragma once
#include "Resource.h"
#include <vector>
#include <list>
#include "FeatTypes.h"

class Feat;
class CFeatSelectionDialog;

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
        afx_msg void OnHeroic();
        afx_msg void OnRacial();
        afx_msg void OnIconic();
        afx_msg void OnEpicArcane();
        afx_msg void OnEpicDivine();
        afx_msg void OnEpicMartial();
        afx_msg void OnEpicPrimal();
        afx_msg void OnSpecial();
        afx_msg void OnFavor();
        afx_msg LRESULT OnRevokeFeats(WPARAM, LPARAM);
        DECLARE_MESSAGE_MAP()

    private:
        void CreateFeatWindows(
                CStatic * groupWindow,
                const std::list<Feat> & featList,
                std::vector<CFeatSelectionDialog *> * dialogs,
                TrainableFeatTypes type);
        size_t PositionWindows(
                CStatic * groupWindow,
                int startX,
                size_t startIndex,
                const std::vector<CFeatSelectionDialog *> & dialogs,
                int * maxX,
                int * yPos,
                bool bMoveDownALine = true);
        void TrainAllFeats(std::vector<CFeatSelectionDialog *>& feats);
        void RevokeAllFeats(std::vector<CFeatSelectionDialog *>& feats);
        std::vector<CFeatSelectionDialog *> m_heroicSelectionViews;
        std::vector<CFeatSelectionDialog *> m_racialSelectionViews;
        std::vector<CFeatSelectionDialog *> m_iconicSelectionViews;
        std::vector<CFeatSelectionDialog *> m_epicSelectionViewsArcane;
        std::vector<CFeatSelectionDialog *> m_epicSelectionViewsDivine;
        std::vector<CFeatSelectionDialog *> m_epicSelectionViewsMartial;
        std::vector<CFeatSelectionDialog *> m_epicSelectionViewsPrimal;
        std::vector<CFeatSelectionDialog *> m_specialSelectionViews;
        std::vector<CFeatSelectionDialog *> m_favorSelectionViews;

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
