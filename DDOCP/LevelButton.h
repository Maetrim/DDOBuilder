// LevelButton.h
//

#pragma once

#include "Resource.h"
#include "ClassTypes.h"

// this window handles selection of stances and enhancement sub-option
class CLevelButton :
    public CStatic
{
    public:
        CLevelButton();

        void SetLevel(size_t level);
        void SetClass(ClassType ct);
        void SetSelected(bool selected);
        bool IsSelected() const;
        void SetIssueState(bool hasIssue);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CLevelButton)
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CLevelButton)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CLevelButton)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        ClassType m_class;
        CImage m_image;
        int m_level;
        bool m_bSelected;
        bool m_bHasIssue;
        CFont m_font;
};

//{{AFX_INSERT_LOCATION}}
