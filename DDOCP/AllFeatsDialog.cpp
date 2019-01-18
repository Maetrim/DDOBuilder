// AllFeatsDialog.cpp
//
#include "stdafx.h"
#include "AllFeatsDialog.h"
#include "GlobalSupportFunctions.h"

// CAllFeatsDialog dialog

IMPLEMENT_DYNAMIC(CAllFeatsDialog, CDialog)

CAllFeatsDialog::CAllFeatsDialog(Character * pCharacter) :
    CDialog(CAllFeatsDialog::IDD, NULL),
    m_pCharacter(pCharacter)
{
    // make a copy of the character we are working on
    m_workingCopy = *m_pCharacter;
}

CAllFeatsDialog::~CAllFeatsDialog()
{
}

void CAllFeatsDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FEAT_LIST, m_listSelectedFeats);
    DDX_Control(pDX, IDC_LIST_ALL_FEATS, m_listAllAvailableFeats);
}

BEGIN_MESSAGE_MAP(CAllFeatsDialog, CDialog)
END_MESSAGE_MAP()

// CAllFeatsDialog message handlers
BOOL CAllFeatsDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_listSelectedFeats.InsertColumn(0, "Level", LVCFMT_LEFT, 75);
    m_listSelectedFeats.InsertColumn(1, "Feat Type", LVCFMT_LEFT, 125);
    m_listSelectedFeats.InsertColumn(2, "Feat", LVCFMT_LEFT, 250);

    m_listAllAvailableFeats.InsertColumn(0, "Selectable Feats", LVCFMT_LEFT, 250);

    PopulateSelectableFeatSlots();
    PopulateAvailableFeats();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAllFeatsDialog::PopulateSelectableFeatSlots()
{
    m_listSelectedFeats.DeleteAllItems();
    for (size_t level = 0; level < MAX_LEVEL; ++level)
    {
        std::vector<TrainableFeatTypes> slots =
                m_workingCopy.TrainableFeatTypeAtLevel(level);
        for (size_t fi = 0; fi < slots.size(); ++fi)
        {
            CString text;
            text.Format("%d", level + 1);
            size_t index = m_listSelectedFeats.InsertItem(
                    m_listSelectedFeats.GetItemCount(),
                    text,
                    0);
            text = TrainableFeatTypeLabel(slots[fi]);
            m_listSelectedFeats.SetItemText(index, 1, text);
        }
    }
}

void CAllFeatsDialog::PopulateAvailableFeats()
{
}

