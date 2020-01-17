// SpellsView.cpp
//
#include "stdafx.h"
#include "SpellsView.h"
#include "GlobalSupportFunctions.h"
#include <numeric>

#include "SpellsPage.h"
#include "SpellLikeAbilityPage.h"

namespace
{
    const int c_controlSpacing = 3;
    const UINT UWM_NEW_DOCUMENT = ::RegisterWindowMessage(_T("NewActiveDocument"));
}

IMPLEMENT_DYNCREATE(CSpellsView, CFormView)

CSpellsView::CSpellsView() :
    CFormView(CSpellsView::IDD),
    m_pCharacter(NULL),
    m_pDocument(NULL)
{
    // a NULL pointer for every class
    // we maintain this vector at this size to avoid page re-creation/destruction
    // on each class change
    m_pagePointers.resize(Class_Count + 1, NULL);
}

CSpellsView::~CSpellsView()
{
    for (size_t i = 0; i < m_pagePointers.size(); ++ i)
    {
        delete m_pagePointers[i];
        m_pagePointers[i] = NULL;
    }
}

void CSpellsView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CSpellsView, CFormView)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_REGISTERED_MESSAGE(UWM_NEW_DOCUMENT, OnNewDocument)
END_MESSAGE_MAP()
#pragma warning(pop)

#ifdef _DEBUG
void CSpellsView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSpellsView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CSpellsView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    // Create the property sheet ready for adding the pages to
    m_spellsSheet.m_psh.dwFlags |= PSH_HASHELP;
    m_spellsSheet.Construct("", this);

    // must have at least 1 page to construct correctly (dummy page added)
    DetermineSpellViews();

    // Display the property sheet
    m_spellsSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
    // it looks like this line is needed to make sure PrintPreview does not hang
    // when a ShowWindow(SW_HIDE) gets called within the guts of the MFC code.
    // As the code is looking for a window to deactivate/activate it cycles through
    // controls starting from the active window. As the property sheet is a child
    // of this view, it needs to be included in the tab ordering of controls so
    // that the MFC iteration can make it to the correct item without becoming stuck.
    m_spellsSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
}

void CSpellsView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (IsWindow(m_spellsSheet.GetSafeHwnd()))
    {
        // takes up the entire space
        CRect rctSheet(0, 0, cx, cy);
        m_spellsSheet.MoveWindow(rctSheet);
        CRect tabRect(0, 0, rctSheet.Width(), rctSheet.Height());
        m_spellsSheet.GetTabControl()->MoveWindow(tabRect);
        m_spellsSheet.GetTabControl()->AdjustRect(FALSE, &tabRect);
        m_spellsSheet.GetActivePage()->MoveWindow(tabRect);
    }
}

LRESULT CSpellsView::OnNewDocument(WPARAM wParam, LPARAM lParam)
{
    if (m_pCharacter != NULL)
    {
        m_pCharacter->DetachObserver(this);
    }

    // wParam is the document pointer
    CDocument * pDoc = (CDocument*)(wParam);
    m_pDocument = pDoc;
    // lParam is the character pointer
    Character * pCharacter = (Character *)(lParam);
    m_pCharacter = pCharacter;
    if (m_pCharacter != NULL)
    {
        m_pCharacter->AttachObserver(this);
    }
    DetermineSpellViews();
    // set the character for each page displayed
    for (size_t i = 0; i < m_pagePointers.size(); ++i)
    {
        if (m_pagePointers[i] != NULL)
        {
            CSpellLikeAbilityPage * pSLAPage = dynamic_cast<CSpellLikeAbilityPage*>(m_pagePointers[i]);
            if (pSLAPage != NULL)
            {
                pSLAPage->SetCharacter(m_pCharacter);
            }
            CSpellsPage * pSpellPage = dynamic_cast<CSpellsPage*>(m_pagePointers[i]);
            if (pSpellPage != NULL)
            {
                pSpellPage->SetCharacter(m_pCharacter);
            }
        }
    }
    return 0L;
}

BOOL CSpellsView::OnEraseBkgnd(CDC* pDC)
{
    static int controlsNotToBeErased[] =
    {
        0 // end marker
    };

    return OnEraseBackground(this, pDC, controlsNotToBeErased);
}

void CSpellsView::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    // if a class has changed or the number of levels in a given
    // class has changed then we may need to change which spell selection
    // views are displayed
    DetermineSpellViews();
}

void CSpellsView::DetermineSpellViews()
{
    // first lock the window and destroy any existing views
    if (IsWindow(m_spellsSheet.GetSafeHwnd()))
    {
        m_spellsSheet.LockWindowUpdate();
    }

    // get the number of pages already inserted
    size_t numPages = m_spellsSheet.GetPageCount();

    // we have a page for spell like abilities
    if (m_pagePointers[0] == NULL)  // 0 is in effect Class_Unknown
    {
        CSpellLikeAbilityPage * page = new CSpellLikeAbilityPage();
        m_pagePointers[0] = page;
        m_spellsSheet.AddPage(page);
    }

    // now determine the class pages to add to the view
    std::vector<size_t> classLevels;
    if (m_pCharacter != NULL)
    {
        classLevels = m_pCharacter->ClassLevels(MAX_LEVEL);
    }
    else
    {
        // no levels as no character
        classLevels.resize(Class_Count, 0); // 0 levels in each
    }
    for (size_t ci = Class_Unknown + 1; ci < Class_Count; ++ci)
    {
        // get the number of spells available for this class at this level
        std::vector<size_t> spellSlots = SpellSlotsForClass((ClassType)ci, classLevels[ci]);
        // must have at least 1 spell slot at any level to display the
        // required spell selection tab for this class
        size_t slotCount = std::accumulate(spellSlots.begin(), spellSlots.end(), 0);
        if (slotCount > 0)
        {
            // yes there are spell slots for this class, create the controlling
            // window for it
            // does this spell page already exist?
            if (m_pagePointers[ci] == NULL)
            {
                CSpellsPage * page = NULL;
                switch (ci)
                {
                case Class_Alchemist:
                    page = new CSpellsPage(Class_Alchemist, Ability_Intelligence, IDS_ALCHEMIST_SPELLS);
                    break;
                case Class_Artificer:
                    page = new CSpellsPage(Class_Artificer, Ability_Intelligence, IDS_ARTIFICER_SPELLS);
                    break;
                case Class_Bard:
                    page = new CSpellsPage(Class_Bard, Ability_Charisma, IDS_BARD_SPELLS);
                    break;
                case Class_Cleric:
                    page = new CSpellsPage(Class_Cleric, Ability_Wisdom, IDS_CLERIC_SPELLS);
                    break;
                case Class_Druid:
                    page = new CSpellsPage(Class_Druid, Ability_Wisdom, IDS_DRUID_SPELLS);
                    break;
                case Class_FavoredSoul:
                    page = new CSpellsPage(Class_FavoredSoul, Ability_Wisdom, IDS_FAVOREDSOUL_SPELLS);
                    break;
                case Class_Paladin:
                    page = new CSpellsPage(Class_Paladin, Ability_Wisdom, IDS_PALADIN_SPELLS);
                    break;
                case Class_Ranger:
                    page = new CSpellsPage(Class_Ranger, Ability_Wisdom, IDS_RANGER_SPELLS);
                    break;
                case Class_Sorcerer:
                    page = new CSpellsPage(Class_Sorcerer, Ability_Charisma, IDS_SORCERER_SPELLS);
                    break;
                case Class_Warlock:
                    page = new CSpellsPage(Class_Warlock, Ability_Charisma, IDS_WARLOCK_SPELLS);
                    break;
                case Class_Wizard:
                    page = new CSpellsPage(Class_Wizard, Ability_Intelligence, IDS_WIZARD_SPELLS);
                    break;
                default:
                    ASSERT(FALSE);  // not handled for this class yet
                    break;
                }
                if (page != NULL)
                {
                    // add the page
                    m_pagePointers[ci] = page;
                    m_spellsSheet.AddPage(page);
                    page->SetCharacter(m_pCharacter);
                    page->SetTrainableSpells(spellSlots);
                }
            }
            else
            {
                // ensure the page is updated with the correct number of spells
                CSpellsPage * page = dynamic_cast<CSpellsPage *>(m_pagePointers[ci]);
                if (page != NULL)
                {
                    page->SetTrainableSpells(spellSlots);
                }
            }
        }
        else
        {
            // no spells for this class anymore, remove its page if present
            if (m_pagePointers[ci] != NULL
                    && ci != Class_Unknown)
            {
                // work out the page index
                for (size_t i = 0; i < numPages; ++i)
                {
                    if (m_spellsSheet.GetPage(i) == m_pagePointers[ci])
                    {
                        // this is the one to remove
                        m_spellsSheet.RemovePage(i);
                        break; // skip the rest
                    }
                }
                // release the page memory
                delete m_pagePointers[ci];
                m_pagePointers[ci] = NULL;
            }
        }
    }
    if (IsWindow(m_spellsSheet.GetSafeHwnd()))
    {
        m_spellsSheet.UnlockWindowUpdate();
    }
}

const CSLAControl * CSpellsView::GetSLAControl()
{
    const CSLAControl * slaControl = NULL;
    // the SLA control is in the SLA page view, which is always page 0
    if (m_pagePointers.size() > 0)
    {
        CSpellLikeAbilityPage * pSLAPage =
                dynamic_cast<CSpellLikeAbilityPage*>(m_pagePointers[0]);
        if (pSLAPage != NULL)
        {
            slaControl = pSLAPage->GetSLAControl();
        }
    }
    return slaControl;
}

const CSpellsControl * CSpellsView::GetSpellsControl(ClassType classType)
{
    // look through the page pointers to find the one for this class
    const CSpellsControl * pSC = NULL;
    if (m_pagePointers[classType] != NULL)
    {
        CSpellsPage * page = dynamic_cast<CSpellsPage *>(m_pagePointers[classType]);
        pSC = page->SpellsControl();
    }
    return pSC;
}
