// DelayedListBox.cpp : implementation file
//

#include "stdafx.h"
#include "DelayedListBox.h"
#include "ComboBoxTooltip.h"

// CDelayedListBox

IMPLEMENT_DYNAMIC(CDelayedListBox, CListBox)

CDelayedListBox::CDelayedListBox() :
    m_pOwner(NULL),
    m_bDoReset(true)
{

}

CDelayedListBox::~CDelayedListBox()
{
}


BEGIN_MESSAGE_MAP(CDelayedListBox, CListBox)
    ON_WM_LBUTTONUP()
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CDelayedListBox message handlers

void CDelayedListBox::SetOwner(CComboBoxTooltip * pOwner)
{
    m_pOwner = pOwner;
}

void CDelayedListBox::ResetTimer()
{
    m_elapsedTime.Reset();
    m_bDoReset = false;
}

void CDelayedListBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
    // this is only called when the window is closed
    // thus we can say for certain we will need a timer reset when
    // the next draw item goes through this window (i.e. being shown again)
    m_bDoReset = true;
}

void CDelayedListBox::OnLButtonUp(UINT uFlags, CPoint point)
{
    // stop an immediate select unless 0.5 seconds has passed since the window
    // was initially displayed
    if ((double)m_elapsedTime > 500.0)
    {
        CListBox::OnLButtonUp(uFlags, point);
    }
}

void CDelayedListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    if (m_bDoReset)
    {
        ResetTimer();
    }
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

