// CMFCButtonMMRelay

#pragma once

class CMFCButtonMMRelay : public CMFCButton
{
    DECLARE_DYNAMIC(CMFCButtonMMRelay)

public:
    CMFCButtonMMRelay();
    virtual ~CMFCButtonMMRelay();

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    void SetTransparent();
};

