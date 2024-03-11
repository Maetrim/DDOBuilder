// ForumExportDlg.cpp
//
#include "stdafx.h"
#include "ForumExportDlg.h"
#include "BreakdownItem.h"
#include "BreakdownItemMRR.h"
#include "BreakdownItemSave.h"
#include "BreakdownItemWeaponEffects.h"
#include "MainFrm.h"
#include "DCView.h"
#include "SLAControl.h"
#include "StancesView.h"
#include <numeric>

CForumExportDlg::CForumExportDlg(Character * pCharacter) :
    CDialogEx(CForumExportDlg::IDD),
    m_pCharacter(pCharacter),
    m_bPopulatingControl(false)
{
    // read section order and display states from the registry
    for (size_t i = 0; i < FES_Count; ++i)
    {
        CString shown;
        CString section;
        shown.Format("ShowSection%d", i);
        section.Format("SectionType%d", i);
        m_bShowSection[i] = (AfxGetApp()->GetProfileInt("ForumExport", shown, 1) != 0);
        m_SectionOrder[i] = (ForumExportSections)(AfxGetApp()->GetProfileInt("ForumExport", section, i));
    }
}

CForumExportDlg::~CForumExportDlg()
{
    // save section order and display states to the registry
    for (size_t i = 0; i < FES_Count; ++i)
    {
        CString shown;
        CString section;
        shown.Format("ShowSection%d", i);
        section.Format("SectionType%d", i);
        AfxGetApp()->WriteProfileInt("ForumExport", shown, m_bShowSection[i]);
        AfxGetApp()->WriteProfileInt("ForumExport", section, m_SectionOrder[i]);
    }
}

void CForumExportDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SECTIONS, m_listConfigureExport);
    DDX_Control(pDX, IDC_EDIT_EXPORT, m_editExport);
    DDX_Control(pDX, IDC_BUTTON_PROMOTE, m_buttonMoveUp);
    DDX_Control(pDX, IDC_BUTTON_DEMOTE, m_buttonMoveDown);
}

BEGIN_MESSAGE_MAP(CForumExportDlg, CDialogEx)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SECTIONS, OnItemchangedListConfigureExport)
    ON_BN_CLICKED(IDC_BUTTON_PROMOTE, OnMoveUp)
    ON_BN_CLICKED(IDC_BUTTON_DEMOTE, OnMoveDown)
END_MESSAGE_MAP()

BOOL CForumExportDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // need non-proportional font for the edit control
    LOGFONT lf;
    ZeroMemory((PVOID)&lf, sizeof(LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0));
    lf = nm.lfMenuFont;
    strcpy_s(lf.lfFaceName, LF_FACESIZE, "Consolas");
    m_font.CreateFontIndirect(&lf);
    m_editExport.SetFont(&m_font);

    // Configure the list control with items and states
    m_listConfigureExport.InsertColumn(0, "Export Item", LVCFMT_LEFT, 150);
    m_listConfigureExport.SetExtendedStyle(
            m_listConfigureExport.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_LABELTIP);
    PopulateConfigureExportList();
    PopulateExport();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CForumExportDlg::PopulateConfigureExportList()
{
    m_bPopulatingControl = true;
    m_listConfigureExport.LockWindowUpdate();
    // get current selection
    int sel = m_listConfigureExport.GetSelectionMark();
    m_listConfigureExport.DeleteAllItems();
    for (size_t i = 0; i < FES_Count; ++i)
    {
        CString item;
        item = EnumEntryText(m_SectionOrder[i], forumExportSectionsMap);
        m_listConfigureExport.InsertItem(i, item, 0);
        m_listConfigureExport.SetItemData(i, m_SectionOrder[i]);
        m_listConfigureExport.SetItemState(
                i,
                INDEXTOSTATEIMAGEMASK((int)m_bShowSection[i] + 1),
                LVIS_STATEIMAGEMASK);
    }
    // restore selection (if any)
    if (sel >= 0)
    {
        m_listConfigureExport.SetItemState(
                sel,
                LVIS_SELECTED | LVIS_FOCUSED,
                LVIS_SELECTED | LVIS_FOCUSED);
        m_listConfigureExport.SetSelectionMark(sel);
        // enable the Move Up/Down buttons appropriately
        m_buttonMoveUp.EnableWindow(sel > 0);
        m_buttonMoveDown.EnableWindow(sel < FES_Count - 1);
    }
    else
    {
        m_buttonMoveUp.EnableWindow(FALSE);
        m_buttonMoveDown.EnableWindow(FALSE);
    }
    m_listConfigureExport.UnlockWindowUpdate();
    m_bPopulatingControl = false;
}

void CForumExportDlg::OnItemchangedListConfigureExport(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (!m_bPopulatingControl)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        BOOL oldCheckState = ((pNMListView->uOldState & LVIS_STATEIMAGEMASK)>>12) - 1;
        BOOL newCheckState = ((pNMListView->uNewState & LVIS_STATEIMAGEMASK)>>12) - 1;

        if (oldCheckState != -1
                && newCheckState != -1
                && oldCheckState != newCheckState)
        {
            m_bShowSection[pNMListView->iItem] = !m_bShowSection[pNMListView->iItem];
            PopulateExport();
        }

        if ((pNMListView->uChanged & LVIF_STATE) 
                && (pNMListView->uNewState & LVIS_SELECTED))
        {
            // item selection has changed, select it
            int sel = pNMListView->iItem;
            // enable the Move Up/Down buttons appropriately
            m_buttonMoveUp.EnableWindow(sel > 0);
            m_buttonMoveDown.EnableWindow(sel < FES_Count - 1);
        }
    }
    *pResult = 0;
}

void CForumExportDlg::PopulateExport()
{
    std::stringstream forumExport;
    forumExport << "[font=courier]\r\n";
    for (size_t i = 0 ; i < FES_Count; ++i)
    {
        if (m_bShowSection[i])
        {
            switch (m_SectionOrder[i])
            {
            case FES_Header:
                AddCharacterHeader(forumExport);
                break;
            case FES_PastLives:
                AddPastLives(forumExport);
                break;
            case FES_SpecialFeats:
                AddSpecialFeats(forumExport);
                break;
            case FES_Saves:
                AddSaves(forumExport);
                break;
            case FES_EnergyResistances:
                AddEnergyResistances(forumExport);
                break;
            case FES_FeatSelections:
            case FES_FeatSelectionsNoSkills:
                AddFeatSelections(forumExport, m_SectionOrder[i] == FES_FeatSelections);
                break;
            case FES_GrantedFeats:
                AddGrantedFeats(forumExport);
                break;
            case FES_AutomaticFeats:
                AddAutomaticFeats(forumExport);
                break;
            case FES_ConsolidatedFeats:
                AddConsolidatedFeats(forumExport);
                break;
            case FES_Skills:
                AddSkills(forumExport);
                break;
            case FES_ActiveStances:
                AddActiveStances(forumExport);
                break;
            case FES_SelfAndPartyBuffs:
                AddSelfAndPartyBuffs(forumExport);
                break;
            case FES_EnhancementTrees:
                AddEnhancements(forumExport);
                break;
            case FES_DestinyTrees:
                AddEpicDestinyTree(forumExport);
                break;
            case FES_ReaperTrees:
                AddReaperTrees(forumExport);
                break;
            case FES_SpellPowers:
                AddSpellPowers(forumExport);
                break;
            case FES_Spells:
                AddSpells(forumExport);
                break;
            case FES_SLAs:
                AddSLAs(forumExport);
                break;
            case FES_WeaponDamage:
                AddWeaponDamage(forumExport);
                break;
            case FES_TacticalDCs:
                AddTacticalDCs(forumExport);
                break;
            case FES_Gear:
                AddGear(forumExport);
                break;
            case FES_AlternateGearLayouts:
                AddAlternateGear(forumExport);
                break;
            case FES_SimpleGear:
                AddSimpleGear(forumExport);
                break;
            }
        }
    }
    forumExport << "[/font]\r\n";
    // to format correctly to the forums multiple space " " characters cannot
    // be present else they get reduced to a single space losing all layout the formatting
    // to avoid this we do a a replace of all "  " to " ." in the generated
    // string on all odd character positions from the start of each line
    std::string generatedData = forumExport.str();
    FormatExportData(&generatedData);
    // need to retain the control scroll position
    int pos = m_editExport.GetFirstVisibleLine();
    m_editExport.SetWindowText(generatedData.c_str());
    m_editExport.LineScroll(pos, 0);
}

void CForumExportDlg::OnOK()
{
    // copy to clipboard and close dialog
    CString clipboardText;
    m_editExport.GetWindowText(clipboardText);
    // now place the text on the clipboard
    if (::OpenClipboard(NULL))
    {
        HGLOBAL clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, clipboardText.GetLength()+1);
        ASSERT(clipbuffer != NULL);
        char *buffer = (char*)::GlobalLock(clipbuffer);
        strcpy_s(buffer, clipboardText.GetLength()+1, clipboardText);
        ::GlobalUnlock(clipbuffer);
        ::EmptyClipboard();
        ::SetClipboardData(CF_TEXT, clipbuffer);
        ::CloseClipboard();
    }
    CDialogEx::OnOK();
}

void CForumExportDlg::AddCharacterHeader(std::stringstream & forumExport)
{
    // Character Name: [.................................]
    // Classes: [x][class1], [y][class2], [z][class3] and [10]Epic
    // Race: [...............] Alignment: [..............]
    //
    //      Start Tome Final     Incorp: xx%         Displacement: xx%
    // Str: [...] [..] [...]     HP:    [....]       AC: [...]
    // Dex: [...] [..] [...]     PRR:   [....]       DR: [........................]
    // Con: [...] [..] [...]     MRR:   [....]/[MAX] +Healing Amp: [...]
    // Int: [...] [..] [...]     Dodge: [...]%/[MAX] -Healing Amp: [...]
    // Wis: [...] [..] [...]     Fort:  [...]%       Repair Amp:   [...]
    // Cha: [...] [..] [...]     SR:    [...]        BAB:   [..]
    // DR:  List of DR items
    // immunities:  List of immunities

     // first line is the character name
    forumExport << "Character name: " << m_pCharacter->Name() << "\r\n";

    // Classes line
    forumExport << "Classes: " << m_pCharacter->GetBuildDescription() << "\r\n";

    // Race/Alignment line
    std::string race = EnumEntryText(m_pCharacter->Race(), raceTypeMap);
    forumExport << "Race: ";
    forumExport.fill(' ');
    forumExport.width(21);
    forumExport << std::left << race;
    std::string alignment = EnumEntryText(m_pCharacter->OverrideAlignment(), alignmentTypeMap);
    forumExport << "Alignment: ";
    forumExport << std::left << alignment << "\r\n";
    // blank line
    forumExport << "\r\n";
    forumExport << "     Start Tome Final";
    AddBreakdown(forumExport, "      Incorp: ", 5, Breakdown_Incorpreality);
    AddBreakdown(forumExport, "%      Displacement: ", 4, Breakdown_Displacement);
    forumExport << "%\r\n";

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
    AddBreakdown(forumExport, "      +Healing Amp: ", 5, Breakdown_HealingAmplification);
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Intelligence);
    AddBreakdown(forumExport, "      Dodge: ", 4, Breakdown_Dodge);
    if (m_pCharacter->IsStanceActive("Tower Shield"))
    {
        AddBreakdown(forumExport, "/", 1, Breakdown_DodgeCapTowerShield);
    }
    else
    {
        AddBreakdown(forumExport, "/", 1, Breakdown_DodgeCap);
    }
    AddBreakdown(forumExport, "      -Healing Amp: ", 5, Breakdown_NegativeHealingAmplification);
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Wisdom);
    AddBreakdown(forumExport, "      Fort: ", 7, Breakdown_Fortification);
    AddBreakdown(forumExport, "%      Repair Amp:   ", 5, Breakdown_RepairAmplification);
    forumExport << "\r\n";

    AddAbilityValues(forumExport, Ability_Charisma);
    AddBreakdown(forumExport, "      SR: ", 10, Breakdown_SpellResistance);
    AddBreakdown(forumExport, "      BAB: ", 14, Breakdown_BAB);
    forumExport << "\r\n";
    BreakdownItem * pDR = FindBreakdown(Breakdown_DR);
    forumExport << "DR: " << pDR->Value();
    forumExport << "\r\n";
    BreakdownItem * pImmunities = FindBreakdown(Breakdown_Immunities);
    forumExport << "Immunities: " << pImmunities->Value();
    forumExport << "\r\n\r\n";
}

void CForumExportDlg::AddPastLives(std::stringstream & forumExport)
{
    // add the special feats (past lives and other feats such as inherent)
    std::list<TrainedFeat> feats = m_pCharacter->SpecialFeats().Feats();
    if (feats.size() > 0)
    {
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
        forumExport << "\r\n";
    }
}

void CForumExportDlg::AddSpecialFeats(std::stringstream & forumExport)
{
    // add the special feats (past lives and other feats such as inherent)
    std::list<TrainedFeat> feats = m_pCharacter->SpecialFeats().Feats();
    if (feats.size() > 0)
    {
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
        AddFeats(forumExport, "Special Feats", TFT_SpecialFeat, feats);
        forumExport << "\r\n";
    }
}

void CForumExportDlg::AddFeats(
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

void CForumExportDlg::AddSaves(std::stringstream & forumExport)
{
    forumExport << "Saves:\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
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

void CForumExportDlg::AddAbilityValues(std::stringstream & forumExport, AbilityType ability)
{
    size_t baseValue = m_pCharacter->BaseAbilityValue(ability);
    BreakdownItem * pBI = FindBreakdown(StatToBreakdown(ability));
    size_t buffedValue = (size_t)pBI->Total();      // whole numbers only
    size_t tomeValue = m_pCharacter->TomeAtLevel(ability, m_pCharacter->MaxLevel()-1);
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

void CForumExportDlg::AddBreakdown(
        std::stringstream & forumExport,
        const std::string & header,
        size_t width,
        BreakdownType bt)
{
    BreakdownItem * pBI = FindBreakdown(bt);
    size_t value = (size_t)pBI->CappedTotal();      // whole numbers only
    if (bt == Breakdown_MRR)
    {
        BreakdownItem * pBIMRRCap = FindBreakdown(Breakdown_MRRCap);
        if (pBIMRRCap->Total() > 0
                && pBIMRRCap->Total() < pBI->Total())
        {
            // show that the MRR value is capped
            CString text;
            text.Format("%d/%d", value, (size_t)pBIMRRCap->Total());
            forumExport << header;
            forumExport.width(width);
            forumExport << std::right << (LPCTSTR)text;
        }
        else
        {
            // just show the MRR value
            forumExport << header;
            forumExport.width(width);
            forumExport << std::right << value;
        }
    }
    else
    {
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
    }
}

void CForumExportDlg::AddFeatSelections(std::stringstream & forumExport, bool bIncludeSkills)
{
    forumExport << "Class and Feat Selection\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "Level Class            Feats\r\n";
    for (size_t level = 0; level < m_pCharacter->MaxLevel(); ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
        if (level == 0)
        {
            bool requiresHeartOfWood = false;
            ClassType expectedClass = levelData.HasClass() ? levelData.Class() : Class_Unknown;
            switch (m_pCharacter->Race())
            {
            case Race_AasimarScourge: requiresHeartOfWood = (expectedClass != Class_Ranger); break;
            case Race_BladeForged: requiresHeartOfWood = (expectedClass != Class_Paladin); break;
            case Race_DeepGnome: requiresHeartOfWood = (expectedClass != Class_Wizard); break;
            case Race_KoboldShamen: requiresHeartOfWood = (expectedClass != Class_Sorcerer); break;
            case Race_Morninglord: requiresHeartOfWood = (expectedClass != Class_Cleric); break;
            case Race_PurpleDragonKnight: requiresHeartOfWood = (expectedClass != Class_Fighter); break;
            case Race_RazorclawShifter: requiresHeartOfWood = (expectedClass != Class_Barbarian); break;
            case Race_ShadarKai: requiresHeartOfWood = (expectedClass != Class_Rogue); break;
            case Race_TabaxiTrailblazer: requiresHeartOfWood = (expectedClass != Class_Monk); break;
            case Race_TieflingScoundrel: requiresHeartOfWood = (expectedClass != Class_Bard); break;
            }
            if (requiresHeartOfWood)
            {
                forumExport << "                       Level 1 Requires a +1 Heart of Wood to switch out of Iconic Class\r\n";
            }
        }
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(level);
        CString className;
        className.Format("%s(%d)",
                EnumEntryText(levelData.HasClass() ? levelData.Class() : Class_Unknown, classTypeMap),
                levelData.HasClass() ? classLevels[levelData.Class()] : 0);
        forumExport.width(2);
        forumExport << std::left << (level + 1) << "    ";
        forumExport.fill(' ');
        forumExport.width(17);
        forumExport << std::left << className;
        // now add the trainable feat types and their selections
        std::vector<TrainableFeatTypes> trainable = m_pCharacter->TrainableFeatTypeAtLevel(level);
        if (trainable.size() > 0)
        {
            for (size_t tft = 0; tft < trainable.size(); ++tft)
            {
                CString label = TrainableFeatTypeLabel(trainable[tft]);
                label += ": ";
                TrainedFeat tf = m_pCharacter->GetTrainedFeat(
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
                    if (tf.HasAlternateFeatName())
                    {
                        label += " Alternate: ";
                        label += tf.AlternateFeatName().c_str();
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
        AbilityType ability = m_pCharacter->AbilityLevelUp(level + 1);
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
        if (bIncludeSkills)
        {
            AddSkillsAtLevel(level, forumExport);
        }
    }
    forumExport << "\r\n";
}

void CForumExportDlg::AddGrantedFeats(std::stringstream & forumExport)
{
    if (m_pCharacter->HasGrantedFeats())
    {
        forumExport << "Granted Feats\r\n";
        forumExport << "------------------------------------------------------------------------------------------\r\n";
        const std::list<TrainedFeat> & grantedFeats = m_pCharacter->GrantedFeats();
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

void CForumExportDlg::AddAutomaticFeats(std::stringstream & forumExport)
{
    // list all the automatic feats gained at each level
    forumExport << "Automatic Feats\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "Level Class            Feats\r\n";
    for (size_t level = 0; level < m_pCharacter->MaxLevel(); ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(level);
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

void CForumExportDlg::AddConsolidatedFeats(std::stringstream & forumExport)
{
    forumExport << "Class and Feat Selection (Consolidated)\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "Level Class            Feats\r\n";
    for (size_t level = 0; level < m_pCharacter->MaxLevel(); ++level)
    {
        bool first = true;
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
        if (level == 0)
        {
            bool requiresHeartOfWood = false;
            ClassType expectedClass = levelData.HasClass() ? levelData.Class() : Class_Unknown;
            switch (m_pCharacter->Race())
            {
            case Race_AasimarScourge: requiresHeartOfWood = (expectedClass != Class_Ranger); break;
            case Race_BladeForged: requiresHeartOfWood = (expectedClass != Class_Paladin); break;
            case Race_DeepGnome: requiresHeartOfWood = (expectedClass != Class_Wizard); break;
            case Race_KoboldShamen: requiresHeartOfWood = (expectedClass != Class_Sorcerer); break;
            case Race_Morninglord: requiresHeartOfWood = (expectedClass != Class_Cleric); break;
            case Race_PurpleDragonKnight: requiresHeartOfWood = (expectedClass != Class_Fighter); break;
            case Race_RazorclawShifter: requiresHeartOfWood = (expectedClass != Class_Barbarian); break;
            case Race_ShadarKai: requiresHeartOfWood = (expectedClass != Class_Rogue); break;
            case Race_TabaxiTrailblazer: requiresHeartOfWood = (expectedClass != Class_Monk); break;
            case Race_TieflingScoundrel: requiresHeartOfWood = (expectedClass != Class_Bard); break;
            }
            if (requiresHeartOfWood)
            {
                forumExport << "                       Level 1 Requires a +1 Heart of Wood to switch out of Iconic Class\r\n";
                first = false;
            }
        }
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(level);
        CString className;
        className.Format("%s(%d)",
                EnumEntryText(levelData.HasClass() ? levelData.Class() : Class_Unknown, classTypeMap),
                levelData.HasClass() ? classLevels[levelData.Class()] : 0);
        forumExport.width(2);
        forumExport << std::left << (level + 1) << "    ";
        forumExport.fill(' ');
        forumExport.width(17);
        forumExport << std::left << className;
        // now add the trainable feat types and their selections
        std::vector<TrainableFeatTypes> trainable = m_pCharacter->TrainableFeatTypeAtLevel(level);
        if (trainable.size() > 0)
        {
            for (size_t tft = 0; tft < trainable.size(); ++tft)
            {
                CString label = TrainableFeatTypeLabel(trainable[tft]);
                label += ": ";
                TrainedFeat tf = m_pCharacter->GetTrainedFeat(
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
            first = false;
        }
        // also need to show ability adjustment on every 4th level
        AbilityType ability = m_pCharacter->AbilityLevelUp(level + 1);
        if (ability != Ability_Unknown)
        {
            if (trainable.size() > 0)
            {
                forumExport << "                       ";
            }
            forumExport << EnumEntryText(ability, abilityTypeMap);
            forumExport << ": +1 Level up\r\n";
            first = false;
        }
        // now add the automatic feats
        const FeatsListObject & automaticFeats = levelData.AutomaticFeats();
        const std::list<TrainedFeat> & feats = automaticFeats.Feats();
        std::list<TrainedFeat>::const_iterator it = feats.begin();
        while (it != feats.end())
        {
            if (!first)
            {
                forumExport << "                       ";
            }
            forumExport << "Automatic: " << (*it).FeatName();
            forumExport << "\r\n";
            ++it;
            first = false;
        }
        forumExport << "\r\n";
    }
    forumExport << "\r\n";
}

void CForumExportDlg::AddActiveStances(std::stringstream & forumExport)
{
    CWnd * pWnd = AfxGetMainWnd();
    CMainFrame * pMainWnd = dynamic_cast<CMainFrame*>(pWnd);
    const CStancesView * pStancesView = pMainWnd->GetStancesView();
    if (pStancesView != NULL)
    {
        const std::vector<CStanceButton *> & userStances =
                pStancesView->ActiveUserStances();
        const std::vector<CStanceButton *> & autoStances =
                pStancesView->ActiveAutoStances();
        bool first = true;
        for (size_t i = 0; i < userStances.size(); ++i)
        {
            if (first)
            {
                forumExport << "Active User Controlled Stances\r\n";
                forumExport << "------------------------------------------------------------------------------------------\r\n";
            }
            // this is an active stance
            const Stance & stance = userStances[i]->GetStance();
            forumExport << stance.Name();
            forumExport << "\r\n";
            first = false;
        }
        first = true;
        for (size_t i = 0; i < autoStances.size(); ++i)
        {
            if (first)
            {
                forumExport << "\r\n";
                forumExport << "Active Auto Controlled Stances\r\n";
                forumExport << "------------------------------------------------------------------------------------------\r\n";
            }
            // this is an active stance
            const Stance & stance = autoStances[i]->GetStance();
            forumExport << stance.Name();
            forumExport << "\r\n";
            first = false;
        }
        if (!first)
        {
            forumExport << "------------------------------------------------------------------------------------------\r\n";
            forumExport << "\r\n";
        }
    }
}

void CForumExportDlg::AddSelfAndPartyBuffs(std::stringstream & forumExport)
{
    forumExport << "Self and Party Buffs\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    const std::list<std::string> & buffs = m_pCharacter->SelfAndPartyBuffs();
    std::list<std::string>::const_iterator it = buffs.begin();
    while (it != buffs.end())
    {
        forumExport << (*it) << "\r\n";
        ++it;
    }
    forumExport << "\r\n";
}

void CForumExportDlg::AddSkills(std::stringstream & forumExport)
{
    // Example Output:
    // Skills
    // ------------------------------------------------------------------------------------------
    // Skill Points     18  9  9  9 10 10 10 10 10 10 11 11 11 11 11 12 12 12 12 12
    // Skill Name       01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 Total Buffed
    // -----------------------------------------------------------------------------------------
    // Disable Device    4  1        3                                              xxx   xxx
    // Use Magic Device                                                             xxx   xxx
    // -----------------------------------------------------------------------------------------
    // Available Points 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    forumExport << "Skills\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "Skill Points    ";
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
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
        for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
        {
            const LevelTraining & levelData = m_pCharacter->LevelData(level);
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
                ClassType ct = m_pCharacter->LevelData(level).HasClass()
                        ? m_pCharacter->LevelData(level).Class()
                        : Class_Unknown;
                if (IsClassSkill(ct, (SkillType)skill))
                {
                    text.Format("%d", skillRanks[skill]);
                }
                else
                {
                    // its a cross class skill, show in multiples of �
                    text = "";
                    int fullRanks = (skillRanks[skill] / 2);
                    if (fullRanks > 0)
                    {
                        text.Format("%d", fullRanks);
                    }
                    if (skillRanks[skill] % 2 != 0)
                    {
                        text += "�";
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
        double total = m_pCharacter->SkillAtLevel((SkillType)skill, MAX_CLASS_LEVELS, false);
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
    for (size_t level = 0; level < MAX_CLASS_LEVELS; ++level)
    {
        const LevelTraining & levelData = m_pCharacter->LevelData(level);
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

void CForumExportDlg::AddSkillsAtLevel(size_t level, std::stringstream & forumExport)
{
    // get the ranks spent in each skill for this level
    const LevelTraining & levelData = m_pCharacter->LevelData(level);
    const std::list<TrainedSkill> & ts = levelData.TrainedSkills();
    std::list<TrainedSkill>::const_iterator it = ts.begin();
    std::vector<size_t> skillRanks(Skill_Count, 0);
    while (it != ts.end())
    {
        skillRanks[(*it).Skill()]++;
        ++it;
    }
    // add the class skills
    size_t numAdded = 0;
    for (size_t skill = Skill_Unknown + 1; skill < Skill_Count; ++skill)
    {
        if (skillRanks[skill] > 0
                && IsClassSkill(levelData.Class(), (SkillType)skill))
        {
            if (numAdded > 0)
            {
                forumExport << ", ";
            }
            else
            {
                forumExport << "                       Class Skills: ";
            }
            forumExport << EnumEntryText((SkillType)skill, skillTypeMap)
                    << "(" << skillRanks[skill] << ")";
            ++numAdded;
        }
    }
    if (numAdded > 0)
    {
        forumExport << "\r\n";
    }
    // add the cross class skills
    numAdded = 0;
    for (size_t skill = Skill_Unknown + 1; skill < Skill_Count; ++skill)
    {
        if (skillRanks[skill] > 0
                && !IsClassSkill(levelData.Class(), (SkillType)skill))
        {
            if (numAdded > 0)
            {
                forumExport << ", ";
            }
            else
            {
                forumExport << "                       Cross Class Skills: ";
            }
            forumExport << EnumEntryText((SkillType)skill, skillTypeMap)
                    << "(" << skillRanks[skill] << ")";
            ++numAdded;
        }
    }
    if (numAdded > 0)
    {
        forumExport << "\r\n";
    }
}

void CForumExportDlg::AddEnergyResistances(std::stringstream & forumExport)
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
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    AddEnergyResistances(forumExport, "Acid:", Breakdown_EnergyResistanceAcid, Breakdown_EnergyAbsorptionAcid);
    AddEnergyResistances(forumExport, "Chaos:", Breakdown_EnergyResistanceChaos, Breakdown_EnergyAbsorptionChaos);
    AddEnergyResistances(forumExport, "Cold:", Breakdown_EnergyResistanceCold, Breakdown_EnergyAbsorptionCold);
    AddEnergyResistances(forumExport, "Electric:", Breakdown_EnergyResistanceElectric, Breakdown_EnergyAbsorptionElectric);
    AddEnergyResistances(forumExport, "Evil:", Breakdown_EnergyResistanceEvil, Breakdown_EnergyAbsorptionEvil);
    AddEnergyResistances(forumExport, "Fire:", Breakdown_EnergyResistanceFire, Breakdown_EnergyAbsorptionFire);
    AddEnergyResistances(forumExport, "Force:", Breakdown_EnergyResistanceForce, Breakdown_EnergyAbsorptionForce);
    AddEnergyResistances(forumExport, "Good:", Breakdown_EnergyResistanceGood, Breakdown_EnergyAbsorptionGood);
    AddEnergyResistances(forumExport, "Lawful:", Breakdown_EnergyResistanceLawful, Breakdown_EnergyAbsorptionLawful);
    AddEnergyResistances(forumExport, "Light:", Breakdown_EnergyResistanceLight, Breakdown_EnergyAbsorptionLight);
    AddEnergyResistances(forumExport, "Negative:", Breakdown_EnergyResistanceNegative, Breakdown_EnergyAbsorptionNegative);
    AddEnergyResistances(forumExport, "Poison:", Breakdown_EnergyResistancePoison, Breakdown_EnergyAbsorptionPoison);
    //AddEnergyResistances(forumExport, "Positive:", Breakdown_EnergyResistancePositive, Breakdown_EnergyAbsorptionPositive);
    //AddEnergyResistances(forumExport, "Repair:", Breakdown_EnergyResistanceRepair, Breakdown_EnergyAbsorptionRepair);
    //AddEnergyResistances(forumExport, "Rust:", Breakdown_EnergyResistanceRust, Breakdown_EnergyAbsorptionRust);
    AddEnergyResistances(forumExport, "Sonic:", Breakdown_EnergyResistanceSonic, Breakdown_EnergyAbsorptionSonic);
    forumExport << "\r\n";
}

void CForumExportDlg::AddEnergyResistances(
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

void CForumExportDlg::AddEnhancements(std::stringstream & forumExport)
{
    forumExport << "Enhancements: 80 APs";
    if (m_pCharacter->BonusRacialActionPoints() > 0)
    {
        forumExport << ", Racial ";
        forumExport.width(1);
        forumExport << m_pCharacter->BonusRacialActionPoints();
    }
    if (m_pCharacter->BonusUniversalActionPoints() > 0)
    {
        forumExport << ", Universal ";
        forumExport.width(1);
        forumExport << m_pCharacter->BonusUniversalActionPoints();
    }
    forumExport << "\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    const SelectedEnhancementTrees & trees = m_pCharacter->SelectedTrees();
    for (size_t ti = 0; ti < MAX_ENHANCEMENT_TREES; ++ti)
    {
        const std::string & treeName = trees.Tree(ti);
        if (!SelectedEnhancementTrees::IsNotSelected(treeName))
        {
            // this is a tree we have to list
            EnhancementSpendInTree * treeSpend = m_pCharacter->Enhancement_FindTree(treeName);
            if (treeSpend != NULL
                    && treeSpend->Spent() > 0)
            {
                // tree can be selected and have no enhancements trained, thus not be present
                AddEnhancementTree(forumExport, *treeSpend);
            }
        }
    }
}

void CForumExportDlg::AddEpicDestinyTree(
        std::stringstream & forumExport)
{
    int permanentDestinyPoints = 0;
    BreakdownItem * pBD = FindBreakdown(Breakdown_DestinyPoints);
    if (pBD != NULL)
    {
        permanentDestinyPoints = static_cast<int>(pBD->Total());
    }
    forumExport << "Epic Destinies: 40 APs + "
            << permanentDestinyPoints << " permanent Destiny Points";
    forumExport << "\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    const SelectedDestinyTrees& trees = m_pCharacter->DestinyTrees();
    for (size_t ti = 0; ti < MAX_EPIC_DESTINY_TREES; ++ti)
    {
        const std::string & treeName = trees.Tree(ti);
        if (!SelectedEnhancementTrees::IsNotSelected(treeName))
        {
            // this is a tree we have to list
            DestinySpendInTree * treeSpend = m_pCharacter->U51Destiny_FindTree(treeName);
            if (treeSpend != NULL
                    && treeSpend->Spent() > 0)
            {
                // tree can be selected and have no enhancements trained, thus not be present
                AddEpicDestinyTree(forumExport, *treeSpend);
            }
        }
    }
}

void CForumExportDlg::AddReaperTrees(std::stringstream & forumExport)
{
    // add each of the 3 Reaper Trees
    ReaperSpendInTree * treeSpend = m_pCharacter->Reaper_FindTree("Dread Adversary");
    if (treeSpend != NULL)
    {
        // tree can be selected and have no enhancements trained, thus not be present
        AddReaperTree(forumExport, *treeSpend);
    }
    treeSpend = m_pCharacter->Reaper_FindTree("Dire Thaumaturge");
    if (treeSpend != NULL)
    {
        // tree can be selected and have no enhancements trained, thus not be present
        AddReaperTree(forumExport, *treeSpend);
    }
    treeSpend = m_pCharacter->Reaper_FindTree("Grim Barricade");
    if (treeSpend != NULL)
    {
        // tree can be selected and have no enhancements trained, thus not be present
        AddReaperTree(forumExport, *treeSpend);
    }
}

void CForumExportDlg::AddEnhancementTree(
        std::stringstream & forumExport,
        const EnhancementSpendInTree & treeSpend)
{
    // TreeName: <name> - Points spent : xxx
    // <List of enhancements by display name>
    forumExport << treeSpend.TreeName() << " - Points spent: " << treeSpend.Spent() << "\r\n";
    const std::list<TrainedEnhancement> & enhancements = treeSpend.Enhancements();

    // output each enhancement by buy index
    std::list<TrainedEnhancement>::const_iterator it = enhancements.begin();
    while (it != enhancements.end())
    {
        const EnhancementTreeItem * item = FindEnhancement((*it).EnhancementName());
        if (item != NULL)
        {
            // show the tier of the enhancement
            switch (item->YPosition())
            {
            case 0: forumExport << "Core" << item->XPosition()+1 << " "; break;
            case 1: forumExport << "Tier1 "; break;
            case 2: forumExport << "Tier2 "; break;
            case 3: forumExport << "Tier3 "; break;
            case 4: forumExport << "Tier4 "; break;
            case 5: forumExport << "Tier5 "; break;
            case 6: forumExport << "Tier6 "; break;
            }
            // remove "Treename: " from the output for every individual enhancement
            std::string name = item->DisplayName((*it).HasSelection() ? (*it).Selection() : "");
            if (name.find(':') != std::string::npos)
            {
                name = name.substr(name.find(':') + 2);
            }
            forumExport << name;
            if (item->Ranks(name) > 1)
            {
                forumExport << " - " << (*it).Ranks() << " Ranks";
            }
            forumExport << "\r\n";
        }
        else
        {
            forumExport << "Error - Unknown enhancement\r\n";
        }
        ++it;
    }
    forumExport << "------------------------------------------------------------------------------------------\r\n";
}

void CForumExportDlg::AddEpicDestinyTree(
        std::stringstream & forumExport,
        const DestinySpendInTree & treeSpend)
{
    // TreeName: <name> - Points spent : xxx
    // <List of enhancements by display name>
    forumExport << treeSpend.TreeName() << " - Points spent: " << treeSpend.Spent() << "\r\n";
    const std::list<TrainedEnhancement> & enhancements = treeSpend.Enhancements();

    std::list<TrainedEnhancement>::const_iterator it = enhancements.begin();
    while (it != enhancements.end())
    {
        const EnhancementTreeItem * item = FindEnhancement((*it).EnhancementName());
        if (item != NULL)
        {
            // show the tier of the enhancement
            switch (item->YPosition())
            {
            case 0: forumExport << "Core" << item->XPosition()+1 << " "; break;
            case 1: forumExport << "Tier1 "; break;
            case 2: forumExport << "Tier2 "; break;
            case 3: forumExport << "Tier3 "; break;
            case 4: forumExport << "Tier4 "; break;
            case 5: forumExport << "Tier5 "; break;
            case 6: forumExport << "Tier6 "; break;
            }
            // remove "Treename: " from the output for every individual enhancement
            std::string name = item->DisplayName((*it).HasSelection() ? (*it).Selection() : "");
            if (name.find(':') != std::string::npos)
            {
                name = name.substr(name.find(':') + 2);
            }
            forumExport << name;
            if (item->Ranks(name) > 1)
            {
                forumExport << " - " << (*it).Ranks() << " Ranks";
            }
            forumExport << "\r\n";
        }
        else
        {
            forumExport << "Error - Unknown enhancement\r\n";
        }
        ++it;
    }
    forumExport << "------------------------------------------------------------------------------------------\r\n";
}

void CForumExportDlg::AddReaperTree(
        std::stringstream & forumExport,
        const ReaperSpendInTree & treeSpend)
{
    // TreeName: <name> - Points spent : xxx
    // <List of enhancements by display name>
    forumExport << treeSpend.TreeName() << " - Points spent: " << treeSpend.Spent() << "\r\n";
    const std::list<TrainedEnhancement> & enhancements = treeSpend.Enhancements();

    std::list<TrainedEnhancement>::const_iterator it = enhancements.begin();
    while (it != enhancements.end())
    {
        const EnhancementTreeItem * item = FindEnhancement((*it).EnhancementName());
        if (item != NULL)
        {
            // show the tier of the enhancement
            switch (item->YPosition())
            {
            case 0: forumExport << "Core" << item->XPosition()+1 << " "; break;
            case 1: forumExport << "Tier1 "; break;
            case 2: forumExport << "Tier2 "; break;
            case 3: forumExport << "Tier3 "; break;
            case 4: forumExport << "Tier4 "; break;
            case 5: forumExport << "Tier5 "; break;
            case 6: forumExport << "Tier6 "; break;
            }
            // remove "Treename: " from the output for every individual enhancement
            std::string name = item->DisplayName((*it).HasSelection() ? (*it).Selection() : "");
            forumExport << name;
            if (item->Ranks(name) > 1)
            {
                forumExport << " - " << item->Ranks(name) << " Ranks";
            }
            forumExport << "\r\n";
        }
        else
        {
            forumExport << "Error - Unknown enhancement\r\n";
        }
        ++it;
    }
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "\r\n";
}

void CForumExportDlg::AddSpellPowers(std::stringstream & forumExport)
{
    // Spell Power              Base    Critical Chance    Critical Multiplier
    forumExport << "Spell Power              Base    Critical Chance    Critical Multiplier.\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    AddSpellPower(forumExport, "Acid           ", Breakdown_SpellPowerAcid, Breakdown_SpellCriticalChanceAcid, Breakdown_SpellCriticalMultiplierAcid);
    AddSpellPower(forumExport, "Light/Alignment", Breakdown_SpellPowerLightAlignment, Breakdown_SpellCriticalChanceLightAlignment, Breakdown_SpellCriticalMultiplierLightAlignment);
    AddSpellPower(forumExport, "Cold           ", Breakdown_SpellPowerCold, Breakdown_SpellCriticalChanceCold, Breakdown_SpellCriticalMultiplierCold);
    AddSpellPower(forumExport, "Electric       ", Breakdown_SpellPowerElectric, Breakdown_SpellCriticalChanceElectric, Breakdown_SpellCriticalMultiplierElectric);
    AddSpellPower(forumExport, "Evil           ", Breakdown_SpellPowerEvil, Breakdown_SpellCriticalChanceEvil, Breakdown_SpellCriticalMultiplierEvil);
    AddSpellPower(forumExport, "Fire           ", Breakdown_SpellPowerFire, Breakdown_SpellCriticalChanceFire, Breakdown_SpellCriticalMultiplierFire);
    AddSpellPower(forumExport, "Force/Untyped  ", Breakdown_SpellPowerForceUntyped, Breakdown_SpellCriticalChanceForceUntyped, Breakdown_SpellCriticalMultiplierForceUntyped);
    AddSpellPower(forumExport, "Negative       ", Breakdown_SpellPowerNegative, Breakdown_SpellCriticalChanceNegative, Breakdown_SpellCriticalMultiplierNegative);
    AddSpellPower(forumExport, "Physical       ", Breakdown_SpellPowerPhysical, Breakdown_SpellCriticalChancePhysical, Breakdown_SpellCriticalMultiplierPhysical);
    AddSpellPower(forumExport, "Poison         ", Breakdown_SpellPowerPoison, Breakdown_SpellCriticalChancePoison, Breakdown_SpellCriticalMultiplierPoison);
    AddSpellPower(forumExport, "Positive       ", Breakdown_SpellPowerPositive, Breakdown_SpellCriticalChancePositive, Breakdown_SpellCriticalMultiplierPositive);
    AddSpellPower(forumExport, "Repair         ", Breakdown_SpellPowerRepair, Breakdown_SpellCriticalChanceRepair, Breakdown_SpellCriticalMultiplierRepair);
    AddSpellPower(forumExport, "Rust           ", Breakdown_SpellPowerRust, Breakdown_SpellCriticalChanceRust, Breakdown_SpellCriticalMultiplierRust);
    AddSpellPower(forumExport, "Sonic          ", Breakdown_SpellPowerSonic, Breakdown_SpellCriticalChanceSonic, Breakdown_SpellCriticalMultiplierSonic);
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "\r\n";
}

void CForumExportDlg::AddSpellPower(
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

void CForumExportDlg::AddSpells(std::stringstream & forumExport)
{
    bool first = true;
    // check each possible class for selected spells
    std::vector<size_t> classLevels = m_pCharacter->ClassLevels(m_pCharacter->MaxLevel());
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
                    forumExport << "Spells                                      School         DC\r\n";
                    forumExport << "------------------------------------------------------------------------------------------\r\n";
                    first = false;
                }
                forumExport << EnumEntryText((ClassType)ci, classTypeMap) << " Spells\r\n";
                for (size_t spellLevel = 0; spellLevel < spellSlots.size(); ++spellLevel)
                {
                    // now output each fixed spell
                    std::list<TrainedSpell> fixedSpells = m_pCharacter->FixedSpells(
                            (ClassType)ci, spellLevel); // 0 based
                    AddSpellList(
                            forumExport,
                            (ClassType)ci,
                            fixedSpells,
                            spellLevel,
                            spellSlots.size());
                    // now output each selected spell
                    std::list<TrainedSpell> trainedSpells = m_pCharacter->TrainedSpells(
                            (ClassType)ci, spellLevel + 1); // 1 based
                    AddSpellList(
                            forumExport,
                            (ClassType)ci,
                            trainedSpells,
                            spellLevel,
                            spellSlots.size());
                }
            }
        }
    }
    if (!first)
    {
        forumExport << "------------------------------------------------------------------------------------------\r\n";
        forumExport << "\r\n";
    }
}

void CForumExportDlg::AddSpellList(
        std::stringstream & forumExport,
        ClassType ct,
        const std::list<TrainedSpell> & spellList,
        size_t spellLevel,
        size_t maxSpellLevel) const
{
    std::list<TrainedSpell>::const_iterator it = spellList.begin();
    while (it != spellList.end())
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
        size_t spellDC = spell.SpellDC(
                *m_pCharacter,
                ct,
                spellLevel,
                maxSpellLevel);
        forumExport.width(3);
        forumExport << std::right << spellDC;
        forumExport << "\r\n";
        ++it;
    }
}

void CForumExportDlg::AddSLAs(std::stringstream & forumExport)
{
    bool first = true;
    // find the SLA control view and ask it for the list of SLAs
    CWnd * pWnd = AfxGetMainWnd();
    CMainFrame * pMainWnd = dynamic_cast<CMainFrame*>(pWnd);
    const CSLAControl * slaControl = pMainWnd->GetSLAControl();
    if (slaControl != NULL)
    {
        const std::list<SLA> & slas = slaControl->SLAs();
        std::list<SLA>::const_iterator it = slas.begin();
        while (it != slas.end())
        {
            if (first)
            {
                forumExport << "Spell Like / Special Abilities\r\n";
                forumExport << "------------------------------------------------------------------------------------------\r\n";
                first = false;
            }
            forumExport.width(44);
            forumExport << std::left << (*it).Name();
            forumExport << "\r\n";
            ++it;
        }
    }
    if (!first)
    {
        forumExport << "------------------------------------------------------------------------------------------\r\n";
        forumExport << "\r\n";
    }
}

void CForumExportDlg::AddWeaponDamage(std::stringstream & forumExport)
{
    forumExport << "Weapon Damage\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    BreakdownItem * pBI = FindBreakdown(Breakdown_MeleePower);
    forumExport << "Melee Power:  " << pBI->Total() << "\r\n";
    AddBreakdown(forumExport, "Doublestrike: ", 1, Breakdown_DoubleStrike);
    forumExport << "%\r\n";
    AddBreakdown(forumExport, "Strikethrough: ", 1, Breakdown_Strikethrough);
    forumExport << "%\r\n";
    AddBreakdown(forumExport, "Mainhand damage ability multiplier: ", 1, Breakdown_DamageAbilityMultiplier);
    forumExport << "\r\n";
    AddBreakdown(forumExport, "Offhand damage ability multiplier: ", 1, Breakdown_DamageAbilityMultiplierOffhand);
    forumExport << "\r\n";
    AddBreakdown(forumExport, "Off-Hand attack Chance: ", 1, Breakdown_OffHandAttackBonus);
    forumExport << "%\r\n";
    AddBreakdown(forumExport, "Fortification Bypass: ", 1, Breakdown_FortificationBypass);
    forumExport << "%\r\n";
    AddBreakdown(forumExport, "Dodge Bypass: ", 1, Breakdown_DodgeBypass);
    forumExport << "%\r\n";
    AddBreakdown(forumExport, "Helpless Damage bonus: ", 1, Breakdown_HelplessDamage);
    forumExport << "%\r\n";

    pBI = FindBreakdown(Breakdown_RangedPower);
    forumExport << "Ranged Power: " << pBI->Total();
    forumExport << "\r\n";
    AddBreakdown(forumExport, "Doubleshot Chance: ", 1, Breakdown_DoubleShot);
    forumExport << "%\r\n";
    forumExport << "\r\n";

    AddBreakdown(forumExport, "Sneak Attack Attack bonus: ", 1, Breakdown_SneakAttackAttack);
    forumExport << "\r\n";
    AddBreakdown(forumExport, "Sneak Attack Damage: ", 1, Breakdown_SneakAttackDice);
    forumExport << "d6+";
    AddBreakdown(forumExport, "", 1, Breakdown_SneakAttackDamage);
    forumExport << "\r\n";
    forumExport << "\r\n";

    EquippedGear gear = m_pCharacter->ActiveGearSet();
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

void CForumExportDlg::AddTacticalDCs(std::stringstream & forumExport)
{
    CWnd * pWnd = AfxGetMainWnd();
    CMainFrame * pMainWnd = dynamic_cast<CMainFrame*>(pWnd);
    const CDCView * pDCView = pMainWnd->GetDCView();
    if (pDCView != NULL)
    {
        const std::vector<CDCButton *> & dcs = pDCView->DCs();
        bool first = true;
        for (size_t i = 0; i < dcs.size(); ++i)
        {
            if (first)
            {
                forumExport << "Tactical DCs\r\n";
                forumExport << "------------------------------------------------------------------------------------------\r\n";
            }
            // this is an active stance
            const DC & dc = dcs[i]->GetDCItem();
            forumExport.fill(' ');
            forumExport.width(35);
            forumExport << std::left << dc.Name();
            forumExport << dc.DCBreakdown(m_pCharacter);
            forumExport << "\r\n";
            first = false;
        }
        if (!first)
        {
            forumExport << "------------------------------------------------------------------------------------------\r\n";
            forumExport << "\r\n";
        }
    }
}

void CForumExportDlg::AddGear(std::stringstream & forumExport)
{
    EquippedGear gear = m_pCharacter->ActiveGearSet();
    ExportGear(gear, forumExport, false);
}

void CForumExportDlg::AddSimpleGear(std::stringstream & forumExport)
{
    EquippedGear gear = m_pCharacter->ActiveGearSet();
    ExportGear(gear, forumExport, true);
}

void CForumExportDlg::ExportGear(
        const EquippedGear & gear,
        std::stringstream & forumExport,
        bool bSimple)
{
    forumExport << "Equipped Gear Set: " << gear.Name() << "\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    for (size_t gi = Inventory_Unknown; gi < Inventory_Count; ++gi)
    {
        if (gear.IsSlotRestricted((InventorySlotType)gi, m_pCharacter))
        {
            forumExport.width(10);
            forumExport << std::left << EnumEntryText((InventorySlotType)gi, InventorySlotTypeMap);
            forumExport << "    Restricted by another item in this gear set\r\n";
        }
        if (gear.HasItemInSlot((InventorySlotType)gi))
        {
            Item item = gear.ItemInSlot((InventorySlotType)gi);
            forumExport.width(10);
            forumExport << std::left << EnumEntryText((InventorySlotType)gi, InventorySlotTypeMap);
            forumExport << "    ";
            forumExport << item.Name();
            if (item.HasDropLocation())
            {
                forumExport << "   Drops in: " << item.DropLocation();
            }
            forumExport << "\r\n";
            if (!bSimple)
            {
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
            }
            // show any augment slots also
            bool bSetBonusSuppressed = false;
            std::vector<ItemAugment> augments = item.Augments();
            for (size_t i = 0; i < augments.size(); ++i)
            {
                if (augments[i].HasSelectedAugment())
                {
                    forumExport << "              ";
                    forumExport << augments[i].Type();
                    forumExport << ": ";
                    // may be a configurable augment
                    if (augments[i].HasValue())
                    {
                        CString text;
                        text.Format("%+.0f ", augments[i].Value());
                        forumExport << (LPCTSTR)text;
                    }
                    forumExport << augments[i].SelectedAugment();
                    const Augment & augment = FindAugmentByName(augments[i].SelectedAugment());
                    bSetBonusSuppressed |= augment.HasSuppressSetBonus();
                    forumExport << "\r\n";
                    const Augment & aug = FindAugmentByName(augments[i].SelectedAugment());
                    // also include any of the effect descriptions it may have
                    const std::list<std::string> & eds = aug.EffectDescription();
                    std::list<std::string>::const_iterator it = eds.begin();
                    while (it != eds.end())
                    {
                        CString processedDescription = (*it).c_str();
                        forumExport << "              ";
                        forumExport << processedDescription;
                        forumExport << "\r\n";
                        ++it;
                    }
                }
                else
                {
                    if (augments[i].Type() != "Mythic"
                            && augments[i].Type() != "Reaper"
                            && augments[i].Type() != "Reaper Ring")
                    {
                        forumExport << "              ";
                        forumExport << augments[i].Type();
                        forumExport << ": ";
                        forumExport << "Empty augment slot\r\n";
                    }
                }
            }
            // show any set bonuses (update name if suppressed)
            const std::list<std::string> & sets = item.SetBonus();
            std::list<std::string>::const_iterator sit = sets.begin();
            while (sit != sets.end())
            {
                const SetBonus& set = FindSetBonus((*sit));
                std::string processedDescription = set.Description();
                processedDescription = processedDescription.substr(0, processedDescription.find(':'));
                forumExport << "              ";
                forumExport << processedDescription;
                if (bSetBonusSuppressed)
                {
                    forumExport << " (Suppressed by selected augment)";
                }
                forumExport << "\r\n";
                ++sit;
            }
            // show artifact filigrees if this is the artifact
            if (item.HasMinorArtifact())
            {
                for (size_t fi = 0; fi < MAX_ARTIFACT_FILIGREE; ++fi)
                {
                    std::string filigree = gear.SentientIntelligence().GetArtifactFiligree(fi);
                    if (filigree != "")
                    {
                        forumExport << "              Filigree " << (fi + 1) << ": ";
                        forumExport << filigree;
                        if (gear.SentientIntelligence().IsRareArtifactFiligree(fi))
                        {
                            forumExport << "(Rare Version)";
                        }
                        forumExport << "\r\n";
                    }
                }
            }
            if (gi == Inventory_Weapon1)
            {
                // now add the Filigree upgrades
                bool bHadGem = false;
                for (size_t fi = 0; fi < MAX_FILIGREE; ++fi)
                {
                    std::string filigree = gear.SentientIntelligence().GetFiligree(fi);
                    if (filigree != "")
                    {
                        // add any sentient weapon Filigree to the list also
                        if (!bHadGem && gear.SentientIntelligence().HasPersonality())
                        {
                            forumExport << "              Sentient Weapon Personality: ";
                            forumExport << gear.SentientIntelligence().Personality();
                            forumExport << "\r\n";
                            bHadGem = true;
                        }
                        forumExport << "              Filigree " << (fi + 1) << ": ";
                        forumExport << filigree;
                        if (gear.SentientIntelligence().IsRareFiligree(fi))
                        {
                            forumExport << "(Rare Version)";
                        }
                        forumExport << "\r\n";
                    }
                }
            }
        }
    }
    forumExport << "\r\n";
    forumExport << "------------------------------------------------------------------------------------------\r\n";
    forumExport << "\r\n";
}

void CForumExportDlg::AddAlternateGear(std::stringstream & forumExport)
{
    // export all other gear layouts except the current one
    const std::list<EquippedGear> & setups = m_pCharacter->GearSetups();
    std::list<EquippedGear>::const_iterator it = setups.begin();
    while (it != setups.end())
    {
        if ((*it).Name() != m_pCharacter->ActiveGear())
        {
            ExportGear((*it), forumExport, true);
        }
        ++it;
    }
}

void CForumExportDlg::OnMoveUp()
{
    int sel = m_listConfigureExport.GetSelectionMark();
    if (sel > 0)
    {
        bool bOld = m_bShowSection[sel-1];
        m_bShowSection[sel-1] = m_bShowSection[sel];
        m_bShowSection[sel] = bOld;
        // move the option up also
        ForumExportSections old = m_SectionOrder[sel-1];
        m_SectionOrder[sel-1] = m_SectionOrder[sel];
        m_SectionOrder[sel] = old;
        m_listConfigureExport.SetItemState(
                sel,
                LVIS_SELECTED | LVIS_FOCUSED,
                LVIS_SELECTED | LVIS_FOCUSED);
        m_listConfigureExport.SetSelectionMark(sel-1);
        // update export and control list
        PopulateConfigureExportList();
        PopulateExport();
    }
}

void CForumExportDlg::OnMoveDown()
{
    int sel = m_listConfigureExport.GetSelectionMark();
    if (sel < FES_Count - 1)
    {
        bool bOld = m_bShowSection[sel+1];
        m_bShowSection[sel+1] = m_bShowSection[sel];
        m_bShowSection[sel] = bOld;
        // move the option up also
        ForumExportSections old = m_SectionOrder[sel+1];
        m_SectionOrder[sel+1] = m_SectionOrder[sel];
        m_SectionOrder[sel] = old;
        m_listConfigureExport.SetItemState(
                sel,
                LVIS_SELECTED | LVIS_FOCUSED,
                LVIS_SELECTED | LVIS_FOCUSED);
        m_listConfigureExport.SetSelectionMark(sel+1);
        // update export and control list
        PopulateConfigureExportList();
        PopulateExport();
    }
}
