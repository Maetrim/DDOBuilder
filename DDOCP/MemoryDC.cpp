// MemoryDC.cpp
//

#pragma once
#include "stdafx.h"
#include "MemoryDC.h"

using MfcControls::MemoryDC;

MemoryDC::MemoryDC(CDC* pDC, const CRect * pRect) :
    CDC()
{
    // make sure the call provided a valid device context
    ASSERT(pDC != NULL);

    // save the original device context for later BitBlt()
    m_pDC = pDC;

    // check whether we're printing
    m_bMemDC = !pDC->IsPrinting();

    // get the output rectangle
    if (pRect == NULL)
    {
        pDC->GetClipBox(&m_rect);
    }
    else
    {
        m_rect = *pRect;
    }

    if (m_bMemDC)
    {
        // do the memory DC stuff if not printing
        // create a memory DC
        CreateCompatibleDC(pDC);
        // create new bitmap and save old bitmap
        pDC->LPtoDP(&m_rect);
        m_MemBitmap.CreateCompatibleBitmap(
                pDC, 
                m_rect.Width(), 
                m_rect.Height());
        m_pOldBitmap = SelectObject(&m_MemBitmap);
        SetMapMode(pDC->GetMapMode());
        SetWindowExt(pDC->GetWindowExt());
        SetViewportExt(pDC->GetViewportExt());
        pDC->DPtoLP(&m_rect);

        // save old background brush
        m_pOldBrush = GetCurrentBrush();

        // save old font
        m_pOldFont = GetCurrentFont();

        // save old background color
        m_crOldBkColor = GetBkColor();

        // save old text color
        m_crOldTextColor = GetTextColor();

        // save old pen
        m_pOldPen = GetCurrentPen();

        // save old background mode
        m_nOldBkMode = GetBkMode();

        // save old window origin and set new one
        m_ptOldOrigin = SetWindowOrg(m_rect.left, m_rect.top);
    }
    else
    {
        // make a copy of the relevant parts of the original DC for printing
        m_bPrinting = pDC->m_bPrinting;
        m_hDC       = pDC->m_hDC;
        m_hAttribDC = pDC->m_hAttribDC;
    }

    // fill background 
    FillSolidRect(m_rect, m_crOldBkColor);
}

// Destructor copies the contents of the memory DC to the original DC
MemoryDC::~MemoryDC()
{
    if (m_bMemDC)
    {
        // copy the off screen bitmap onto the screen
        m_pDC->BitBlt(
                m_rect.left,
                m_rect.top,
                m_rect.Width(),
                m_rect.Height(),
                this,
                m_rect.left,
                m_rect.top,
                SRCCOPY);

        // restore the original DC settings
        SelectObject(m_pOldBitmap);
        SelectObject(m_pOldBrush);
        SelectObject(m_pOldFont);
        SetBkColor(m_crOldBkColor);
        SetTextColor(m_crOldTextColor);
        SelectObject(m_pOldPen);
        SetBkMode(m_nOldBkMode);
        SetWindowOrg(m_ptOldOrigin);
    }
    else
    {
        // all we need to do is replace the DC with an illegal value,
        // this keeps us from accidently deleting the handles associated
        // with the CDC that was passed to the constructor
        m_hDC = m_hAttribDC = NULL;
    }
}

// Allow usage as a pointer
MemoryDC* MemoryDC::operator->()
{
    return this;
}

// Allow usage as a pointer
MemoryDC::operator MemoryDC*()
{
    return this;
}
