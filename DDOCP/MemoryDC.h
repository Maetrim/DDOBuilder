// MemoryDC.h
//

#pragma once

namespace MfcControls
{
    class MemoryDC : public CDC
    {
        public:
            // Constructor sets up the memory DC
            MemoryDC(CDC* pDC, const CRect * pRect = NULL);
            // Destructor copies the contents of the memory DC to the original DC
             ~MemoryDC();
            // Allow usage as a pointer
            MemoryDC* operator->();
            // Allow usage as a pointer
            operator MemoryDC*();

        private:
            BOOL m_bMemDC;                      // TRUE if not printing
            CBitmap m_MemBitmap;                // off screen bitmap
            CDC * m_pDC;                        // saves CDC passed in constructor
            CRect m_rect;                       // rectangle of drawing area

            // GDI stuff to be saved and restored
            CBitmap * m_pOldBitmap;             // original bitmap
            CBrush * m_pOldBrush;               // original brush
            CFont * m_pOldFont;                 // original font
            COLORREF m_crOldBkColor;            // original background color
            COLORREF m_crOldTextColor;          // original text color
            CPen * m_pOldPen;                   // original pen
            CPoint m_ptOldOrigin;               // original origin
            int m_nOldBkMode;                   // original background mode
    };
}
