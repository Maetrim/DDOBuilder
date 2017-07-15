// GearSetNameDialog.h
//
#pragma once
#include "Resource.h"
#include <string>

class Character;

class CGearSetNameDialog :
        public CDialog
{
    DECLARE_DYNAMIC(CGearSetNameDialog)

    public:
        CGearSetNameDialog(
                CWnd * pParent,
                const Character * pCharacter,
                const std::string & defaultName = "");
        virtual ~CGearSetNameDialog();

        std::string Name() const;

    // Dialog Data
        enum { IDD = IDD_NAME_GEAR_SET };

    protected:
        virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
        virtual BOOL OnInitDialog() override;
        virtual void OnOK() override;

        DECLARE_MESSAGE_MAP()
    private:
        const Character * m_pCharacter;
        CEdit m_editGearName;

        std::string m_name;
};
