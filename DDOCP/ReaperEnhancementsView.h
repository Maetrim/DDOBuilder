// ReaperEnhancementsView.h
//
#pragma once
#include "Resource.h"
#include <vector>
#include "EnhancementTree.h"
#include "Character.h"

class CReaperEnhancementsView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_REAPER_ENHANCEMENTS_VIEW };
        DECLARE_DYNCREATE(CReaperEnhancementsView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CReaperEnhancementsView();           // protected constructor used by dynamic creation
        virtual ~CReaperEnhancementsView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

    private:
        virtual void UpdateEnhancementTrained(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;
        virtual void UpdateEnhancementRevoked(Character * charData, const std::string & enhancementName, const std::string & selection, bool isTier5) override;
        virtual void UpdateEnhancementTreeReset(Character * charData) override;

        std::vector<EnhancementTree> DetermineTrees();
        void CreateEnhancementWindows();
        void DestroyEnhancementWindows();
        void UpdateWindowTitle();
        void UpdateTrees(const std::string & enhancementName);
        enum MaxSupportTrees
        {
            MST_Number = 3  // only 3 reaper trees
        };
        CDocument * m_pDocument;
        Character * m_pCharacter;
        std::vector<EnhancementTree> m_availableTrees;
        std::vector<CDialog *> m_treeViews;
        std::vector<size_t> m_visibleTrees; // only some trees will be visible (indexes into m_treeViews)
};
