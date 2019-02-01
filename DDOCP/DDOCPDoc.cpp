// DDOCPDoc.cpp
//
#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DDOCP.h"
#endif

#include "DDOCPDoc.h"
#include "ForumExportDlg.h"
#include "MainFrm.h"
#include "SkillSpendDialog.h"
#include "XmlLib\SaxReader.h"
#include "EnhancementEditorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
    const XmlLib::SaxString f_saxElementName = L"DDOCharacterData"; // root element name to look for
}

// CDDOCPDoc
IMPLEMENT_DYNCREATE(CDDOCPDoc, CDocument)

BEGIN_MESSAGE_MAP(CDDOCPDoc, CDocument)
    ON_COMMAND(ID_EDIT_ENHANCEMENTTREEEDITOR, &CDDOCPDoc::OnEditEnhancementTreeEditor)
    ON_COMMAND(ID_FORUMEXPORT_EXPORTTOCLIPBOARD, &CDDOCPDoc::OnForumExportToClipboard)
    ON_COMMAND(ID_EDIT_SKILLPOINTS, &CDDOCPDoc::OnEditSkillPoints)
END_MESSAGE_MAP()

// CDDOCPDoc construction/destruction
CDDOCPDoc::CDDOCPDoc():
    SaxContentElement(f_saxElementName),
    m_characterData(this)
{
}

CDDOCPDoc::~CDDOCPDoc()
{
}

BOOL CDDOCPDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    return TRUE;
}

// CDDOCPDoc serialization
void CDDOCPDoc::Serialize(CArchive& ar)
{
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CDDOCPDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
    // Modify this code to draw the document's data
    dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

    CString strText = _T("TODO: implement thumbnail drawing here");
    LOGFONT lf;

    CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
    pDefaultGUIFont->GetLogFont(&lf);
    lf.lfHeight = 36;

    CFont fontDraw;
    fontDraw.CreateFontIndirect(&lf);

    CFont* pOldFont = dc.SelectObject(&fontDraw);
    dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
    dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CDDOCPDoc::InitializeSearchContent()
{
    CString strSearchContent;
    // Set search contents from document's data. 
    // The content parts should be separated by ";"

    // For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
    SetSearchContent(strSearchContent);
}

void CDDOCPDoc::SetSearchContent(const CString& value)
{
    if (value.IsEmpty())
    {
        RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
    }
    else
    {
        CMFCFilterChunkValueImpl *pChunk = NULL;
        ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
        if (pChunk != NULL)
        {
            pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
            SetChunkValue(pChunk);
        }
    }
}

#endif // SHARED_HANDLERS

// CDDOCPDoc diagnostics

#ifdef _DEBUG
void CDDOCPDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CDDOCPDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CDDOCPDoc commands


BOOL CDDOCPDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    CWaitCursor longOperation;
    // set up a reader with this as the expected root node
    XmlLib::SaxReader reader(this, f_saxElementName);
    // read in the xml from a file (fully qualified path)
    bool ok = reader.Open(lpszPathName);
    if (ok)
    {
        m_characterData.JustLoaded();
    }
    else
    {
        std::string errorMessage = reader.ErrorMessage();
        // document has failed to load. Tell the user what we can about it
        CString text;
        text.Format("The document %s\n"
                "failed to load. The XML parser reported the following problem:\n"
                "\n", lpszPathName);
        text += errorMessage.c_str();
        AfxMessageBox(text, MB_ICONERROR);
    }
    return ok;
}


BOOL CDDOCPDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    bool ok = false;
    try
    {
        XmlLib::SaxWriter writer;
        writer.Open(lpszPathName);
        writer.StartDocument(f_saxElementName);
        m_characterData.Write(&writer);
        writer.EndDocument();
        ok = true;
    }
    catch (const std::exception & e)
    {
        std::string errorMessage = e.what();
        // document has failed to save. Tell the user what we can about it
        CString text;
        text.Format("The document %s\n"
                "failed to save. The XML parser reported the following problem:\n"
                "\n", lpszPathName);
        text += errorMessage.c_str();
        AfxMessageBox(text, MB_ICONERROR);
        ok = false;
    }
    SetModifiedFlag(FALSE);
    return ok;
}

XmlLib::SaxContentElementInterface * CDDOCPDoc::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);
    if (subHandler == NULL)
    {
        if (m_characterData.SaxElementIsSelf(name, attributes))
        {
            subHandler = &m_characterData;
        }
    }

    return subHandler;
}

void CDDOCPDoc::EndElement()
{
    SaxContentElement::EndElement();
}

Character * CDDOCPDoc::GetCharacter()
{
    return &m_characterData;
}

void CDDOCPDoc::OnCloseDocument()
{
    // as the document is being closed, make sure no floating views are
    // referencing it
    CWnd * pWnd = AfxGetMainWnd();
    CMainFrame * pFrame = dynamic_cast<CMainFrame*>(pWnd);
    if (pFrame != NULL)
    {
        pFrame->SetActiveDocumentAndCharacter(NULL, NULL);
    }
    __super::OnCloseDocument();
}

BOOL CDDOCPDoc::SaveModified()
{
    // stop tooltips showing while a possible Save Modified message box displayed
    GetMouseHook()->SaveState();
    BOOL ret = CDocument::SaveModified();
    GetMouseHook()->RestoreState();
    return ret;
}

void CDDOCPDoc::OnEditEnhancementTreeEditor()
{
    // no tooltips while a dialog is displayed
    GetMouseHook()->SaveState();
    CEnhancementEditorDialog dlg(NULL, &m_characterData);
    dlg.DoModal();
    GetMouseHook()->RestoreState();
}

void CDDOCPDoc::OnEditSkillPoints()
{
    // no tooltips while a dialog is displayed
    GetMouseHook()->SaveState();
    CSkillSpendDialog dlg(&m_characterData);
    dlg.DoModal();
    GetMouseHook()->RestoreState();
}

void CDDOCPDoc::OnForumExportToClipboard()
{
    CForumExportDlg dlg(&m_characterData);
    dlg.DoModal();
}
