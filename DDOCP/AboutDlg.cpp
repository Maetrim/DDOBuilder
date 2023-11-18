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
            "84monta, "
            "aaron250, "
            "Aganthor, "
            "Amateretsu, "
            "Amastris, "
            "Amberlink, "
            "aphextf, "
            "Aquoia, "
            "Arakasu, "
            "ASilver, "
            "Attlanttizz, "
            "awsome-dudes, "
            "Augon, "
            "Aymerick55, "
            "AzrielAngelus, "
            "Bjond, "
            "Bolo_Grubb, "
            "Bowser_Koopa, "
            "Brakkart, "
            "BrentAuble, "
            "BWRStennett, "
            "Caarb, "
            "Cardo, "
            "Carpone, "
            "CBDunkerson, "
            "Chaos-Magus, "
            "christhemisss, "
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
            "DDO-Genjih, "
            "Delosari, "
            "deochii, "
            "Dielzen, "
            "diquintino, "
            "DougSK, "
            "DracoFlame90, "
            "DwarvemHack, "
            "Edrein, "
            "ElvinkinVvV, "
            "EmmaHarper, "
            "Erofen, "
            "EShadowbringer, "
            "Fanaval, "
            "Feteush, "
            "fognozzle, "
            "F4nelia, "
            "Frinkky, "
            "FullPlayer20, "
            "Gambrul, "
            "Gavaleus, "
            "Gemini-Dragon357, "
            "Gerkenstein, "
            "ghost, "
            "GillianGroks, "
            "Gizah10, "
            "Gizmo0815, "
            "gorocz, "
            "gpgooiker, "
            "Gregen, "
            "Grunthorno, "
            "Gudlaugsson, "
            "Guntharm, "
            "hallowedone, "
            "Hank-the-Ranger, "
            "Havader, "
            "Hawkwier, "
            "heartmanpd, "
            "hit_fido, "
            "Hobgoblin, "
            "Hoidx, "
            "Huntxrd, "
            "iAntiquary, "
            "IgorIvancic, "
            "iiNyxii, "
            "InfraRiot, "
            "iplaywithdrums, "
            "Jack Jarvis Esquire, "
            "Jalak21, "
            "jhnsnc, "
            "jleverett751, "
            "jmonsky, "
            "Jokersir2, "
            "Kamatozer, "
            "Kamdragon, "
            "Kamro1, "
            "Kaustics "
            "keluynn, "
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
            "Lokinet-Github, "
            "LordTyphoon, "
            "LouDaCrisp, "
            "Lupito, "
            "outlawless, "
            "PurpleSerpent, "
            "Madja, "
            "mathmatica, "
            "MaxPaxy, "
            "Maxxcore, "
            "melynnn007, "
            "mhoida, "
            "mikameow, "
            "MIvanIsten, "
            "Modern_Jesus, "
            "Mr Blue, "
            "MrDorak, "
            "MuazAlhaidar, "
            "Mummykiller, "
            "Mystickal413, "
            "nadia72295, "
            "NaughtyNancy, "
            "Nectmar, "
            "Neith, "
            "Nicodeamous, "
            "nkoiv, "
            "Ntoukis, "
            "nymcraian, "
            "Orcao, "
            "Ozmandias42, "
            "pastorman1974, "
            "pcloke123, "
            "peng, "
            "Pesh, "
            "pevergreen, "
            "philosopheckelman, "
            "PixieSlayer, "
            "Potnia, "
            "Question2, "
            "Rauven, "
            "Rawel, "
            "Remmer75, "
            "RetroDark, "
            "Riareos, "
            "rnett, "
            "saphyron, "
            "Sardabot, "
            "Seljuck, "
            "Selvera, "
            "Shakti-Orien, "
            "ShinuzukaRakam, "
            "ShintaoMonk, "
            "SmexyMan, "
            "smithbt, "
            "SpartanKiller13, "
            "Sqrlmonger, "
            "Strambotica, "
            "Strimtom, "
            "Somikamaru, "
            "SouCarioca, "
            "Spodi, "
            "Stoner81, "
            "Swimms, "
            "Talchwi, "
            "Talnar, "
            "TandyFW, "
            "Tarrith, "
            "TeamscorpioRI, "
            "Teog09, "
            "Texturace, "
            "ThanBogan, "
            "TheGoldyGopher, "
            "Toefer, "
            "Tronko, "
            "ValariusK, "
            "Valdieron, "
            "ValkyrieDuskfire, "
            "Viamel-1, "
            "Vilhena, "
            "vryxnr, "
            "Vvalmart-1, "
            "Vychos, "
            "warioman91, "
            "Whitering, "
            "wolfdy, "
            "wolvemd, "
            "Wrunt, "
            "wyldlock, "
            "Xycona, "
            "YoureNotDead, "
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
        "Deacon of the Auricular Sacrarium, "
        "Defender of Tanaroa, "
        "Devotion of the Firemouth, "
        "Echoes of the Walking Ancestors, "
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
