// TreeListHeaderCtrl.h : header file
//

#pragma once
#include <afxtempl.h>

#define UWM_AUTO_RESIZE_COLUMN  _T("UWM_AUTO_RESIZE_COL_MESSAGE")

namespace MfcControls
{
    /////////////////////////////////////////////////////////////////////////////
    // CTreeListHeaderCtrl window

    class CTreeListHeaderCtrl :
        public CHeaderCtrl
    {
        DECLARE_DYNAMIC(CTreeListHeaderCtrl);

        public:
            CTreeListHeaderCtrl();
            virtual ~CTreeListHeaderCtrl();

            void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
            CImageList* SetImageList( CImageList* pImageList );

            int GetItemImage( int nItem );
            void SetItemImage( int nItem, int nImage );
            void SetAutofit(bool bAutofit = true) { m_bAutofit = bAutofit; Autofit(); }
            void SetColumnWidth(int column, int width);

            //{{AFX_VIRTUAL(CTreeListHeaderCtrl)    
            //}}AFX_VIRTUAL

        protected:
            //{{AFX_MSG(CTreeListHeaderCtrl)
            afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
            afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
            //}}AFX_MSG

            DECLARE_MESSAGE_MAP()
        private:
            void Autofit(int nOverrideItemData = -1, int nOverrideWidth = 0);

            BOOL m_RTL;
            BOOL m_bAutofit;
            CImageList *m_pImageList;
            CMap< int, int, int, int> m_mapImageIndex;
    };
}
