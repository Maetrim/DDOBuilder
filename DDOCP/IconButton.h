// IconButton.h
//

#pragma once

// this window handles selection of stances and enhancement sub-option
class CIconButton :
    public CStatic
{
    public:
        CIconButton();

        void SetSelected(bool selected);
        bool IsSelected() const;

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CIconButton)
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CIconButton)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CIconButton)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        bool m_bSelected;
        CImage m_image;

        friend class CEnhancementsView;
};

//{{AFX_INSERT_LOCATION}}
