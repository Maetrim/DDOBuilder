// SpellTip.cpp
//

#include "stdafx.h"
#include "SpellTip.h"

#include "EnhancementTreeItem.h"
#include "Feat.h"
#include "GlobalSupportFunctions.h"

namespace
{
    const int c_controlSpacing = 3;
}

CSpellTip::CSpellTip() :
    m_origin(CPoint(0, 0)),
    m_bRightAlign(false),
    m_class(Class_Unknown),
    m_DC(0),
    m_spellLevel(0),
    m_pCharacter(NULL)
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

CSpellTip::~CSpellTip()
{
}

BEGIN_MESSAGE_MAP(CSpellTip, CWnd)
    //{{AFX_MSG_MAP(CSpellTip)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSpellTip::Create(CWnd* pParentWnd) 
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

void CSpellTip::Show()
{
    CDC * pDC = GetDC();
    CSize windowSize;
    GetWindowSize(pDC, &windowSize);
    ReleaseDC(pDC);

    if (!m_bRightAlign)
    {
        // before actually showing the tip, determine whether it fits on the screen
        // if it extends below the bottom or past the right we move the window
        // accordingly to make sure it is visible. Do this for the monitor the
        // origin point for this tooltip is on.
        HMONITOR hMonitor = MonitorFromPoint(m_origin, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo;
        memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);
        CRect monitorSize(monitorInfo.rcWork);
        if (m_origin.x + windowSize.cx > monitorSize.right)
        {
            // move the tip left to ensure all text visible
            m_origin.x = monitorSize.right - windowSize.cx;
        }
        if (m_origin.y + windowSize.cy > monitorSize.bottom)
        {
            // move the tip above the origin position to ensure content visible
            m_origin.y = m_alternate.y - windowSize.cy; // alternate position
        }
    }
    else
    {
        // if it extends beyond the left we use the alternate position
        // accordingly to make sure it is visible. 
        // we move the tool tip up as required
        HMONITOR hMonitor = MonitorFromPoint(m_origin, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo;
        memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);
        CRect monitorSize(monitorInfo.rcWork);
        if (m_origin.x - windowSize.cx < monitorSize.left)
        {
            // need to use the alternate position
            m_origin = m_alternate;
        }
        else
        {
            m_origin.x -= (windowSize.cx + 2);
        }
        // alternate position is above
        if (m_origin.y + windowSize.cy > monitorSize.bottom)
        {
            // move the tip up
            m_origin.y = monitorSize.bottom - windowSize.cy;
        }
    }

    SetWindowPos(
            &wndTopMost, 
            m_origin.x, 
            m_origin.y, 
            windowSize.cx, 
            windowSize.cy, 
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void CSpellTip::SetOrigin(CPoint origin, CPoint alternate, bool rightAlign)
{
    m_origin = origin;
    m_alternate = alternate;
    m_bRightAlign = rightAlign;
}

void CSpellTip::Hide()
{ 
    ShowWindow(SW_HIDE);
}

void CSpellTip::OnPaint() 
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

    // get the height of a standard text line in the current font
    CSize standardLine = dc.GetTextExtent("A");
    int top = c_controlSpacing;
    int left = c_controlSpacing;

    // draw the icon
    m_image.TransparentBlt(
            dc.GetSafeHdc(),
            CRect(left, top, left + 32, top + 32),
            CRect(0, 0, 32, 32));

    // draw the spell name text
    CSize csName = dc.GetTextExtent(m_spell.Name().c_str());
    dc.TextOut(
            left + 32 + c_controlSpacing,
            top + c_controlSpacing + (32 - standardLine.cy) / 2,
            m_spell.Name().c_str());

    dc.SelectObject(m_standardFont);
    // draw spell cost
    CString text;
    if (m_spell.HasSPCost())
    {
        // its a hard coded spell cost
        if (m_spell.SPCost() != "")
        {
            CString costText;
            costText = m_spell.SPCost().c_str();
            text.Format("Cost %s", costText);
            dc.TextOut(
                    left + 32 + c_controlSpacing + csName.cx + c_controlSpacing,
                    top + c_controlSpacing,
                    text);
        }
    }
    else
    {
        // cost is 5sp + 5sp per level
        int cost = 5 + (m_spellLevel * 5);
                    CString costText;
        // now include the cost of any active metamagics
        //?? TBD
        costText.Format("%d", cost);
        text.Format("SP Cost %s", costText);
        dc.TextOut(
                left + 32 + c_controlSpacing + csName.cx + c_controlSpacing,
                top + c_controlSpacing,
                text);
    }
    if (m_spell.HasSchool())
    {
        // draw spell DC
        text.Format("%s DC %d",
                EnumEntryText(m_spell.School(), spellSchoolTypeMap),
                m_DC);
        dc.TextOut(
                left + 32 + c_controlSpacing + csName.cx + c_controlSpacing,
                top + standardLine.cy + c_controlSpacing,
                text);
    }
    // include caster level line
    CString txtCasterLevel;
    txtCasterLevel.Format(
            "Caster Level %2d  Max Caster Level %2d",
            m_spell.DynamicCasterLevel(*m_pCharacter, m_class),
            m_spell.DynamicMaxCasterLevel(*m_pCharacter, m_class));
    dc.TextOut(left, 32 + c_controlSpacing, txtCasterLevel);

    // draw the metamagics available
    // work out the max width off all metamagics
    std::vector<std::string> metas = m_spell.Metamagics();
    if (metas.size() > 0)
    {
        // spell has some metamagics, find the size of the largest
        dc.SelectObject(m_boldFont);
        dc.TextOut(
                rc.right - c_controlSpacing - m_csMetas.cx,
                top + c_controlSpacing,
                "Metamagics");
        dc.SelectObject(m_standardFont);
        for (size_t i = 0; i < metas.size(); ++i)
        {
            dc.TextOut(
                    rc.right - c_controlSpacing - m_csMetas.cx,
                    top + c_controlSpacing + standardLine.cy * (i + 1),
                    metas[i].c_str());
        }
        // draw vertical line separating metas from rest
        dc.MoveTo(rc.right - c_controlSpacing - m_csMetas.cx- c_controlSpacing, 0);
        dc.LineTo(rc.right - c_controlSpacing - m_csMetas.cx- c_controlSpacing, rc.bottom);
    }
    // draw the description
    dc.DrawText(
            m_spell.Description().c_str(),
            &m_rcDescription,
            DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);

    // Clean up GDI
    dc.RestoreDC(-1);
}

BOOL CSpellTip::GetWindowSize(CDC* pDC, CSize * size)
{
    ASSERT(pDC != NULL);
    // an Spell item looks like this:
    // +------------------------------------------+----------+
    // | +----+                        SP Cost xx |Metamagics|
    // | |icon| Spell Name         <School> DC nn |<type1>   |
    // | +----+                                   |<type2>   |
    // | Caster Level xx      Max caster level xx |<type3>...|
    // | +---------------------------------------+|<type4>   |
    // | |Description                            ||<type5>   |
    // | +---------------------------------------+|<type6>   |
    // +------------------------------------------+----------+
    CRect rcWnd(0, 0, 0, 0);
    // border space first
    rcWnd.InflateRect(c_controlSpacing, c_controlSpacing, c_controlSpacing, c_controlSpacing);
    pDC->SaveDC();
    pDC->SelectObject(m_boldFont);
    // get the height of a standard text line in the current font
    CSize standardLine = pDC->GetTextExtent("A");
    // Calculate the area for the tip text
    CSize csSpellName = pDC->GetTextExtent(m_spell.Name().c_str());
    rcWnd.bottom += max(standardLine.cy * 2, 32) + c_controlSpacing;  // 2 lines or icon height

    pDC->SelectObject(m_standardFont);
    CSize csSPCost = pDC->GetTextExtent("SP Cost ???");
    CString text;
    text.Format("%s DC %d",
            EnumEntryText(m_spell.School(), spellSchoolTypeMap),
            m_DC);
    CSize csDC = pDC->GetTextExtent(text);
    int topWidth = 32 + c_controlSpacing + csSpellName.cx
            + c_controlSpacing + max(csSPCost.cx, csDC.cx);

    // include caster level line
    CSize csCL = pDC->GetTextExtent("Caster Level XX  Max Caster Level XX");
    topWidth = max(topWidth, csCL.cx);
    rcWnd.bottom += csCL.cy;        // include line height

    m_rcDescription = CRect(0, 0, 0, 0);
    pDC->DrawText(
            m_spell.Description().c_str(),
            &m_rcDescription,
            DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);
    m_rcDescription += CPoint(0, rcWnd.Height());

    // work out the max width off all metamagics
    int metasHeight = 0;
    std::vector<std::string> metas = m_spell.Metamagics();
    m_csMetas = CSize(0, 0);
    if (metas.size() > 0)
    {
        // spell has some metamagics, find the size of the largest
        m_csMetas = pDC->GetTextExtent("Metamagics");
        for (size_t i = 0; i < metas.size(); ++i)
        {
            CSize cs = pDC->GetTextExtent(metas[i].c_str());
            if (cs.cx > m_csMetas.cx)
            {
                m_csMetas.cx = cs.cx;
            }
        }
        metasHeight += (standardLine.cy * (metas.size() + 1)) - rcWnd.Height() + (c_controlSpacing * 2);
        m_csMetas.cx += (c_controlSpacing * 2); // border between text and metamagics
    }
    // now work out the total window size
    int bottomWidth = m_rcDescription.Width();
    rcWnd.InflateRect(
            0,
            0,
            max(topWidth, bottomWidth) + m_csMetas.cx,
            max(m_rcDescription.Height(), metasHeight));

    // Set the window size
    if (size != NULL)
    {
        size->cx = rcWnd.Width();
        size->cy = rcWnd.Height();
    }
    pDC->RestoreDC(-1);

    return TRUE;
}

void CSpellTip::SetSpell(
        Character * charData,
        const Spell & spell,
        ClassType ct,
        size_t spellLevel,
        size_t maxSpellLevel)
{
    m_image.Destroy();
    m_spell = spell;
    m_class = ct;
    LoadImageFile(IT_spell, m_spell.Icon(), &m_image);
    m_DC = m_spell.SpellDC(*charData, ct, spellLevel, maxSpellLevel);
    m_spellLevel = spellLevel;
    m_pCharacter = charData;
}
