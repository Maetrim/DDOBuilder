#pragma once

#include "ElapsedTimer.h"
// CDelayedListBox

class CComboBoxTooltip;

class CDelayedListBox : public CListBox
{
    DECLARE_DYNAMIC(CDelayedListBox)
    void SetOwner(CComboBoxTooltip * pOwner);
    void ResetTimer();

public:
    CDelayedListBox();
    virtual ~CDelayedListBox();

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT);
    virtual int CompareItem(LPCOMPAREITEMSTRUCT);
    virtual void DeleteItem(LPDELETEITEMSTRUCT);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnLButtonUp(UINT uFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
    ElapsedTimer m_elapsedTime;
    CComboBoxTooltip * m_pOwner;

    bool m_bDoReset;
};


