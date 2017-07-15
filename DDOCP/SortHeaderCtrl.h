/*----------------------------------------------------------------------
Copyright (C)2001 MJSoft. All Rights Reserved.
          This source may be used freely as long as it is not sold for
                    profit and this copyright information is not altered or removed.
                    Visit the web-site at www.mjsoft.co.uk
                    e-mail comments to info@mjsoft.co.uk
File:     SortHeaderCtrl.h
Purpose:  Provides the header control, with drawing of the arrows, for
          the list control.
----------------------------------------------------------------------*/

#pragma once

class CSortHeaderCtrl :
    public CHeaderCtrl
{
    public:
        CSortHeaderCtrl();
        virtual ~CSortHeaderCtrl();

        void SetSortArrow(const int iColumn, const bool bAscending);
        void GetSortArrow(int *column, bool *bAscending);

    protected:
        void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

        int     m_iSortColumn;
        bool    m_bSortAscending;

        //{{AFX_VIRTUAL(CSortHeaderCtrl)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CSortHeaderCtrl)
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
