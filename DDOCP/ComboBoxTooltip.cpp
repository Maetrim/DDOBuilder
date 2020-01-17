// ComboBoxTooltip.cpp
//
// This specialised combo box has two functions:
// 1: It can display images to the left of items (set an image list on it)
// 2: When the drop list is displayed it notifies its parent which item
//    is selected so it can display a tooltip. This is done via a WM_MOUSEHOVER message
#include "stdafx.h"
#include "ComboBoxTooltip.h"

// CComboBoxTooltip
IMPLEMENT_DYNAMIC(CComboBoxTooltip, CComboBox)

CComboBoxTooltip::CComboBoxTooltip() :
    m_bHasImageList(false),
    m_selection(CB_ERR),
    m_bSubclassedListbox(false)
{
}

CComboBoxTooltip::~CComboBoxTooltip()
{
}

BEGIN_MESSAGE_MAP(CComboBoxTooltip, CComboBox)
    //ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CComboBoxTooltip::SetImageList(CImageList * il)
{
    m_bHasImageList = false;
    m_imageList.DeleteImageList();  // clear any previous image list
    if (il != NULL)
    {
        // create from list passed in
        m_imageList.Create(il);
        IMAGEINFO imageInfo;
        m_imageList.GetImageInfo(0, &imageInfo);
        // ensure the combo edit control height is correct
        CRect iconRect(imageInfo.rcImage);
        SetItemHeight(-1, iconRect.Height());
        m_bHasImageList = true;
    }
}

void CComboBoxTooltip::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
    bool isDropped = (GetDroppedState() != 0);
    if (!m_bSubclassedListbox)
    {
        COMBOBOXINFO cbInfo;
        memset(&cbInfo, 0, sizeof(COMBOBOXINFO));
        cbInfo.cbSize = sizeof(COMBOBOXINFO);
        GetComboBoxInfo(&cbInfo);
        m_delayedListBox.SetOwner(this);
        m_delayedListBox.SubclassWindow(cbInfo.hwndList);
        m_bSubclassedListbox = true;
    }
    if (lpDis->itemID != CB_ERR)
    {
        CDC *pDC = CDC::FromHandle(lpDis->hDC);
        CRect rcItem = lpDis->rcItem;
        int nState = lpDis->itemState;
        int nItem = lpDis->itemID;
        int nIndexDC = pDC->SaveDC();

        // highlight the background if selected.
        if (nState & ODS_SELECTED) 
        {
            pDC->FillSolidRect(rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
            pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            if (isDropped
                    && lpDis->itemID != m_selection
                    && lpDis->itemAction == ODA_SELECT
                    && (nState & 0x1000) == 0)  // Cancel operation
            {
                m_selection = lpDis->itemID;
                // this is the item we need to notify a hover about
                ::SendMessage(
                        GetParent()->GetSafeHwnd(),
                        WM_MOUSEHOVER,
                        lpDis->itemID,
                        GetDlgCtrlID());
            }
        }
        else
        {
            // white background, black text
            pDC->FillSolidRect(rcItem, RGB(255, 255, 255));
            pDC->SetTextColor(RGB(0, 0, 0));
        }

        // draw image if it has one
        CRect rcText(lpDis->rcItem);
        if (m_bHasImageList)
        {
            IMAGEINFO imageInfo;
            m_imageList.GetImageInfo(0, &imageInfo);
            CRect iconRect(imageInfo.rcImage);
            // ensure text is drawn to the right of the image
            rcText.left += rcItem.Height() + 2;
            m_imageList.Draw(pDC, lpDis->itemData, rcItem.TopLeft(), ILD_NORMAL);
        }

        pDC->SetBkMode(TRANSPARENT);    // draw text in transparent mode
        // Draw the text item.
        CString strFontName;
        GetLBText(nItem, strFontName);
        pDC->DrawText(strFontName, rcText, DT_VCENTER | DT_SINGLELINE);

        // Restore the original device context.
        pDC->RestoreDC(nIndexDC);
    }
}

void CComboBoxTooltip::MeasureItem(LPMEASUREITEMSTRUCT lpMis)
{
    ASSERT(lpMis->CtlType == ODT_COMBOBOX);
    if (m_bHasImageList)
    {
        IMAGEINFO imageInfo;
        m_imageList.GetImageInfo(0, &imageInfo);
        CRect iconRect(imageInfo.rcImage);
        lpMis->itemHeight = max(::GetSystemMetrics(SM_CYVTHUMB), iconRect.Height());
        lpMis->itemWidth  = 0;
    }
    else
    {
        // default control height
        lpMis->itemHeight = ::GetSystemMetrics(SM_CYVTHUMB);
        lpMis->itemWidth  = 0;
    }
}

int CComboBoxTooltip::CompareItem(LPCOMPAREITEMSTRUCT lpCis)
{
    ASSERT(lpCis->CtlType == ODT_COMBOBOX);

    int iItem1 = (int)lpCis->itemID1;
    int iItem2 = (int)lpCis->itemID2;
    if (iItem1 != CB_ERR && iItem2 != CB_ERR)
    {
        CString strItem1;
        GetLBText(iItem1, strItem1);
        CString strItem2;
        GetLBText(iItem2, strItem2);
        return strItem1.Collate(strItem2);
    }
    return -1;
}

void CComboBoxTooltip::DeleteItem(LPDELETEITEMSTRUCT lpDis)
{
}

void CComboBoxTooltip::OnSetFocus(CWnd * pWnd)
{
    m_delayedListBox.ResetTimer();
    CComboBox::OnSetFocus(pWnd);
}


