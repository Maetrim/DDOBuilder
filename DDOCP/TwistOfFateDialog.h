// TwistOfFateDialog.h
//

#pragma once

#include "Resource.h"
#include "MFCButtonMMRelay.h"
#include "InfoTip.h"
#include "Character.h"

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
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CTwistOfFateDialog)
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnButtonMinus();
        afx_msg void OnButtonPlus();
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        afx_msg void OnComboTwistSelect();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        // CharacterObserver overrides
        void UpdateFatePointsChanged(Character * charData) override;
        void UpdateAvailableTwistsChanged(Character * charData) override;
    private:
        void SetupControls();
        void ShowTip(CRect itemRect);
        void HideTip();
        void SetTooltipText(CPoint tipTopLeft);
        void PopulateTwistCombobox();
        size_t m_twistIndex;
        CButton m_buttonMinus;
        CButton m_buttonPlus;
        CStatic m_staticTier;
        CComboBoxEx m_comboTwistSelect;
        CImageList m_twistImages;
        Character * m_pCharacter;
        CBitmap m_bitmap;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        std::list<TrainedEnhancement> m_availableTwists;
};

//{{AFX_INSERT_LOCATION}}
