// DDOCP.cpp
//
#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "DDOCP.h"
#include "MainFrm.h"

#include "AboutDlg.h"
#include "ChildFrm.h"
#include "DDOCPDoc.h"
#include "DDOCPView.h"
#include "AugmentsFile.h"
#include "EnhancementsFile.h"
#include "FeatsFile.h"
#include "GlobalSupportFunctions.h"
#include "GuildBuffsFile.h"
#include "ItemsFile.h"
#include "SpellsFile.h"
#include "LocalSettingsStore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only CDDOCPApp object
CDDOCPApp theApp;

// CDDOCPApp
BEGIN_MESSAGE_MAP(CDDOCPApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &CDDOCPApp::OnAppAbout)
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// CDDOCPApp construction
CDDOCPApp::CDDOCPApp()
{
    m_bHiColorIcons = TRUE;

    // support Restart Manager
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
    // If the application is built using Common Language Runtime support (/clr):
    //     1) This additional setting is needed for Restart Manager support to work properly.
    //     2) In your project, you must add a reference to System.Windows.Forms in order to build.
    System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

    // TODO: replace application ID string below with unique ID string; recommended
    // format for string is CompanyName.ProductName.SubProduct.VersionInformation
    SetAppID(_T("DDOCP.AppID.NoVersion"));

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

// CDDOCPApp initialization
BOOL CDDOCPApp::InitInstance()
{
    //  store settings in a file in local ini file, not in the registry
    CSettingsStoreSP::SetRuntimeClass(RUNTIME_CLASS(CLocalSettingsStore));

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    EnableTaskbarInteraction();

    LoadData();
    // AfxInitRichEdit2() is required to use RichEdit control
    // AfxInitRichEdit2();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    SetRegistryKey(_T("DDOBuilder"));
    LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

    //InitContextMenuManager();    // we construct our own custom one
    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(
            AFX_TOOLTIP_TYPE_ALL,
            RUNTIME_CLASS(CMFCToolTipCtrl),
            &ttParams);

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views
    CMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(
            IDR_DDOCPTYPE,
            RUNTIME_CLASS(CDDOCPDoc),
            RUNTIME_CLASS(CChildFrame), // custom MDI child frame
            RUNTIME_CLASS(CDDOCPView));
    if (!pDocTemplate)
    {
        return FALSE;
    }
    AddDocTemplate(pDocTemplate);

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;
    // call DragAcceptFiles only if there's a suffix
    //  In an MDI app, this should occur immediately after setting m_pMainWnd
    // Enable drag/drop open
    m_pMainWnd->DragAcceptFiles();

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Enable DDE Execute open
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);

    // create the additional floating views
    pMainFrame->CreateViews();

    // stop a new document being created if just run normally
    if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
    {
        cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
    }

    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo))
    {
        return FALSE;
    }

    // The main window has been initialized, so show and update it
    LoadState(pMainFrame);
    pMainFrame->UpdateWindow();
    pMainFrame->ShowWindow(m_originalShowState);

    return TRUE;
}

BOOL CDDOCPApp::LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd)
{
    BOOL b = CWinAppEx::LoadWindowPlacement(rectNormalPosition, nFflags, nShowCmd);
    m_originalShowState = nShowCmd;
    nShowCmd = SW_HIDE;
    return b;
}
int CDDOCPApp::ExitInstance()
{
    //TODO: handle additional resources you may have added
    AfxOleTerm(FALSE);

    return CWinAppEx::ExitInstance();
}

// CDDOCPApp message handlers
// App command to run the dialog
void CDDOCPApp::OnAppAbout()
{
    // no tooltips while a dialog is displayed
    GetMouseHook()->SaveState();
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
    GetMouseHook()->RestoreState();
}

// CDDOCPApp customization load/save methods
void CDDOCPApp::PreLoadState()
{
    BOOL bNameValid;
    CString strName;
    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CDDOCPApp::LoadCustomState()
{
}

void CDDOCPApp::SaveCustomState()
{
}

// global data support
void CDDOCPApp::LoadData()
{
    // load all the global data required by the program
    // all data files are in the same directory as the executable
    char fullPath[MAX_PATH];
    ::GetModuleFileName(NULL, fullPath, MAX_PATH);

    char drive[_MAX_DRIVE];
    char folder[_MAX_PATH];
    _splitpath_s(fullPath, drive, _MAX_DRIVE, folder, _MAX_PATH, NULL, 0, NULL, 0);

    char path[_MAX_PATH];
    _makepath_s(path, _MAX_PATH, drive, folder, NULL, NULL);

    LoadFeats(path);
    LoadEnhancements(path);
    LoadSpells(path);
    LoadItems(path);
    LoadAugments(path);
    LoadGuildBuffs(path);
    VerifyFeats();
    VerifyEnhancements();
    VerifyAugments();
    VerifySpells();
    VerifyItems();
    SeparateFeats();
}

void CDDOCPApp::LoadFeats(const std::string & path)
{
    // create the filename to load from
    std::string filename = path;
    filename += "Feats.xml";

    FeatsFile file(filename);
    file.Read();
    m_allFeats = file.Feats();
}

void CDDOCPApp::LoadEnhancements(const std::string & path)
{
    UINT ret = AfxMessageBox("Temporary Startup Option\n"
            "\n"
            "Use U39 (Current Lamannia Build #2) versions of enhancement trees?\n"
            "---Beacon of Hope: New Favord Soul tree\n"
            "---Falconry: New tree\n"
            "---Angel of Vengeance Updated\n"
            "---War Soul Updated\n"
            "---War Priest Updated\n"
            "---All Barbarian Trees updated\n"
            "\n"
            "Yes: Use update 39 trees.\n"
            "No: Use current live server tree versions.",
            MB_ICONQUESTION | MB_YESNO);
    // create the filename to load from
    std::string filename = path;
    if (ret == IDYES)
    {
        // standard U39
        filename += "EnhancementsU39.xml";
    }
    else
    {
        // standard U38
        filename += "Enhancements.xml";
    }

    EnhancementsFile file(filename);
    file.Read();
    m_enhancementTrees = file.EnhancementTrees();
}

void CDDOCPApp::LoadSpells(const std::string & path)
{
    // create the filename to load from
    std::string filename = path;
    filename += "Spells.xml";

    SpellsFile file(filename);
    file.Read();
    m_spells = file.Spells();
}

void CDDOCPApp::LoadItems(const std::string & path)
{
    // create the filename to load from
    std::string itemPath = path;
    itemPath += "Items\\";

    ItemsFile file(itemPath);
    file.ReadFiles();
    m_items = file.Items();
}

void CDDOCPApp::LoadAugments(const std::string & path)
{
    // create the filename to load from
    std::string filename = path;
    filename += "Augments.xml";

    AugmentsFile file(filename);
    file.Read();
    m_augments = file.Augments();
}

void CDDOCPApp::LoadGuildBuffs(const std::string & path)
{
    // create the filename to load from
    std::string filename = path;
    filename += "GuildBuffs.xml";

    GuildBuffsFile file(filename);
    file.Read();
    m_guildBuffs = file.GuildBuffs();
}

void CDDOCPApp::VerifyFeats()
{
    std::list<Feat>::iterator it = m_allFeats.begin();
    while (it != m_allFeats.end())
    {
        (*it).VerifyObject(m_enhancementTrees, m_allFeats);
        ++it;
    }
}

void CDDOCPApp::VerifyEnhancements()
{
    std::list<EnhancementTree>::iterator it = m_enhancementTrees.begin();
    while (it != m_enhancementTrees.end())
    {
        (*it).VerifyObject(m_enhancementTrees, m_allFeats);
        ++it;
    }
}

void CDDOCPApp::VerifyAugments()
{
    std::list<Augment>::iterator it = m_augments.begin();
    while (it != m_augments.end())
    {
        (*it).VerifyObject();
        ++it;
    }
}

void CDDOCPApp::VerifySpells()
{
    std::list<Spell>::iterator it = m_spells.begin();
    while (it != m_spells.end())
    {
        (*it).VerifyObject();
        ++it;
    }
}

void CDDOCPApp::VerifyItems()
{
    std::list<Item>::iterator it = m_items.begin();
    while (it != m_items.end())
    {
        (*it).VerifyObject();
        ++it;
    }
}

void CDDOCPApp::SeparateFeats()
{
    // break each feat out into special sub groups
    std::list<Feat>::iterator it = m_allFeats.begin();
    while (it != m_allFeats.end())
    {
        if ((*it).Acquire() == FeatAcquisition_EpicPastLife)
        {
            m_epicPastLifeFeats.push_back((*it));
        }
        else if ((*it).Acquire() == FeatAcquisition_HeroicPastLife)
        {
            m_heroicPastLifeFeats.push_back((*it));
        }
        else if ((*it).Acquire() == FeatAcquisition_RacialPastLife)
        {
            m_racialPastLifeFeats.push_back((*it));
        }
        else if ((*it).Acquire() == FeatAcquisition_IconicPastLife)
        {
            m_iconicPastLifeFeats.push_back((*it));
        }
        else if ((*it).Acquire() == FeatAcquisition_Special)
        {
            m_specialFeats.push_back((*it));
        }
        else
        {
            // has to be a bog standard feat
            m_standardFeats.push_back((*it));
        }
        ++it;
    }
    // now sort them in to order
    m_epicPastLifeFeats.sort();
    m_heroicPastLifeFeats.sort();
    m_racialPastLifeFeats.sort();
    m_iconicPastLifeFeats.sort();
    m_specialFeats.sort();
    m_standardFeats.sort();
}

const std::list<Feat> & CDDOCPApp::AllFeats() const
{
    return m_allFeats;
}

const std::list<Feat> & CDDOCPApp::StandardFeats() const
{
    return m_standardFeats;
}

const std::list<Feat> & CDDOCPApp::HeroicPastLifeFeats() const
{
    return m_heroicPastLifeFeats;
}

const std::list<Feat> & CDDOCPApp::RacialPastLifeFeats() const
{
    return m_racialPastLifeFeats;
}

const std::list<Feat> & CDDOCPApp::IconicPastLifeFeats() const
{
    return m_iconicPastLifeFeats;
}

const std::list<Feat> & CDDOCPApp::EpicPastLifeFeats() const
{
    return m_epicPastLifeFeats;
}

const std::list<Feat> & CDDOCPApp::SpecialFeats() const
{
    return m_specialFeats;
}

const std::list<EnhancementTree> & CDDOCPApp::EnhancementTrees() const
{
    return m_enhancementTrees;
}

const std::list<Spell> & CDDOCPApp::Spells() const
{
    return m_spells;
}

const std::list<Item> & CDDOCPApp::Items() const
{
    return m_items;
}

const std::list<Augment> & CDDOCPApp::Augments() const
{
    return m_augments;
}

const std::list<GuildBuff> & CDDOCPApp::GuildBuffs() const
{
    return m_guildBuffs;
}

// CDDOCPApp message handlers


