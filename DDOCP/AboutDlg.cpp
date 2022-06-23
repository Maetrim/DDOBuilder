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
            "aaron250, "
            "Aganthor, "
            "Amateretsu, "
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
            "CBDunkerson, "
            "Chaos-Magus, "
            "Cinnamoronroll, "
            "Civet, "
            "Cleru, "
            "cmecu, "
            "constantinetx-github, "
            "crcabanillas, "
            "Crolug, "
            "Cromfrein, "
            "cswob, "
            "Cybree, "
            "DaWiner, "
            "Delosari, "
            "diquintino, "
            "DougSK, "
            "DracoFlame90, "
            "DwarvemHack, "
            "EmmaHarper, "
            "Erofen, "
            "Fanaval, "
            "Feteush, "
            "fognozzle, "
            "F4nelia, "
            "Frinkky, "
            "FullPlayer20, "
            "Gemini-Dragon357, "
            "Gerkenstein, "
            "ghost, "
            "Gizah10, "
            "Gizmo0815, "
            "Gregen, "
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
            "iplaywithdrums, "
            "Jalak21, "
            "jhnsnc, "
            "jmonsky, "
            "Jokersir2, "
            "Kamatozer, "
            "Kamdragon, "
            "Kaustics "
            "Kenaustin, "
            "Kentos9, "
            "Kentpb, "
            "Kerkos, "
            "Kerthyn, "
            "Khalibano, "
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
            "mathmatica, "
            "MaxPaxy, "
            "Maxxcore, "
            "mhoida, "
            "mikameow, "
            "MIvanIsten, "
            "Modern_Jesus, "
            "MrDorak, "
            "Mystickal413, "
            "nadia72295, "
            "NaughtyNancy, "
            "Nectmar, "
            "Neith, "
            "Ntoukis, "
            "nymcraian, "
            "Orcao, "
            "pastorman1974, "
            "pcloke123, "
            "peng, "
            "pevergreen, "
            "PixieSlayer, "
            "Potnia, "
            "Question2, "
            "Rauven, "
            "Rawel, "
            "RetroDark, "
            "rnett, "
            "saphyron, "
            "Seljuck, "
            "Selvera, "
            "Shakti-Orien, "
            "ShinuzukaRakam, "
            "ShintaoMonk, "
            "smithbt, "
            "Strambotica, "
            "Strimtom, "
            "SouCarioca, "
            "Spodi, "
            "Swimms, "
            "Talchwi, "
            "Tarrith, "
            "TeamscorpioRI, "
            "Teog09, "
            "Texturace, "
            "TheGoldyGopher, "
            "Toefer, "
            "ValariusK, "
            "Valdieron, "
            "ValkyrieDuskfire, "
            "Viamel-1, "
            "Vilhena, "
            "Vychos, "
            "warioman91, "
            "Whitering, "
            "wolfdy, "
            "wolvemd, "
            "Wrunt, "
            "Xycona, "
            "zilgabex, "
            "Zretch");
    m_bugReporters.SetSel(-1);    // nothing starts selected

    m_staticInfoRequest.SetWindowText("The following listed Set Bonus icons are missing and I need your"
        " help in acquiring them. If you can drop a screen shot or a section of one with the icon present"
        " in the posted GitHub issue thread that will be of great help to me.\n"
        "\n"
        "Anthem Melody, "
        "Draconic Resilience, "
        "Escalation, "
        "Legendary Kundarak Delving Equipment, "
        "Legendary Slavelords Might, "
        "Way of the Sun Soul, "
        "Woodsman's Guile.");
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    GetDlgItem(IDOK)->SetFocus();

    return FALSE;
}
