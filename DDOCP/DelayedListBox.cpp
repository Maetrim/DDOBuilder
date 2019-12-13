// DelayedListBox.cpp : implementation file
//

#include "stdafx.h"
#include "DelayedListBox.h"
#include "ComboBoxTooltip.h"

// CDelayedListBox

IMPLEMENT_DYNAMIC(CDelayedListBox, CListBox)

CDelayedListBox::CDelayedListBox() :
    m_pOwner(NULL)
{

}

CDelayedListBox::~CDelayedListBox()
{
}


BEGIN_MESSAGE_MAP(CDelayedListBox, CListBox)
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CDelayedListBox message handlers

void CDelayedListBox::SetOwner(CComboBoxTooltip * pOwner)
{
    m_pOwner = pOwner;
}

void CDelayedListBox::ResetTimer()
{
    m_elapsedTime.Reset();
}

void CDelayedListBox::OnLButtonUp(UINT uFlags, CPoint point)
{
    // stop an immediate select
    if ((double)m_elapsedTime > 250.0)
    {
        CListBox::OnLButtonUp(uFlags, point);
    }
}

void CDelayedListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    m_pOwner->DrawItem(lpDIS);
}

void CDelayedListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    m_pOwner->MeasureItem(lpMIS);
}

int CDelayedListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
{
    return m_pOwner->CompareItem(lpCIS);
}

void CDelayedListBox::DeleteItem(LPDELETEITEMSTRUCT lpDIS)
{
    m_pOwner->DeleteItem(lpDIS);
}

