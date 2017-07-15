// TreeListHeaderCtrl.cpp
//

#include "stdafx.h"
#include "TreeListHeaderCtrl.h"

using MfcControls::CTreeListHeaderCtrl;

/////////////////////////////////////////////////////////////////////////////
// CTreeListHeaderCtrl

IMPLEMENT_DYNAMIC(CTreeListHeaderCtrl, CHeaderCtrl);

const UINT UWM_AUTO_RESIZE_COL = ::RegisterWindowMessage(UWM_AUTO_RESIZE_COLUMN);

CTreeListHeaderCtrl::CTreeListHeaderCtrl()
{
    //initialise variables
    m_pImageList = NULL;
    m_RTL = FALSE;
}

CTreeListHeaderCtrl::~CTreeListHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CTreeListHeaderCtrl, CHeaderCtrl)
    //{{AFX_MSG_MAP(CTreeListHeaderCtrl)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListHeaderCtrl message handlers

void CTreeListHeaderCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    CDC dc;

    dc.Attach( lpDrawItemStruct->hDC );

    // Save DC
    int nSavedDC = dc.SaveDC();

    // Get the column rectangle
    CRect rcLabel( lpDrawItemStruct->rcItem );

    // Set clipping region to limit drawing within column
    CRgn rgn;
    rgn.CreateRectRgnIndirect( &rcLabel );
    dc.SelectObject( &rgn );
    rgn.DeleteObject();

    // Labels are offset by a certain amount
    // This offset is related to the width of a space character
    int offset = dc.GetTextExtent(_T(" "), 1 ).cx*2;


    // Draw image from image list

    // Get the column text and format
    TCHAR buf[256];
    HD_ITEM hditem;

    hditem.mask = HDI_TEXT | HDI_FORMAT;
    hditem.pszText = buf;
    hditem.cchTextMax = 255;

    GetItem( lpDrawItemStruct->itemID, &hditem );

    CFont font;
    font.CreateFont(14,0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,                 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH,             // nPitchAndFamily
    NULL);                     // lpszFacename

    dc.SelectObject(font);

    // Determine format for drawing column label
    UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
                        | DT_VCENTER | DT_END_ELLIPSIS;

    if( hditem.fmt & HDF_CENTER)
        uFormat |= DT_CENTER;
    else if( hditem.fmt & HDF_RIGHT)
        uFormat |= DT_RIGHT;
    else
        uFormat |= DT_LEFT;

    if(!(uFormat & DT_RIGHT))
    {
        // Adjust the rectangle if the mouse button is pressed on it
        if( lpDrawItemStruct->itemState == ODS_SELECTED )
        {
            rcLabel.left++;
            rcLabel.top += 2;
            rcLabel.right++;
        }

        rcLabel.left += offset;
        rcLabel.right -= offset;


        // Draw column label
        if( rcLabel.left < rcLabel.right )
            dc.DrawText(buf,-1,rcLabel, uFormat);
    }

    int imageIndex;
    if (m_pImageList &&
        m_mapImageIndex.Lookup( lpDrawItemStruct->itemID, imageIndex ) )
    {
        if( imageIndex != -1 )
        {
            if(uFormat & DT_RIGHT)
            // draw to the left of the label
                m_pImageList->Draw(&dc, imageIndex,
                        CPoint( rcLabel.left + offset,offset/3 ),
                        ILD_TRANSPARENT );
            else
            // draw to the right
                m_pImageList->Draw(&dc, imageIndex,
                        CPoint( rcLabel.right - dc.GetTextExtent(_T(buf), 1 ).cx*2,offset/3 ),
                        ILD_TRANSPARENT );


            // Now adjust the label rectangle
            IMAGEINFO imageinfo;
            if( m_pImageList->GetImageInfo( imageIndex, &imageinfo ) )
            {
                rcLabel.left += offset/2 +
                    imageinfo.rcImage.right - imageinfo.rcImage.left;
            }
        }
    }

    if(uFormat & DT_RIGHT)
    {
        // Adjust the rectangle if the mouse button is pressed on it
        if( lpDrawItemStruct->itemState == ODS_SELECTED )
        {
            rcLabel.left++;
            rcLabel.top += 2;
            rcLabel.right++;
        }

        rcLabel.left += offset;
        rcLabel.right -= offset;


        // Draw column label
        if( rcLabel.left < rcLabel.right )
            dc.DrawText(buf,-1,rcLabel, uFormat);
    }

    // Restore dc
    dc.RestoreDC( nSavedDC );

    // Detach the dc before returning
    dc.Detach();
}

CImageList* CTreeListHeaderCtrl::SetImageList( CImageList* pImageList )
{
    CImageList *pPrevList = m_pImageList;
    m_pImageList = pImageList;
    return pPrevList;
}

int CTreeListHeaderCtrl::GetItemImage( int nItem )
{
    int imageIndex;
    if( m_mapImageIndex.Lookup( nItem, imageIndex ) )
        return imageIndex;
    return -1;
}

void CTreeListHeaderCtrl::SetItemImage( int nItem, int nImage )
{
    // Save the image index
    m_mapImageIndex[nItem] = nImage;

    // Change the item to owner drawn
    HD_ITEM hditem;

    hditem.mask = HDI_FORMAT;
    GetItem( nItem, &hditem );
    hditem.fmt |= HDF_OWNERDRAW;
    SetItem( nItem, &hditem );

    // Invalidate header control so that it gets redrawn
    Invalidate();
}

void CTreeListHeaderCtrl::Autofit(int nOverrideItemData /*= -1*/, int nOverrideWidth /*= 0*/)
{
    int nItemCount = GetItemCount();
    int nTotalWidthOfColumns = 0;
    int nDifferenceInWidht;
    int nItem;
    HD_ITEM hi;
    CRect rClient;

    if (!m_bAutofit)
        return;

    SetRedraw(FALSE);

    GetParent()->GetClientRect(&rClient);
    if (-1 != nOverrideItemData)
        rClient.right -= nOverrideWidth;

    // Get total width of all columns
    for (nItem = 0; nItem < nItemCount; nItem++)
    {
        if (nItem == nOverrideItemData) // Don't mess with the item being resized by the user
            continue;

        hi.mask = HDI_WIDTH;
        GetItem(nItem, &hi);

        nTotalWidthOfColumns += hi.cxy;
    }

    if (nTotalWidthOfColumns != rClient.Width())
    {
        nDifferenceInWidht = abs(nTotalWidthOfColumns-rClient.Width()); // We need to shrink/expand all columns!

        // Shrink/expand all columns proportionally based on their current size
        for (nItem = 0; nItem < nItemCount; nItem++)
        {
            if (nItem == nOverrideItemData) // Skip the override column if there is one!
                continue;

            hi.mask = HDI_WIDTH;
            GetItem(nItem, &hi);

            hi.mask = HDI_WIDTH;
            hi.cxy = (hi.cxy * rClient.Width()) / nTotalWidthOfColumns;

            SetItem(nItem, &hi);
        }
    }

    SetRedraw(TRUE);
    Invalidate();
}

void CTreeListHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // Find if the user has double clicked on one of the item dividers
    int nColSelected = -1;
    for (int nCol = 0; nCol < GetItemCount(); nCol++)
    {
        CRect colRect;
        GetItemRect(nCol, &colRect);
        if (colRect.PtInRect(point))
        {
            colRect.DeflateRect(2,0);
            if (!colRect.PtInRect(point))
            {
                // Within the area we want it to be to resize automatically
                nColSelected = (point.x < colRect.left ? nCol-1 : nCol);
            }
            break;
        }
    }
    if (nColSelected > -1)
    {
        GetParent()->SendMessage(UWM_AUTO_RESIZE_COL, (WPARAM)nColSelected);
    }

    CHeaderCtrl::OnLButtonDblClk(nFlags, point);
}

void CTreeListHeaderCtrl::SetColumnWidth(int column, int width)
{
    ASSERT(column < GetItemCount());

    HD_ITEM hi;
    hi.mask = HDI_WIDTH;
    hi.cxy = width;
    SetItem(column, &hi);
}

void CTreeListHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    CHeaderCtrl::OnLButtonUp(nFlags, point);

    NMHDR hdr;
    hdr.hwndFrom = GetSafeHwnd();
    hdr.idFrom = GetDlgCtrlID();
    hdr.code = HDN_ENDTRACK;

    GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (long)&hdr);
}

