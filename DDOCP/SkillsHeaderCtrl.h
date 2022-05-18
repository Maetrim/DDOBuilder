// SkillsHeaderCtrl.h
//

#pragma once

class CSkillsHeaderCtrl : public CHeaderCtrl
{
    public:
        CSkillsHeaderCtrl();
        virtual ~CSkillsHeaderCtrl();

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CMyHeader)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CMyHeader)
        afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
        afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
    private:
        int GetColumnUnderMouse(CPoint point);
};
