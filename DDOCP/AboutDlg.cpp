// AboutDlg.cpp
//
#include "stdafx.h"
#include "AboutDlg.h"
#include "BuildInfo.h"
#include "GlobalSupportFunctions.h"

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_VERSION, m_staticVersion);
    DDX_Control(pDX, IDC_BUG_REPORTERS, m_bugReporters);
    DDX_Control(pDX, IDC_STATIC_INFO_REQUEST, m_staticInfoRequest);

    CString text;
    text.Format("DDOBuilder, Version %s", BUILDINFO_VERSION);
    m_staticVersion.SetWindowText(text);

    int count = AllFeats().size();
    text.Format("Feat Count: %d", count);
    SetDlgItemText(IDC_STATIC_FEATCOUNT, text);

    count = Augments().size();
    text.Format("Augment Count: %d", count);
    SetDlgItemText(IDC_STATIC_AUGMENTCOUNT, text);

    count = Items().size();
    text.Format("Item Count: %d", count);
    SetDlgItemText(IDC_STATIC_ITEMCOUNT, text);

    count = Spells().size();
    text.Format("Spell Count: %d", count);
    SetDlgItemText(IDC_STATIC_SPELLCOUNT, text);

    count = EnhancementTrees().size();
    text.Format("Enhancement Tree Count: %d", count);
    SetDlgItemText(IDC_STATIC_TREECOUNT, text);

    count = OptionalBuffs().size();
    text.Format("Self and Party Buff Count: %d", count);
    SetDlgItemText(IDC_STATIC_PARTYBUFF_COUNT, text);

    count = SetBonuses().size();
    text.Format("Set Bonus Count: %d", count);
    SetDlgItemText(IDC_STATIC_SETBONUS_COUNT, text);

    m_bugReporters.SetWindowText(
            "In alphabetical order:\r\n"
            "\r\n"
            "aaron250, "
            "Aganthor, "
            "Amastris, "
            "Amberlink, "
            "awsome-dudes, "
            "Aymerick55, "
            "AzrielAngelus, "
            "Bjond, "
            "Brakkart, "
            "Caarb, "
            "Cardo, "
            "Carpone, "
            "Civet, "
            "Cleru, "
            "crcabanillas, "
            "cswob, "
            "Cybree, "
            "DaWiner, "
            "diquintino, "
            "DougSK, "
            "DwarvemHack, "
            "Erofen, "
            "Fanaval, "
            "fognozzle, "
            "Frinkky, "
            "FullPlayer20, "
            "Gemini-Dragon357, "
            "Gerkenstein, "
            "Gizah10, "
            "Gizmo0815, "
            "Gudlaugsson, "
            "Guntharm, "
            "hallowedone, "
            "Havader, "
            "Hawkwier, "
            "Hobgoblin, "
            "iAntiquary, "
            "Jalak21, "
            "jmonsky, "
            "Jokersir2, "
            "Kamdragon, "
            "Kaustics "
            "Kentpb, "
            "Kiriitto, "
            "Laur, "
            "LittleTealeaf, "
            "Lupito, "
            "outlawless, "
            "Madja, "
            "Maxxcore, "
            "MaxPaxy, "
            "mhoida, "
            "mikameow, "
            "Mystickal413, "
            "nadia72295, "
            "NaughtyNancy, "
            "Neith, "
            "Ntoukis, "
            "nymcraian, "
            "pastorman1974, "
            "pcloke123, "
            "peng, "
            "Potnia, "
            "Question2, "
            "Rauven, "
            "Rawel, "
            "RetroDark, "
            "rnett, "
            "Seljuck, "
            "Selvera, "
            "ShintaoMonk, "
            "Strambotica, "
            "Strimtom, "
            "Spodi, "
            "Swimms, "
            "Talchwi, "
            "TeamscorpioRI, "
            "ValariusK, "
            "ValkyrieDuskfire, "
            "Vilhena, "
            "Vychos, "
            "warioman91, "
            "wolvemd, "
            "Xycona, "
            "Zretch");
    m_bugReporters.SetSel(-1);    // nothing starts selected

    m_staticInfoRequest.SetWindowText("The following listed Set Bonus icons are missing and I need your"
        " help in acquiring them. If you can drop a screen shot or a section of one with the icon present"
        " in the posted GitHub issue thread that will be of great help to me.\n"
        "\n"
        "Amaunator's Blessing, "
        "Anger of the Avalanche, "
        "Anthem Melody, "
        "Astute Alchemist, "
        "Battle Arcanist, "
        "Beacon of Magic Set (Legendary), "
        "Chained Elementals, "
        "Crypt Raider Set (Legendary), "
        "Curse Necromancer Set, "
        "Draconic Resilience, "
        "Epic Captain's Set, "
        "Epic Double Helix Set, "
        "Epic The Devil's Handiwork, "
        "Erudition / Planar Conflux Set Bonus, "
        "Fastidious Fabicator, "
        "Greater Divine Blessing, "
        "Greater Elder's Knowledge, "
        "Greater Marshwalker, "
        "Greater Might of the Abishai, "
        "Greater Raven's Eye, "
        "Greater Shaman's Fury, "
        "Greater Siren's Ward, "
        "Greater Vulkoor's Cunning, "
        "Greater Vulkoor's Might, "
        "Knight of the Shadows Set (Legendary), "
        "Knight's Loyalty, "
        "Legacy of Tharne, "
        "Legendary Arcsteel Battlemage, "
        "Legendary Esoteric Initiate, "
        "Legendary Feywild Dreamer, "
        "Legendary Guardian of the Gates, "
        "Legendary Hruit's Influence, "
        "Legendary Inevitable Balance, "
        "Legendary Profane Experiment, "
        "Legendary Slavelords Might, "
        "Mantle of Suulomades, "
        "Masterful Magewright, "
        "Mind and Matter, "
        "Mountainskin Set, "
        "One with the Swarm, "
        "Pain and Suffering, "
        "Renegade Champion Set, "
        "Seasons of Change Set, "
        "Subterfuge / Planar Conflux Set Bonus, "
        "Tyrannical Tinkerer, "
        "Unbreakable Adamancy, "
        "Way of the Sun Soul, "
        "Woodsman's Guile");
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    GetDlgItem(IDOK)->SetFocus();

    return FALSE;
}
