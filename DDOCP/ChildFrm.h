// ChildFrm.h
//
#pragma once

class CChildFrame : public CMDIChildWndEx
{
        DECLARE_DYNCREATE(CChildFrame)
    public:
        CChildFrame();
        virtual ~CChildFrame();

        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    protected:
        DECLARE_MESSAGE_MAP()
};
