// FeatSelectionDialog.h
//

#pragma once

#include "Resource.h"
#include "Feat.h"
#include "MFCButtonMMRelay.h"
#include "InfoTip.h"

class Character;

class CFeatSelectionDialog :
    public CDialog
{
    public:
        CFeatSelectionDialog(CWnd* pParent, const Feat & feat, TrainableFeatTypes type);

        void DoLeftClickAction();
        void DoRightClickAction();
    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CFeatSelectionDialog)
        enum { IDD = IDD_FEAT_SELECTION };
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CFeatSelectionDialog)
        virtual void DoDataExchange(CDataExchange* pDX);
        virtual BOOL OnInitDialog();
        virtual void OnCancel();
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CFeatSelectionDialog)
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnFeatButtonLeftClick();
        afx_msg void OnFeatButtonRightClick(NMHDR * pNotifyStruct, LRESULT * result);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void SetupControls();
        void LoadFeatBitmap();
        void ShowTip(CRect itemRect);
        void HideTip();
        void SetTooltipText(CPoint tipTopLeft, CPoint tipAlternate);
        CMFCButtonMMRelay m_featButton;
        Character * m_pCharacter;
        Feat m_feat;
        TrainableFeatTypes m_type;
        CBitmap m_bitmap;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        CFont m_buttonFont;
};

//{{AFX_INSERT_LOCATION}}
