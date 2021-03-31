#pragma once

#include "DelayedListBox.h"

// CComboBoxTooltip
class CComboBoxTooltip : public CComboBox
{
    DECLARE_DYNAMIC(CComboBoxTooltip)

public:
    CComboBoxTooltip();
    virtual ~CComboBoxTooltip();

    void SetCanRemoveItems();

    void SetImageList(CImageList * il);
    // Derived class is responsible for implementing these handlers
    //   for owner/self draw controls (except for the optional DeleteItem)
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS) override;
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMis) override;
    virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCis) override;
    virtual void DeleteItem(LPDELETEITEMSTRUCT lpDis) override;
    void OnRButtonDown(int selection);
protected:
    afx_msg void OnSetFocus(CWnd * pWnd);
    DECLARE_MESSAGE_MAP()

    bool m_bHasImageList;
    CImageList m_imageList;
    int m_selection;
    bool m_bSubclassedListbox;
    CDelayedListBox m_delayedListBox;
    bool m_bCanRemoveItems;
};

