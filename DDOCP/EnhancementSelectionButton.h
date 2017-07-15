// EnhancementSelectionButton.h
//

#pragma once

#include "Resource.h"
#include <string>

// this window handles selection of stances and enhancement sub-option
class CEnhancementSelectionButton :
    public CStatic
{
    public:
        CEnhancementSelectionButton();

        void SetImage(const std::string & imageName);
        void SetSelected(bool selected);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CEnhancementSelectionButton)
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CEnhancementSelectionButton)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CEnhancementSelectionButton)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        bool m_bSelected;
        bool m_bMadeGray;
        CImage m_image;
};

//{{AFX_INSERT_LOCATION}}
