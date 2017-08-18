// SpellsView.h
//
#pragma once
#include "Resource.h"
#include <vector>
#include "Character.h"

class CSpellsPage;

class CSpellsView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_SPELLS_VIEW };
        DECLARE_DYNCREATE(CSpellsView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
    protected:
        CSpellsView();           // protected constructor used by dynamic creation
        virtual ~CSpellsView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

        // CharacterObserver overrides
        virtual void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;
    private:
        void DetermineSpellViews();

        CPropertySheet m_spellsSheet;
        std::vector<CPropertyPage*> m_pagePointers;
        CDocument * m_pDocument;
        Character * m_pCharacter;
};
