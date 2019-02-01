// DDOCPDoc.h
//
#pragma once
#include "XmlLib\SaxContentElement.h"
#include "XmlLib\SaxWriter.h"
#include "Character.h"
#include "BreakdownTypes.h"

class CDDOCPDoc :
    public CDocument,
    public XmlLib::SaxContentElement
{
    protected: // create from serialization only
        CDDOCPDoc();
        DECLARE_DYNCREATE(CDDOCPDoc)

        Character * GetCharacter();

    public:
        virtual ~CDDOCPDoc();
        virtual BOOL OnNewDocument() override;
        virtual void Serialize(CArchive& ar) override;
        virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
        virtual BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
        virtual void OnCloseDocument() override;
        virtual BOOL SaveModified() override;
#ifdef SHARED_HANDLERS
        virtual void InitializeSearchContent();
        virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif
        afx_msg void OnEditEnhancementTreeEditor();
        afx_msg void OnForumExportToClipboard();
        afx_msg void OnEditSkillPoints();

    protected:
        DECLARE_MESSAGE_MAP()

        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        void EndElement();
#ifdef SHARED_HANDLERS
        // Helper function that sets search content for a Search Handler
        void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
    private:
        Character m_characterData;
};
