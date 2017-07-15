// InfoTip.cpp
//

#include "stdafx.h"
#include "InfoTip.h"

#include "EnhancementTreeItem.h"
#include "Feat.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const int c_controlSpacing = 3;
    COLORREF c_transparentColour = RGB(255, 128, 255);
}

CInfoTip::CInfoTip() :
    m_origin(CPoint(0, 0))
{
    // create the fonts used
    LOGFONT lf;
    ZeroMemory((PVOID)&lf, sizeof(LOGFONT));
    NONCLIENTMETRICS nm;
    nm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0));
    lf = nm.lfMenuFont;
    m_standardFont.CreateFontIndirect(&lf);
    lf.lfWeight = FW_BOLD;
    m_boldFont.CreateFontIndirect(&lf);
}

CInfoTip::~CInfoTip()
{
}

BEGIN_MESSAGE_MAP(CInfoTip, CWnd)
    //{{AFX_MSG_MAP(CInfoTip)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CInfoTip::Create(CWnd* pParentWnd) 
{
    // Must have a parent
    ASSERT(pParentWnd != NULL);

    BOOL bSuccess = CreateEx(
            NULL, 
            AfxRegisterWndClass(0), 
            NULL, 
            WS_POPUP, 
            0, 
            0, 
            0, 
            0, 
            pParentWnd->GetSafeHwnd(), 
            NULL, 
            NULL);

    return bSuccess;
}

void CInfoTip::Show()
{
    CDC * pDC = GetDC();
    CSize windowSize;
    GetWindowSize(pDC, &windowSize);
    ReleaseDC(pDC);

    SetWindowPos(
            &wndTop, //Most, 
            m_origin.x, 
            m_origin.y, 
            windowSize.cx, 
            windowSize.cy, 
            SWP_NOACTIVATE | SWP_SHOWWINDOW);

    // debug help code to identify why info tip does not show from a dialog

    //HDC hdc = ::GetDC(NULL);
    //::DrawEdge(
    //        hdc,
    //        CRect(m_origin.x, m_origin.y, m_origin.x + windowSize.cx, m_origin.y + windowSize.cy),
    //        EDGE_RAISED,
    //        BF_RECT);
    //::ReleaseDC(NULL, hdc);
}

void CInfoTip::SetOrigin(CPoint point)
{
    m_origin = point;
}

void CInfoTip::Hide()
{ 
    ShowWindow(SW_HIDE);
}

void CInfoTip::OnPaint() 
{
    CPaintDC dc( this ); // device context for painting

    CRect rc;
    CBrush tooltipColourBrush;
    CBrush frameBrush;

    // Get the client rectangle
    GetClientRect(rc);

    // Create the brushes
    frameBrush.CreateSolidBrush(::GetSysColor(COLOR_INFOTEXT));
    tooltipColourBrush.CreateSolidBrush(::GetSysColor(COLOR_INFOBK));

    // Draw the frame
    dc.FillRect(rc, &tooltipColourBrush);
    dc.DrawEdge(rc, BDR_SUNKENOUTER, BF_RECT);
    dc.SetBkMode(TRANSPARENT);

    dc.SaveDC();
    dc.SelectObject(m_boldFont);
    // draw the icon
    m_image.TransparentBlt(
            dc.GetSafeHdc(),
            CRect(c_controlSpacing, c_controlSpacing, c_controlSpacing + 32, c_controlSpacing + 32),
            CRect(0, 0, 32, 32));
    // render the title in bold
    CRect rcTitle(c_controlSpacing + 32 + c_controlSpacing, c_controlSpacing, rc.right, rc.bottom);
    dc.DrawText(m_title, rcTitle, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    CSize sizeCost = dc.GetTextExtent(m_cost);
    CSize sizeRanks = dc.GetTextExtent(m_ranks);
    dc.TextOut(rc.right - c_controlSpacing - sizeCost.cx, c_controlSpacing, m_cost);
    COLORREF old = dc.SetTextColor(RGB(0,128, 0));
    dc.TextOut(rc.right - c_controlSpacing - sizeRanks.cx, c_controlSpacing + sizeCost.cy, m_ranks);
    dc.SetTextColor(old);

    dc.SelectObject(m_standardFont);
    // optional requirements list shown above description, 1 per line
    int top = c_controlSpacing + max(sizeRanks.cy * 2, 32) + c_controlSpacing;
    ASSERT(m_requirements.size() == m_bRequirementMet.size());
    for (size_t ri = 0; ri < m_requirements.size(); ++ri)
    {
        // text drawn in green or red depending on whether the requirement is met or not
        dc.SetTextColor(m_bRequirementMet[ri] ? RGB(0, 128, 0) : RGB(255, 0, 0));
        CRect rctRequirement(c_controlSpacing, top, rc.right, rc.bottom);
        dc.TextOut(c_controlSpacing, top, m_requirements[ri]);
        top += dc.GetTextExtent(m_requirements[ri]).cy;
    }
    dc.SetTextColor(RGB(0, 0, 0));    // ensure black for the rest
    CRect rcDescription(c_controlSpacing, top, rc.right, rc.bottom);
    dc.DrawText(m_description, &rcDescription, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    // Clean up GDI
    dc.RestoreDC(-1);
}

BOOL CInfoTip::GetWindowSize(CDC* pDC, CSize * size)
{
    ASSERT(pDC != NULL);
    // an enhancement item looks like this:
    // +---------------------------------------------------+
    // | +----+ Enhancement name                    Cost x |
    // | |icon|                                    Ranks n |
    // | +----+                                            |
    // | [requirements list if any]                        |
    // | +------------------------------------------------+|
    // | |Description                                     ||
    // | +------------------------------------------------+|
    // +---------------------------------------------------+
    CRect rcWnd(0, 0, 0, 0);
    // border space first
    rcWnd.InflateRect(c_controlSpacing, c_controlSpacing, c_controlSpacing, c_controlSpacing);
    pDC->SaveDC();
    pDC->SelectObject(m_boldFont);
    // Calculate the area for the tip text
    CRect rcTitle;
    pDC->DrawText(m_title, &rcTitle, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    rcWnd.bottom += max(rcTitle.Height() * 2, 32) + c_controlSpacing;      // 2 lines or icon height

    pDC->SelectObject(m_standardFont);
    CRect rcRanks;
    pDC->DrawText(m_ranks, &rcRanks, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    size_t topWidth = 32 + c_controlSpacing + rcTitle.Width() + c_controlSpacing + rcRanks.Width();

    // optional requirements list shown above description, 1 per line
    ASSERT(m_requirements.size() == m_bRequirementMet.size());
    for (size_t ri = 0; ri < m_requirements.size(); ++ri)
    {
        CRect rctRequirement;
        pDC->DrawText(m_requirements[ri], &rctRequirement, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
        topWidth = max(topWidth, (size_t)rctRequirement.Width());
        rcWnd.InflateRect(
                0,
                0,
                0,
                rctRequirement.Height());
    }
    CRect rcDescription;
    pDC->DrawText(m_description, &rcDescription, DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);

    // now work out the total window size
    size_t bottomWidth = rcDescription.Width();
    rcWnd.InflateRect(
            0,
            0,
            max(topWidth, bottomWidth),
            rcDescription.Height());
    // Set the window size
    if (size != NULL)
    {
        size->cx = rcWnd.Width();
        size->cy = rcWnd.Height();
    }
    pDC->RestoreDC(-1);

    return TRUE;
}

void CInfoTip::SetEnhancementTreeItem(
        const Character & charData,
        const EnhancementTreeItem * pItem,
        size_t spentInTree)
{
    m_image.Destroy();
    LoadImageFile(IT_enhancement, pItem->Icon(), &m_image);
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return or possible \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_requirements.clear();
    m_bRequirementMet.clear();
    // add the required spent in tree to requirements list
    if (spentInTree < pItem->MinSpent())
    {
        CString text;
        text.Format("Requires: %d AP spent in tree", pItem->MinSpent());
        m_requirements.push_back(text);
        m_bRequirementMet.push_back(false);
    }
    pItem->CreateRequirementStrings(
            charData,
            &m_requirements,
            &m_bRequirementMet);
    m_cost.Format("Cost %d", pItem->Cost());
    m_ranks.Format("Ranks %d", pItem->Ranks());
}

void CInfoTip::SetEnhancementSelectionItem(
        const Character & charData,
        const EnhancementSelection * pItem,
        size_t ranks)
{
    m_image.Destroy();
    LoadImageFile(IT_enhancement, pItem->Icon(), &m_image);
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_requirements.clear();
    m_bRequirementMet.clear();
    pItem->CreateRequirementStrings(charData, &m_requirements, &m_bRequirementMet);
    m_cost.Format("Cost %d", pItem->Cost());
    m_ranks.Format("Ranks %d", ranks);
}

void CInfoTip::SetFeatItem(
        const Character & charData,
        const Feat * pItem)
{
    m_image.Destroy();
    LoadImageFile(IT_feat, pItem->Icon(), &m_image);
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_requirements.clear();
    m_bRequirementMet.clear();
    pItem->CreateRequirementStrings(charData, &m_requirements, &m_bRequirementMet);
    m_cost = "";
    if (pItem->MaxTimesAcquire() != 1)
    {
        m_ranks.Format("  Max Acquire %d", pItem->MaxTimesAcquire());
    }
    else
    {
        m_ranks = "";
    }
}

void CInfoTip::SetStanceItem(
        const Character & charData,
        const Stance * pItem)
{
    m_image.Destroy();
    if (S_OK != LoadImageFile(IT_enhancement, pItem->Icon(), &m_image, false))
    {
        // see if its a feat icon we need to use
        LoadImageFile(IT_feat, pItem->Icon(), &m_image);
    }
    m_image.SetTransparentColor(c_transparentColour);
    m_title = pItem->Name().c_str();
    m_description = pItem->Description().c_str();
    // actual carriage return are actual \n in text, convert to correct character
    GenerateLineBreaks(&m_title);
    GenerateLineBreaks(&m_description);
    m_requirements.clear();
    m_bRequirementMet.clear();
    // list the stances which cannot be active if this one is
    std::list<std::string> incompatibleStances = pItem->IncompatibleStance();
    std::list<std::string>::const_iterator it = incompatibleStances.begin();
    while (it != incompatibleStances.end())
    {
        CString name;
        name = "Incompatible with: ";
        name += (*it).c_str();
        m_requirements.push_back(name);
        ++it;
    }
    m_bRequirementMet.resize(m_requirements.size(), true);
    m_cost = "";
}

void CInfoTip::GenerateLineBreaks(CString * text)
{
    // this can work in two ways:
    // 1: If the text already contains "\n"'s use those
    // 2: If not, then break the text up automatically to around 80 characters
    //    in length maximum per line.
    size_t count = text->Replace("\\n", "\n");
    if (count == 0)
    {
        // may have embedded "\n"'s
        count = text->Find('\n', 0);
        if (count > (size_t)text->GetLength())
        {
            // looks like we have to do it manually
            size_t length = text->GetLength();
            size_t pos = 75;        // assume 7 characters in
            while (pos < length)
            {
                // look for the first space character from pos onwards
                if (text->GetAt(pos) == ' ')
                {
                    // change the space to a "\n"
                    text->SetAt(pos, '\n');
                    pos += 75;          // move a standard line length from there
                }
                else
                {
                    ++pos;
                }
            }
        }
    }
}
