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
        void AddCharacterHeader(std::stringstream & forumExport);
        void AddAbilityValues(std::stringstream & forumExport, AbilityType ability);
        void AddBreakdown(
                std::stringstream & forumExport,
                const std::string & header,
                size_t width,
                BreakdownType bt);
        void AddSaves(std::stringstream & forumExport);
        void AddFeatSelections(std::stringstream & forumExport);
        void AddSkills(std::stringstream & forumExport);
        void AddEnergyResistances(std::stringstream & forumExport);
        void AddEnergyResistances(std::stringstream & forumExport, const std::string & name, BreakdownType bt1, BreakdownType bt2);
        void AddEnhancements(std::stringstream & forumExport);
        void AddEnhancementTree(std::stringstream & forumExport, const EnhancementSpendInTree & treeSpend);
        void AddEpicDestinyTree(std::stringstream & forumExport, const EpicDestinySpendInTree & treeSpend);
        void AddTwistsOfFate(std::stringstream & forumExport);
        Character m_characterData;
};
