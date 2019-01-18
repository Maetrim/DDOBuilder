// AllFeatsDialog.h
#pragma once
#include "Resource.h"

#include "Character.h"

class CAllFeatsDialog : public CDialog
{
    public:
        CAllFeatsDialog(Character * pCharacter);   // standard constructor
        virtual ~CAllFeatsDialog();

        // Dialog Data
        enum { IDD = IDD_ALL_FEATS };

        DECLARE_DYNAMIC(CAllFeatsDialog)

    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        DECLARE_MESSAGE_MAP()

    private:
        void PopulateSelectableFeatSlots();
        void PopulateAvailableFeats();

        CListCtrl m_listSelectedFeats;
        CListCtrl m_listAllAvailableFeats;

        Character * m_pCharacter;
        Character m_workingCopy;
};
