// DDOCPDoc.cpp
//
#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DDOCP.h"
#endif

#include "BreakdownItem.h"
#include "BreakdownItemMRR.h"
#include "BreakdownItemSave.h"
#include "BreakdownItemWeaponEffects.h"
#include "DDOCPDoc.h"
#include "GlobalSupportFunctions.h"
#include "MainFrm.h"
#include "SkillSpendDialog.h"
#include <propkey.h>
#include "XmlLib\SaxReader.h"
#include "EnhancementEditorDialog.h"
#include <numeric>

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
    std::stringstream forumExport;
    forumExport << "[code]\r\n";
    AddCharacterHeader(forumExport);
    AddSpecialFeats(forumExport);
    AddSaves(forumExport);
    AddEnergyResistances(forumExport);
    AddFeatSelections(forumExport);
    AddAutomaticFeats(forumExport);
    AddSkills(forumExport);
    AddEnhancements(forumExport);
    AddTwistsOfFate(forumExport);
    AddSpellPowers(forumExport);
    AddSpells(forumExport);
    AddWeaponDamage(forumExport);
    AddGear(forumExport);
    forumExport << "[/code]\r\n";

    CString clipboardText = forumExport.str().c_str();
    // now place the text on the clipboard
    if (OpenClipboard(NULL))
    {
        HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, clipboardText.GetLength()+1);
        ASSERT(clipbuffer != NULL);
        char *buffer = (char*)GlobalLock(clipbuffer);
        strcpy_s(buffer, clipboardText.GetLength()+1, clipboardText);
        GlobalUnlock(clipbuffer);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, clipbuffer);
        CloseClipboard();
    }
}

void CDDOCPDoc::AddCharacterHeader(std::stringstream & forumExport)
{
    // Character Name: [.................................]
    // Classes: [x][class1], [y][class2], [z][class3] and [10]Epic
    // Race: [...............] Alignment: [..............]
    //
    //      Start Tome Final
    // Str: [...] [..] [...]     HP:    [....]       AC: [...]
    // Dex: [...] [..] [...]     PRR:   [....]       DR: [........................]
    // Con: [...] [..] [...]     MRR:   [....]       +Healing Amp: [...]
    // Int: [...] [..] [...]     Dodge: [...]%/[MAX] -Healing Amp: [...]
    // Wis: [...] [..] [...]     Fort:  [...]%       Repair Amp:   [...]
    // Cha: [...] [..] [...]     SR:    [...]        BAB:   [..]

     // first line is the character name
    forumExport << "Character name: " << m_characterData.Name() << "\r\n";

    // Classes line
    forumExport << "Classes: " << m_characterData.GetBuildDescription() << "\r\n";

    // Race/Alignment line
    std::string race = EnumEntryText(m_characterData.Race(), raceTypeMap);
    forumExport << "Race: ";
    forumExport.fill(' ');
    forumExport.width(21);
    forumExport << std::left << race;
    std::string alignment = EnumEntryText(m_characterData.Alignment(), alignmentTypeMap);
    forumExport << "Alignment: ";
    forumExport << std::left << alignment << "\r\n";
    // blank line
    forumExport << "\r\n";
    forumExport << "     Start Tome Final\r\n";

    AddAbilityValues(forumExport, Ability_Strength);
    AddBreakdown(forumExport, "      HP: ", 10, Breakdown_Hitpoints);
    AddBreakdown(forumExport, "      AC: ", 5, Breakdown_AC);
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Dexterity);
    AddBreakdown(forumExport, "      PRR: ", 9, Breakdown_PRR);
    //AddBreakdown(forumExport, "      AC: ", 5, Breakdown_AC); // TBD DR BREAKDOWN
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Constitution);
    AddBreakdown(forumExport, "      MRR: ", 9, Breakdown_MRR);
    AddBreakdown(forumExport, "     +Healing Amp: ", 5, Breakdown_HealingAmplification);
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Intelligence);
    AddBreakdown(forumExport, "      Dodge: ", 4, Breakdown_Dodge);
    AddBreakdown(forumExport, "/", 1, Breakdown_DodgeCap);
    AddBreakdown(forumExport, "      -Healing Amp: ", 5, Breakdown_NegativeHealingAmplification);
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Wisdom);
    AddBreakdown(forumExport, "      Fort: ", 7, Breakdown_Fortification);
    AddBreakdown(forumExport, "%      Repair Amp:   ", 5, Breakdown_RepairAmplification);
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Charisma);
    AddBreakdown(forumExport, "      SR: ", 10, Breakdown_SpellResistance);
    forumExport << "\r\n";

    BreakdownItem * pBIMRR = FindBreakdown(Breakdown_MRR);
    BreakdownItem * pBIMRRCap = FindBreakdown(Breakdown_MRRCap);
    if (pBIMRRCap->Value() != "None"
            && pBIMRRCap->Total() < pBIMRR->Total())
    {
        forumExport << "*MRR Capped at " << pBIMRRCap->Total() << " due to armor worn.";
    }
    forumExport << "\r\n\r\n";
}

void CDDOCPDoc::AddSpecialFeats(std::stringstream & forumExport)
{
    // add the special feats (past lives and other feats such as inherent)
    std::list<TrainedFeat> feats = m_characterData.SpecialFeats().Feats();
    if (feats.size() > 0)
    {
        forumExport << "Past Lives and Special Feats\r\n";
        if (feats.size() > 1)
        {
            // combine duplicates in the list
            std::list<TrainedFeat>::iterator it = feats.begin();
            while (it != feats.end())
            {
                // look at all following items and combine if required
                std::list<TrainedFeat>::iterator nit = it;
                ++nit;          // start from next element
                while (nit != feats.end())
                {
                    if ((*it) == (*nit))
                    {
                        // need to combine these elements
                        (*it).IncrementCount();
                        nit = feats.erase(nit);
                    }
                    else
                    {
                        // check the next one
                        ++nit;
                    }
                }
                ++it;
            }
        }
        // sort the feats before output
        feats.sort();
        AddFeats(forumExport, "Heroic Past Lives", TFT_HeroicPastLife, feats);
        AddFeats(forumExport, "Racial Past Lives", TFT_RacialPastLife, feats);
        AddFeats(forumExport, "Iconic Past Lives", TFT_IconicPastLife, feats);
        AddFeats(forumExport, "Epic Past Lives", TFT_EpicPastLife, feats);
        AddFeats(forumExport, "Special Feats", TFT_SpecialFeat, feats);
        forumExport << "\r\n";
    }
}

void CDDOCPDoc::AddFeats(
        std::stringstream & forumExport,
        const std::string & heading,
        TrainableFeatTypes type,
        const std::list<TrainedFeat> & feats) const
{
    bool first = true;
    std::list<TrainedFeat>::const_iterator it = feats.begin();
    while (it != feats.end())
    {
        if ((*it).Type() == type)
        {
            if (first)
            {
                forumExport << heading << "\r\n";
                forumExport << "------------------------------------------------------------------------------------------\r\n";
                first = false;
            }
            // add this one to the export
            forumExport << (*it).FeatName();
            if ((*it).Count() > 1)
            {
                forumExport << "(" << (*it).Count() << ")";
            }
            forumExport << "\r\n";
        }
        ++it;
    }
    if (!first)
    {
        forumExport << "\r\n";
    }
}

void CDDOCPDoc::AddSaves(std::stringstream & forumExport)
{
    forumExport << "Saves:\r\n";
    AddBreakdown(forumExport, "Fortitude:        ", 3, Breakdown_SaveFortitude);
    AddBreakdown(forumExport, "\r\n  vs Poison:      ", 3, Breakdown_SavePoison);
    AddBreakdown(forumExport, "\r\n  vs Disease:     ", 3, Breakdown_SaveDisease);
    forumExport << "\r\n";

    AddBreakdown(forumExport, "Will:             ", 3, Breakdown_SaveWill);
    AddBreakdown(forumExport, "\r\n  vs Enchantment: ", 3, Breakdown_SaveEnchantment);
    AddBreakdown(forumExport, "\r\n  vs Illusion:    ", 3, Breakdown_SaveIllusion);
    AddBreakdown(forumExport, "\r\n  vs Fear:        ", 3, Breakdown_SaveFear);
    AddBreakdown(forumExport, "\r\n  vs Curse:       ", 3, Breakdown_SaveCurse);
    forumExport << "\r\n";

    AddBreakdown(forumExport, "Reflex:           ", 3, Breakdown_SaveReflex);
    AddBreakdown(forumExport, "\r\n  vs Traps:       ", 3, Breakdown_SaveTraps);
    AddBreakdown(forumExport, "\r\n  vs Spell:       ", 3, Breakdown_SaveSpell);
    AddBreakdown(forumExport, "\r\n  vs Magic:       ", 3, Breakdown_SaveMagic);
    forumExport << "\r\n";

    forumExport << "Marked with a * is no fail on a 1 if required DC met\r\n";
    forumExport << "\r\n";
}

void CDDOCPDoc::AddAbilityValues(std::stringstream & forumExport, AbilityType ability)
{
    size_t baseValue = m_characterData.BaseAbilityValue(ability);
    BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
    size_t buffedValue = (size_t)pBI->Total();      // whole numbers only
    size_t tomeValue = m_characterData.TomeAtLevel(ability, MAX_LEVEL-1);
    // get the stat name and limit to first 3 character
    std::string name = (EnumEntryText(ability, abilityTypeMap));
    name.resize(3);
    forumExport.width(3);
    forumExport << name;
    forumExport << ": ";
    forumExport.width(5);
    forumExport << std::right << baseValue << " ";
    forumExport.width(4);
    forumExport << std::right << tomeValue << " ";
    forumExport.width(5);
    forumExport << std::right << buffedValue;
}

void CDDOCPDoc::AddBreakdown(
        std::stringstream & forumExport,
        const std::string & header,
        size_t width,
        BreakdownType bt)
{
    BreakdownItem * pBI = FindBreakdown(bt);
    size_t value = (size_t)pBI->Total();      // whole numbers only
    forumExport << header;
    forumExport.width(width);
    forumExport << std::right << value;
    BreakdownItemSave * pBIS = dynamic_cast<BreakdownItemSave *>(pBI);
    if (pBIS != NULL)
    {
        if (pBIS->HasNoFailOn1())
        {
            forumExport << "*";
        }
    }
    if (bt == Breakdown_MRR)
    {
        BreakdownItem * pBIMRR = FindBreakdown(Breakdown_MRR);
        BreakdownItem * pBIMRRCap = FindBreakdown(Breakdown_MRRCap);
        if (pBIMRRCap->Total() > 0
                && pBIMRRCap->Total() < pBIMRR->Total())
        {
            forumExport << "*";     // notify it's capped
        }
        else
        {
            forumExport << " ";
        }
    }
}

void CDDOCPDoc::AddFeatSelections(std::stringstream & forumExport)
{
    forumExport << "Level Class            Feats\r\n";
    for (size_t level = 0; level < MAX_LEVEL; ++level)
    {
        const LevelTraining & levelData = m_characterData.LevelData(level);
        if (level == 0)
        {
            bool requiresHeartOfWood = false;
            ClassType expectedClass = levelData.HasClass() ? levelData.Class() : Class_Unknown;
            switch (m_characterData.Race())
            {
            case Race_AasimarScourge: requiresHeartOfWood = (expectedClass != Class_Ranger); break;
            case Race_BladeForged: requiresHeartOfWood = (expectedClass != Class_Paladin); break;
            case Race_DeepGnome: requiresHeartOfWood = (expectedClass != Class_Wizard); break;
            case Race_Morninglord: requiresHeartOfWood = (expectedClass != Class_Cleric); break;
            case Race_PurpleDragonKnight: requiresHeartOfWood = (expectedClass != Class_Fighter); break;
            case Race_ShadarKai: requiresHeartOfWood = (expectedClass != Class_Rogue); break;
            }
            if (requiresHeartOfWood)
            {
                forumExport << "                       Level 1 Requires a +1 Heart of Wood to switch out of Iconic Class\r\n";
            }
        }
        std::vector<size_t> classLevels = m_characterData.ClassLevels(level);
        CString className;
        className.Format("%s(%d)",
                EnumEntryText(levelData.HasClass() ? levelData.Class() : Class_Unknown, classTypeMap),
                levelData.HasClass() ? classLevels[levelData.Class()] : 0);
        forumExport.width(3);
        forumExport << std::right << (level + 1) << "   ";
        forumExport.fill(' ');
        forumExport.width(17);
        forumExport << std::left << className;
        // now add the trainable feat types and their selections
        std::vector<TrainableFeatTypes> trainable = m_characterData.TrainableFeatTypeAtLevel(level);
        if (trainable.size() > 0)
        {
            for (size_t tft = 0; tft < trainable.size(); ++tft)
            {
                CString label = TrainableFeatTypeLabel(trainable[tft]);
                label += ": ";
                TrainedFeat tf = m_characterData.GetTrainedFeat(
                        level,
                        trainable[tft]);
                if (tf.FeatName().empty())
                {
                    label += "Empty Feat Slot";
                }
                else
                {
                    label += tf.FeatName().c_str();
                    if (tf.HasFeatSwapWarning())
                    {
                        label += " (Requires Feat Swap with Fred)";
                    }
                }
                if (tft > 0)
                {
                    forumExport << "                       ";
                }
                forumExport << label;
                forumExport << "\r\n";
            }
        }
        // also need to show ability adjustment on every 4th level
        AbilityType ability = m_characterData.AbilityLevelUp(level + 1);
        if (ability != Ability_Unknown)
        {
            if (trainable.size() > 0)
            {
                forumExport << "                       ";
            }
            forumExport << EnumEntryText(ability, abilityTypeMap);
            forumExport << ": +1 Level up\r\n";
        }
        else if (trainable.size() == 0)
        {
            forumExport << "\r\n";
        }
    }
    if (m_characterData.HasGrantedFeats())
    {
        forumExport << "------------------------------------------------------------------------------------------\r\n";
        forumExport << "Granted Feats\r\n";
        const std::list<TrainedFeat> & grantedFeats = m_characterData.GrantedFeats();
        std::list<TrainedFeat>::const_iterator it = grantedFeats.begin();
        while (it != grantedFeats.end())
        {
            forumExport << (*it).FeatName();
            forumExport << "\r\n";
            ++it;
        }
        forumExport << "------------------------------------------------------------------------------------------\r\n";
    }
    // blank line after
    forumExport << "\r\n";
}

void CDDOCPDoc::AddAutomaticFeats(std::stringstream & forumExport)
{
    // list all the automatic feats gained at each level
    forumExport << "Level Class            Feats\r\n";
    for (size_t level = 0; level < MAX_LEVEL; ++level)
    {
        const LevelTraining & levelData = m_characterData.LevelData(level);
        std::vector<size_t> classLevels = m_characterData.ClassLevels(level);
        CString className;
        className.Format("%s(%d)",
                EnumEntryText(levelData.HasClass() ? levelData.Class() : Class_Unknown, classTypeMap),
                levelData.HasClass() ? classLevels[levelData.Class()] : 0);
        forumExport.width(3);
        forumExport << std::right << (level + 1) << "   ";
        forumExport.fill(' ');
        forumExport.width(17);
        forumExport << std::left << className;
        // now add the automatic feats
        const FeatsListObject & automaticFeats = levelData.AutomaticFeats();
        const std::list<TrainedFeat> & feats = automaticFeats.Feats();
        std::list<TrainedFeat>::const_iterator it = feats.begin();
        bool first = true;
        while (it != feats.end())
        {
            if (!first)
            {
                forumExport << "                       ";
            }
            forumExport << (*it).FeatName();
            forumExport << "\r\n";
            ++it;
            first = false;
        }
        if (first)
        {
            forumExport << "\r\n";
        }
    }
    forumExport << "\r\n";
}

void CDDOCPDoc::AddSkills(std::stringstream & forumExport)
{
    // Example Output:
    // Skill Points     18  9  9  9 10 10 10 10 10 10 11 11 11 11 11 12 12 12 12 12
    // Skill Name       01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 Total Buffed
    // -----------------------------------------------------------------------------------------
    // Disable Device    4  1        3                                              xxx   xxx
    // Use Magic Device                                                             xxx   xxx
    // -----------------------------------------------------------------------------------------
    // Available Points 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    forumExport << "Skill Points    ";
    for (size_t level = 0; level < MAX_CLASS_LEVEL; ++level)
    {
        const LevelTraining & levelData = m_characterData.LevelData(level);
        forumExport.width(3);
        forumExport << std::right << levelData.SkillPointsAvailable();
    }
    forumExport << "\r\n";
    forumExport << "Skill Name       01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20  Total Buffed\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    for (size_t skill = Skill_Unknown + 1; skill < Skill_Count; ++skill)
    {
        CString skillName;
        skillName.Format("%-16s", EnumEntryText((SkillType)skill, skillTypeMap));
        forumExport << skillName;
        // now add the number of skill points spent on this skill at each heroic level
        for (size_t level = 0; level < MAX_CLASS_LEVEL; ++level)
        {
            const LevelTraining & levelData = m_characterData.LevelData(level);
            const std::list<TrainedSkill> & ts = levelData.TrainedSkills();
            std::list<TrainedSkill>::const_iterator it = ts.begin();
            std::vector<size_t> skillRanks(Skill_Count, 0);
            while (it != ts.end())
            {
                skillRanks[(*it).Skill()]++;
                ++it;
            }
            CString text;
            if (skillRanks[skill] > 0)
            {
                ClassType ct = m_characterData.LevelData(level).HasClass()
                        ? m_characterData.LevelData(level).Class()
                        : Class_Unknown;
                if (IsClassSkill(ct, (SkillType)skill))
                {
                    text.Format("%d", skillRanks[skill]);
                }
                else
                {
                    text = "";
                    int fullRanks = (skillRanks[skill] / 2);
                    if (fullRanks > 0)
                    {
                        text.Format("%d", fullRanks);
                    }
                    // its a cross class skill, show in multiples of ½
                    if (skillRanks[skill] % 2 != 0)
                    {
                        text += "½";
                    }
                }
                forumExport.fill(' ');
                forumExport.width(3);
                forumExport << std::right << text;
            }
            else
            {
                // no trained ranks at this level, blank field
                forumExport << "   ";
            }
        }
        double total = m_characterData.SkillAtLevel((SkillType)skill, MAX_CLASS_LEVEL, false);
        forumExport.precision(1);          // max 1 dp
        forumExport.width(7);              // xxx.y
        forumExport << std::fixed << std::right << total;
        // now add the total for this skill
        BreakdownType bt = SkillToBreakdown((SkillType)skill);
        BreakdownItem * pBI = FindBreakdown(bt);
        forumExport.width(7);              // xxx.y
        forumExport << std::fixed << std::right << pBI->Total();
        forumExport << "\r\n";     // end of this skill line
    }
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    // show how many available skill points there are to spend
    forumExport << "Available Points";
    for (size_t level = 0; level < MAX_CLASS_LEVEL; ++level)
    {
        const LevelTraining & levelData = m_characterData.LevelData(level);
        forumExport.width(3);
        int available = levelData.SkillPointsAvailable();
        int spent = levelData.SkillPointsSpent();
        forumExport << std::right << (available - spent);
    }
    forumExport << "\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    // blank line after
    forumExport << "\r\n";
}

void CDDOCPDoc::AddEnergyResistances(std::stringstream & forumExport)
{
    // Energy Resistance/Absorbance
    // Energy       Resistance and Absorbance
    // --------------------------------------
    // Acid:             [...]         [...]%
    // Cold:             [...]         [...]%
    // Electric:         [...]         [...]%
    // Fire:             [...]         [...]%
    // Sonic:            [...]         [...]%
    // Light:            [...]         [...]%
    // Negative:         [...]         [...]%
    // Poison:           [...]         [...]%
    // etc
    forumExport << "Energy       Resistance and Absorbance\r\n";
    forumExport << "--------------------------------------\r\n";
    AddEnergyResistances(forumExport, "Acid:", Breakdown_EnergyResistanceAcid, Breakdown_EnergyAbsorptionAcid);
    AddEnergyResistances(forumExport, "Cold:", Breakdown_EnergyResistanceCold, Breakdown_EnergyAbsorptionCold);
    AddEnergyResistances(forumExport, "Electric:", Breakdown_EnergyResistanceElectric, Breakdown_EnergyAbsorptionElectric);
    AddEnergyResistances(forumExport, "Fire:", Breakdown_EnergyResistanceFire, Breakdown_EnergyAbsorptionFire);
    AddEnergyResistances(forumExport, "Force:", Breakdown_EnergyResistanceForce, Breakdown_EnergyAbsorptionForce);
    AddEnergyResistances(forumExport, "Light:", Breakdown_EnergyResistanceLight, Breakdown_EnergyAbsorptionLight);
    AddEnergyResistances(forumExport, "Negative:", Breakdown_EnergyResistanceNegative, Breakdown_EnergyAbsorptionNegative);
    AddEnergyResistances(forumExport, "Poison:", Breakdown_EnergyResistancePoison, Breakdown_EnergyAbsorptionPoison);
    AddEnergyResistances(forumExport, "Positive:", Breakdown_EnergyResistancePositive, Breakdown_EnergyAbsorptionPositive);
    AddEnergyResistances(forumExport, "Repair:", Breakdown_EnergyResistanceRepair, Breakdown_EnergyAbsorptionRepair);
    AddEnergyResistances(forumExport, "Rust:", Breakdown_EnergyResistanceRust, Breakdown_EnergyAbsorptionRust);
    AddEnergyResistances(forumExport, "Sonic:", Breakdown_EnergyResistanceSonic, Breakdown_EnergyAbsorptionSonic);
    forumExport << "\r\n";
}

void CDDOCPDoc::AddEnergyResistances(
        std::stringstream & forumExport,
        const std::string & name,
        BreakdownType bt1,
        BreakdownType bt2)
{
    forumExport.width(13);
    forumExport.fill(' ');
    forumExport << std::left << name;
    BreakdownItem * pB1 = FindBreakdown(bt1);
    forumExport.width(10);
    forumExport << std::right << (int)pB1->Total();
    forumExport << "    ";
    BreakdownItem * pB2 = FindBreakdown(bt2);
    forumExport.width(10);
    forumExport << std::right << (int)pB2->Total();
    forumExport << "%\r\n";
}

void CDDOCPDoc::AddEnhancements(std::stringstream & forumExport)
{
    forumExport << "Enhancements: 80 APs";
    if (m_characterData.BonusActionPoints() > 0)
    {
        forumExport << " and ";
        forumExport.width(1);
        forumExport << m_characterData.BonusActionPoints();
        forumExport << " Racial APs";
    }
    forumExport << "\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    const SelectedEnhancementTrees & trees = m_characterData.SelectedTrees();
    for (size_t ti = 0; ti < MAX_ENHANCEMENT_TREES; ++ti)
    {
        const std::string & treeName = trees.Tree(ti);
        if (!SelectedEnhancementTrees::IsNotSelected(treeName))
        {
            // this is a tree we have to list
            EnhancementSpendInTree * treeSpend = m_characterData.Enhancement_FindTree(treeName);
            if (treeSpend != NULL
                    && treeSpend->Spent() > 0)
            {
                // tree can be selected and have no enhancements trained, thus not be present
                AddEnhancementTree(forumExport, *treeSpend);
            }
        }
    }

    // now add the active epic destiny tree
    std::string activeDestiny = m_characterData.ActiveEpicDestiny();
    if (activeDestiny != "")
    {
        forumExport << "\r\n";
        forumExport << "Active Destiny Tree\r\n";
        forumExport << "------------------------------------------------------------------------------------------\r\n";
        // this is a tree we have to list
        EpicDestinySpendInTree * treeSpend = m_characterData.EpicDestiny_FindTree(activeDestiny);
        if (treeSpend != NULL)
        {
            // tree can be selected and have no enhancements trained, thus not be present
            AddEpicDestinyTree(forumExport, *treeSpend);
        }
    }
}

void CDDOCPDoc::AddEnhancementTree(
        std::stringstream & forumExport,
        const EnhancementSpendInTree & treeSpend)
{
    // TreeName: <name> - Points spent : xxx
    // <List of enhancements by display name>
    forumExport << treeSpend.TreeName() << " - Points spent: " << treeSpend.Spent() << "\r\n";
    const std::list<TrainedEnhancement> & enhancements = treeSpend.Enhancements();

    std::string treeName = treeSpend.TreeName();
    treeName += ": ";
    // output each enhancement by buy index
    size_t buyIndex = 0;
    bool found = true;
    while (found)
    {
        // find the object with this buy index
        found = false;
        std::list<TrainedEnhancement>::const_iterator it = enhancements.begin();
        while (!found && it != enhancements.end())
        {
            found = (*it).HasBuyIndex(buyIndex);
            if (found)
            {
                break;
            }
            ++it;
        }
        if (found)
        {
            // Found it, output it
            forumExport.width(2);
            forumExport << std::right << (buyIndex + 1) << " ";
            const EnhancementTreeItem * item = FindEnhancement((*it).EnhancementName());
            if (item != NULL)
            {
                // show the tier of the enhancement
                switch (item->YPosition())
                {
                case 0: forumExport << "Core  "; break;
                case 1: forumExport << "Tier1 "; break;
                case 2: forumExport << "Tier2 "; break;
                case 3: forumExport << "Tier3 "; break;
                case 4: forumExport << "Tier4 "; break;
                case 5: forumExport << "Tier5 "; break;
                case 6: forumExport << "Tier6 "; break;
                }
                // remove "Treename: " from the output for every individual enhancement
                std::string name = item->DisplayName((*it).HasSelection() ? (*it).Selection() : "");
                name = name.substr(treeName.length());
                forumExport << "(" << item->Cost((*it).HasSelection() ? (*it).Selection() : "") << ") " << name << "\r\n";
            }
            else
            {
                forumExport << "Error - Unknown enhancement\r\n";
            }
        }
        ++buyIndex;
    }
    forumExport << "------------------------------------------------------------------------------------------\r\n";
}

void CDDOCPDoc::AddEpicDestinyTree(
        std::stringstream & forumExport,
        const EpicDestinySpendInTree & treeSpend)
{
    // TreeName: <name> - Points spent : xxx
    // <List of enhancements by display name>
    forumExport << treeSpend.TreeName() << " - Points spent: " << treeSpend.Spent() << "\r\n";
    const std::list<TrainedEnhancement> & enhancements = treeSpend.Enhancements();

    std::string treeName = treeSpend.TreeName();
    treeName += ": ";
    // output each enhancement by buy index
    size_t buyIndex = 0;
    bool found = true;
    while (found)
    {
        // find the object with this buy index
        found = false;
        std::list<TrainedEnhancement>::const_iterator it = enhancements.begin();
        while (!found && it != enhancements.end())
        {
            found = (*it).HasBuyIndex(buyIndex);
            if (found)
            {
                break;
            }
            ++it;
        }
        if (found)
        {
            // Found it, output it
            forumExport.width(2);
            forumExport << std::right << (buyIndex + 1) << " ";
            const EnhancementTreeItem * item = FindEnhancement((*it).EnhancementName());
            if (item != NULL)
            {
                // show the tier of the enhancement
                switch (item->YPosition())
                {
                case 0: forumExport << "Core  "; break;
                case 1: forumExport << "Tier1 "; break;
                case 2: forumExport << "Tier2 "; break;
                case 3: forumExport << "Tier3 "; break;
                case 4: forumExport << "Tier4 "; break;
                case 5: forumExport << "Tier5 "; break;
                case 6: forumExport << "Tier6 "; break;
                }
                // remove "Treename: " from the output for every individual enhancement
                std::string name = item->DisplayName((*it).HasSelection() ? (*it).Selection() : "");
                name = name.substr(treeName.length());
                forumExport << "(" << item->Cost((*it).HasSelection() ? (*it).Selection() : "") << ") " << name << "\r\n";
            }
            else
            {
                forumExport << "Error - Unknown enhancement\r\n";
            }
        }
        ++buyIndex;
    }
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "\r\n";
}

void CDDOCPDoc::AddTwistsOfFate(std::stringstream & forumExport)
{
    // Twists of Fate - xx of yy fate points spent
    // Twist 1 - Tier X: Selected twist
    // Twist 2 - Tier X: Selected twist
    // Twist 3 - Tier X: Selected twist
    // Twist 4 - Tier X: Selected twist
    // Twist 5 - Tier X: Selected twist
    int totalFatePoints = m_characterData.FatePoints();
    size_t spentFatePoints = m_characterData.SpentFatePoints();
    forumExport << "Twists of fate - " << spentFatePoints << " of " << totalFatePoints;
    forumExport << " Fate points spent.\r\n";
    for (size_t twist = 0; twist < MAX_TWISTS; ++twist)
    {
        if (twist == MAX_TWISTS - 1
                && !m_characterData.HasEpicCompletionist())
        {
            // don't show last twist
            break;
        }
        forumExport.width(1);
        size_t tier = m_characterData.TwistTier(twist);
        forumExport << "Twist " << (twist + 1) << " - Tier " << tier << ": ";
        const TrainedEnhancement * tt = m_characterData.TrainedTwist(twist);
        if (tt != NULL)
        {
            const EnhancementTreeItem * item = FindEnhancement(tt->EnhancementName());
            if (item != NULL)
            {
                forumExport << item->DisplayName(tt->HasSelection() ? tt->Selection() : "");
            }
            else
            {
                forumExport << "Trained twist not found";
            }
        }
        else
        {
            forumExport << "Twist slot empty";
        }
        forumExport << "\r\n";
    }
    forumExport << "\r\n";
}

void CDDOCPDoc::AddSpellPowers(std::stringstream & forumExport)
{
    // Spell Power        Base    Critical Chance    Critical Multiplier
    forumExport << "Spell Power        Base    Critical Chance    Critical Multiplier.\r\n";
    forumExport << "------------------------------------------------------------------\r\n";
    AddSpellPower(forumExport, "Acid     ", Breakdown_SpellPowerAcid, Breakdown_SpellCriticalChanceAcid, Breakdown_SpellCriticalMultiplierAcid);
    AddSpellPower(forumExport, "Alignment", Breakdown_SpellPowerAlignment, Breakdown_SpellCriticalChanceAlignment, Breakdown_SpellCriticalMultiplierAlignment);
    AddSpellPower(forumExport, "Cold     ", Breakdown_SpellPowerCold, Breakdown_SpellCriticalChanceCold, Breakdown_SpellCriticalMultiplierCold);
    AddSpellPower(forumExport, "Earth    ", Breakdown_SpellPowerEarth, Breakdown_SpellCriticalChanceEarth, Breakdown_SpellCriticalMultiplierEarth);
    AddSpellPower(forumExport, "Electric ", Breakdown_SpellPowerElectric, Breakdown_SpellCriticalChanceElectric, Breakdown_SpellCriticalMultiplierElectric);
    AddSpellPower(forumExport, "Fire     ", Breakdown_SpellPowerFire, Breakdown_SpellCriticalChanceFire, Breakdown_SpellCriticalMultiplierFire);
    AddSpellPower(forumExport, "Force    ", Breakdown_SpellPowerForce, Breakdown_SpellCriticalChanceForce, Breakdown_SpellCriticalMultiplierForce);
    AddSpellPower(forumExport, "Light    ", Breakdown_SpellPowerLight, Breakdown_SpellCriticalChanceLight, Breakdown_SpellCriticalMultiplierLight);
    AddSpellPower(forumExport, "Negative ", Breakdown_SpellPowerNegative, Breakdown_SpellCriticalChanceNegative, Breakdown_SpellCriticalMultiplierNegative);
    AddSpellPower(forumExport, "Physical ", Breakdown_SpellPowerPhysical, Breakdown_SpellCriticalChancePhysical, Breakdown_SpellCriticalMultiplierPhysical);
    AddSpellPower(forumExport, "Positive ", Breakdown_SpellPowerPositive, Breakdown_SpellCriticalChancePositive, Breakdown_SpellCriticalMultiplierPositive);
    AddSpellPower(forumExport, "Repair   ", Breakdown_SpellPowerRepair, Breakdown_SpellCriticalChanceRepair, Breakdown_SpellCriticalMultiplierRepair);
    AddSpellPower(forumExport, "Rust     ", Breakdown_SpellPowerRust, Breakdown_SpellCriticalChanceRust, Breakdown_SpellCriticalMultiplierRust);
    AddSpellPower(forumExport, "Sonic    ", Breakdown_SpellPowerSonic, Breakdown_SpellCriticalChanceSonic, Breakdown_SpellCriticalMultiplierSonic);
    AddSpellPower(forumExport, "Untyped  ", Breakdown_SpellPowerUntyped, Breakdown_SpellCriticalChanceUntyped, Breakdown_SpellCriticalMultiplierUntyped);
    AddSpellPower(forumExport, "Water    ", Breakdown_SpellPowerWater, Breakdown_SpellCriticalChanceWater, Breakdown_SpellCriticalMultiplierWater);
    forumExport << "------------------------------------------------------------------\r\n";
    forumExport << "\r\n";
}

void CDDOCPDoc::AddSpellPower(
        std::stringstream & forumExport,
        const std::string & label,
        BreakdownType btPower,
        BreakdownType btCrit,
        BreakdownType btMult)
{
    forumExport << label;
    // spell power
    forumExport << "          ";
    BreakdownItem * pBPower = FindBreakdown(btPower);
    forumExport.width(4);
    forumExport << std::right << (int)pBPower->Total();
    // spell critical chance
    forumExport << "     ";
    BreakdownItem * pBCrit = FindBreakdown(btCrit);
    forumExport.width(3);
    forumExport << std::right << (int)pBCrit->Total();
    forumExport << "%";
    // multiplier
    forumExport << "               ";
    BreakdownItem * pBMult = FindBreakdown(btMult);
    forumExport.width(3);
    forumExport << std::right << (int)pBMult->Total();
    forumExport << "\r\n";
}

void CDDOCPDoc::AddSpells(std::stringstream & forumExport)
{
    bool first = true;
    // check each possible class for selected spells
    std::vector<size_t> classLevels = m_characterData.ClassLevels(MAX_LEVEL);
    for (size_t ci = Class_Unknown + 1; ci < Class_Count; ++ci)
    {
        if (classLevels[ci] > 0)
        {
            std::vector<size_t> spellSlots = SpellSlotsForClass((ClassType)ci, classLevels[ci]);
            // must have at least 1 spell slot at any level to display the
            // required spells for this class
            size_t slotCount = std::accumulate(spellSlots.begin(), spellSlots.end(), 0);
            if (slotCount > 0)
            {
                if (first)
                {
                    forumExport << "Spells                                      School         DC     \r\n";
                    forumExport << "------------------------------------------------------------------\r\n";
                    first = false;
                }
                forumExport << EnumEntryText((ClassType)ci, classTypeMap) << " Spells\r\n";
                for (size_t spellLevel = 0; spellLevel < spellSlots.size(); ++spellLevel)
                {
                    std::list<TrainedSpell> trainedSpells = m_characterData.TrainedSpells((ClassType)ci, spellLevel + 1); // 1 based
                    // now output each spell
                    std::list<TrainedSpell>::const_iterator it = trainedSpells.begin();
                    while (it != trainedSpells.end())
                    {
                        forumExport.width(1);
                        forumExport << "L" << (spellLevel + 1) << ": ";
                        forumExport.width(40);
                        forumExport << std::left << (*it).SpellName();
                        // spell school
                        Spell spell = FindSpellByName((*it).SpellName());
                        forumExport.width(15);
                        forumExport << std::left << EnumEntryText(spell.School(), spellSchoolTypeMap);
                        // show the spell DC also
                        size_t spellDC = spell.DC(
                                m_characterData,
                                (ClassType)ci,
                                spellLevel,
                                spellSlots.size());
                        forumExport.width(3);
                        forumExport << std::right << spellDC;
                        forumExport << "\r\n";
                        ++it;
                    }
                }
            }
        }
    }
    if (!first)
    {
        forumExport << "------------------------------------------------------------------\r\n";
        forumExport << "\r\n";
    }
}

void CDDOCPDoc::AddWeaponDamage(std::stringstream & forumExport)
{
    forumExport << "Weapon Damage                                                     \r\n";
    forumExport << "------------------------------------------------------------------\r\n";
    BreakdownItem * pBI = FindBreakdown(Breakdown_MeleePower);
    forumExport << "Melee Power:  " << pBI->Total() << "\r\n";
    pBI = FindBreakdown(Breakdown_RangedPower);
    forumExport << "Ranged Power: " << pBI->Total();
    if (m_characterData.IsFeatTrained("Manyshot"))
    {
        // list ranged power when manyshot active additional BAB * 4
        forumExport << "   with Manyshot active: ";
        double total = pBI->Total();
        pBI = FindBreakdown(Breakdown_BAB);
        total += (pBI->Total() * 4);
        forumExport << total;
    }
    forumExport << "\r\n";

    EquippedGear gear = m_characterData.ActiveGearSet();
    if (gear.HasItemInSlot(Inventory_Weapon1))
    {
        BreakdownItem * pBI = FindBreakdown(Breakdown_WeaponEffectHolder);
        if (pBI != NULL)
        {
            BreakdownItemWeaponEffects * pBIWE = dynamic_cast<BreakdownItemWeaponEffects*>(pBI);
            if (pBIWE != NULL)
            {
                pBIWE->AddForumExportData(forumExport);
            }
        }
    }
}

void CDDOCPDoc::AddGear(std::stringstream & forumExport)
{
    forumExport << "Equipped Gear                                                     \r\n";
    forumExport << "------------------------------------------------------------------\r\n";
    EquippedGear gear = m_characterData.ActiveGearSet();
    for (size_t gi = Inventory_Unknown; gi < Inventory_Count; ++gi)
    {
        if (gear.HasItemInSlot((InventorySlotType)gi))
        {
            Item item = gear.ItemInSlot((InventorySlotType)gi);
            forumExport.width(10);
            forumExport << std::left << EnumEntryText((InventorySlotType)gi, InventorySlotTypeMap);
            forumExport << "    ";
            forumExport << item.Name();
            forumExport << "\r\n";
            // show effect descriptions up to the first encountered ":" character
            const std::list<std::string> & eds = item.EffectDescription();
            std::list<std::string>::const_iterator it = eds.begin();
            while (it != eds.end())
            {
                std::string processedDescription = (*it);
                processedDescription = processedDescription.substr(0, processedDescription.find(':'));
                forumExport << "              ";
                forumExport << processedDescription;
                forumExport << "\r\n";
                ++it;
            }
            // show any augment slots also
            std::vector<ItemAugment> augments = item.Augments();
            for (size_t i = 0; i < augments.size(); ++i)
            {
                forumExport << "              ";
                forumExport << augments[i].Type();
                forumExport << ": ";
                if (augments[i].HasSelectedAugment())
                {
                    // may be a configurable augment
                    if (augments[i].HasValue())
                    {
                        CString text;
                        text.Format("%+.0f ", augments[i].Value());
                        forumExport << (LPCTSTR)text;
                    }
                    forumExport << augments[i].SelectedAugment();
                }
                else
                {
                    forumExport << "Empty augment slot";
                }
                forumExport << "\r\n";
            }
            // add any sentient weapon Filigree to the list also
            if (item.HasSentientIntelligence())
            {
                forumExport << "              Sentient Weapon Personality: ";
                if (item.SentientIntelligence().HasPersonality())
                {
                    forumExport << item.SentientIntelligence().Personality();
                }
                forumExport << "\r\n";
                // now add the Filigree upgrades
                for (size_t fi = 0; fi < MAX_FILIGREE; ++fi)
                {
                    forumExport << "              Filigree " << (fi + 1) << ": ";
                    forumExport << item.SentientIntelligence().Filigree(fi);
                    if (item.SentientIntelligence().IsRareFiligree(fi))
                    {
                        forumExport << "(Rare Version)";
                    }
                    forumExport << "\r\n";
                }
            }
        }
    }
    forumExport << "------------------------------------------------------------------\r\n";
    forumExport << "\r\n";
}
