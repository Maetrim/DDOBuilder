// GroupLine.h
//

#pragma once

namespace MfcControls
{
    class CGroupLine :
        public CWnd
    {
        public:
            CGroupLine();
            ~CGroupLine();

            void SetWindowText(LPCTSTR lpszString);

        //{{AFX_VIRTUAL(CGroupLine)
        //}}AFX_VIRTUAL

        protected:
        //{{AFX_MSG(CGroupLine)
        afx_msg void OnEnable(BOOL bEnable);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };
}

//{{AFX_INSERT_LOCATION}}
