// ActiveStanceDialog.h
//

#pragma once

#include "Resource.h"
#include "Feat.h"
#include "Character.h"
#include "MFCButtonMMRelay.h"

class CActiveStanceDialog :
    public CDialog,
    public CharacterObserver
{
    public:
        CActiveStanceDialog(CWnd* pParent, Character * pCharacter, const Stance & stance);

        bool IsYou(const Stance & stance) const;
        void AddStack();
        void LoseStack();
        size_t NumStacks() const;
        ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CActiveStanceDialog)
        enum { IDD = IDD_ACTIVE_STANCE };
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CActiveStanceDialog)
        virtual void DoDataExchange(CDataExchange* pDX);
        virtual BOOL OnInitDialog();
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CActiveStanceDialog)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnButtonStance();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        // character observer overrides
        void UpdateStanceActivated(Character * charData, const std::string & stanceName) override;
        void UpdateStanceDeactivated(Character * charData, const std::string & stanceName) override;
    private:
        void SetupControls();
        Stance m_stance;
        Character * m_pCharacter;
        CMFCButtonMMRelay m_buttonStance;
        CMFCToolTipCtrl m_tooltip;
        CMFCToolTipInfo m_tooltipConfig;
        bool m_isActive;
        size_t m_stacks;
};

//{{AFX_INSERT_LOCATION}}
