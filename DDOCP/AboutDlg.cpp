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
            "ASilver, "
            "awsome-dudes, "
            "Aymerick55, "
            "AzrielAngelus, "
            "Bjond, "
            "Brakkart, "
            "BWRStennett, "
            "Caarb, "
            "Cardo, "
            "Carpone, "
            "Cinnamoronroll, "
            "Civet, "
            "Cleru, "
            "cmecu, "
            "crcabanillas, "
            "Crolug, "
            "cswob, "
            "Cybree, "
            "DaWiner, "
            "Delosari, "
            "diquintino, "
            "DougSK, "
            "DracoFlame90, "
            "DwarvemHack, "
            "Erofen, "
            "Fanaval, "
            "Feteush, "
            "fognozzle, "
            "F4nelia, "
            "Frinkky, "
            "FullPlayer20, "
            "Gemini-Dragon357, "
            "Gerkenstein, "
            "Gizah10, "
            "Gizmo0815, "
            "Grunthorno, "
            "Gudlaugsson, "
            "Guntharm, "
            "hallowedone, "
            "Havader, "
            "Hawkwier, "
            "Hobgoblin, "
            "Hoidx, "
            "iAntiquary, "
            "InfraRiot, "
            "Jalak21, "
            "jhnsnc, "
            "jmonsky, "
            "Jokersir2, "
            "Kamatozer, "
            "Kamdragon, "
            "Kaustics "
            "Kenaustin, "
            "Kentpb, "
            "Kentos9, "
            "Kerkos, "
            "Kerthyn, "
            "Kiriitto, "
            "krizinja, "
            "KYRREHK, "
            "Laur, "
            "LittleTealeaf, "
            "LordTyphoon, "
            "Lupito, "
            "outlawless, "
            "PurpleSerpent, "
            "Madja, "
            "Maxxcore, "
            "MaxPaxy, "
            "mhoida, "
            "mikameow, "
            "MIvanIsten, "
            "Modern_Jesus, "
            "Mystickal413, "
            "nadia72295, "
            "NaughtyNancy, "
            "Neith, "
            "Ntoukis, "
            "nymcraian, "
            "pastorman1974, "
            "pcloke123, "
            "peng, "
            "PixieSlayer, "
            "Potnia, "
            "Question2, "
            "Rauven, "
            "Rawel, "
            "RetroDark, "
            "rnett, "
            "Seljuck, "
            "Selvera, "
            "Shakti-Orien, "
            "ShinuzukaRakam, "
            "ShintaoMonk, "
            "Strambotica, "
            "Strimtom, "
            "Spodi, "
            "Swimms, "
            "Talchwi, "
            "Tarrith, "
            "TeamscorpioRI, "
            "TheGoldyGopher, "
            "ValariusK, "
            "ValkyrieDuskfire, "
            "Viamel-1, "
            "Vilhena, "
            "Vychos, "
            "warioman91, "
            "wolvemd, "
            "Wrunt, "
            "Xycona, "
            "Zretch");
    m_bugReporters.SetSel(-1);    // nothing starts selected

    m_staticInfoRequest.SetWindowText("The following listed Set Bonus icons are missing and I need your"
        " help in acquiring them. If you can drop a screen shot or a section of one with the icon present"
        " in the posted GitHub issue thread that will be of great help to me.\n"
        "\n"
        "Anger of the Avalanche, "
        "Anthem Melody, "
        "Draconic Resilience, "
        "Greater Divine Blessing, "
        "Greater Elder's Knowledge, "
        "Greater Marshwalker, "
        "Greater Raven's Eye, "
        "Greater Shaman's Fury, "
        "Greater Siren's Ward, "
        "Greater Vulkoor's Cunning, "
        "Greater Vulkoor's Might, "
        "Legacy of Tharne, "
        "Legendary Inevitable Balance, "
        "Legendary Slavelords Might, "
        "Mind and Matter, "
        "One with the Swarm, "
        "Subterfuge / Planar Conflux Set Bonus, "
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
