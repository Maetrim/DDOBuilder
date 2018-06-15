// TwistOfFateDialog.h
//

#pragma once

#include "Resource.h"
#include "MFCButtonMMRelay.h"
#include "Character.h"
#include "ComboBoxTooltip.h"
#include "InfoTip.h"

class CTwistOfFateDialog :
    public CDialog,
    public CharacterObserver
{
    public:
        CTwistOfFateDialog(CWnd* pParent, size_t twistIndex);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CTwistOfFateDialog)
        enum { IDD = IDD_TWIST_OF_FATE };
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CTwistOfFateDialog)
        virtual void DoDataExchange(CDataExchange* pDX);
        virtual BOOL OnInitDialog();
        virtual void OnCancel();
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CTwistOfFateDialog)
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnButtonMinus();
        afx_msg void OnButtonPlus();
        afx_msg void OnComboTwistSelect();
        afx_msg void OnComboTwistCancel();
        afx_msg LRESULT OnHoverComboBox(WPARAM wParam, LPARAM lParam);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        // CharacterObserver overrides
        void UpdateFatePointsChanged(Character * charData) override;
        void UpdateAvailableTwistsChanged(Character * charData) override;
    private:
        void SetupControls();
        void PopulateTwistCombobox();
        void ShowTip(const EnhancementTreeItem & item, CRect itemRect);
        void SetTooltipText(
            const EnhancementTreeItem & item,
            CPoint tipTopLeft,
            CPoint tipAlternate);
        size_t m_twistIndex;
        CButton m_buttonMinus;
        CButton m_buttonPlus;
        CStatic m_staticTier;
        CComboBoxTooltip m_comboTwistSelect;
        CImageList m_twistImages;
        Character * m_pCharacter;
        std::list<TrainedEnhancement> m_availableTwists;
        CInfoTip m_tooltip;
        bool m_bShowingTip;
        bool m_bIgnoreNextMessage;
};

//{{AFX_INSERT_LOCATION}}
