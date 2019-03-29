// InventoryDialog.cpp
//

#include "stdafx.h"
#include "InventoryDialog.h"
#include "DDOCPDoc.h"
#include "GlobalSupportFunctions.h"
#include "SelectionSelectDialog.h"
#include "MainFrm.h"

namespace
{
    const size_t c_controlSpacing = 3;
    // inventory window size
    const int c_sizeX = 418;
    const int c_sizeY = 290;
    const int c_jewelX = 357;
    const int c_jewelY = 24;
    const int c_filigreeX = 232;
    const int c_filigreeY = 115;
    const int c_filigreeXOffset = 36;
    const int c_filigreeYOffset = 52;
}

#pragma warning(push)
#pragma warning(disable: 4407) // warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
BEGIN_MESSAGE_MAP(CInventoryDialog, CDialog)
    //{{AFX_MSG_MAP(CInventoryDialog)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()
#pragma warning(pop)

CInventoryDialog::CInventoryDialog(CWnd* pParent) :
    CDialog(CInventoryDialog::IDD, pParent),
    m_pCharacter(NULL),
    m_bitmapSize(CSize(0, 0)),
    m_showingItemTip(false),
    m_showingFiligreeTip(false),
    m_tipCreated(false),
    m_tooltipItem(Inventory_Unknown),
    m_tooltipFiligree(-2)
{
    //{{AFX_DATA_INIT(CInventoryDialog)
    //}}AFX_DATA_INIT
    // there are a fixed list of hit boxes which are hard coded here
    // order is important as used for drawing items (declare in same order as enum InventorySlotType)
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Arrows, CRect(164, 237, 196, 269)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Armor, CRect(30, 78, 62, 110)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Belt, CRect(164, 124, 196, 156)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Boots, CRect(72, 180, 104, 212)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Bracers, CRect(30, 124, 62, 156)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Cloak, CRect(164, 78, 196, 110)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Gloves, CRect(117, 180, 149, 212)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Goggles, CRect(30, 35, 62, 67)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Helmet, CRect(72, 24, 104, 56)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Necklace, CRect(117, 24, 149, 56)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Quiver, CRect(117, 237, 149, 269)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Ring1, CRect(30, 169, 62, 200)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Ring2, CRect(164, 168, 196, 200)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Trinket, CRect(164, 35, 196, 67)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Weapon1, CRect(30, 237, 62, 269)));
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_Weapon2, CRect(72, 237, 104, 269)));
    // fixed hit boxes for Filigree items
    m_hitBoxesFiligrees.push_back(FiligreeHitBox(-1, CRect(c_jewelX, c_jewelY, c_jewelX + 34, c_jewelY + 34)));
    CRect filigreeLocation(c_filigreeX, c_filigreeY, c_filigreeX + 34, c_filigreeY + 48);
    for (size_t i = 0; i < MAX_FILIGREE; ++i)
    {
        m_hitBoxesFiligrees.push_back(FiligreeHitBox(i, filigreeLocation));
        // move location for next filigree
        if ((i % 5) == 4)
        {
            // start of next row
            filigreeLocation += CPoint(c_filigreeXOffset * -4, c_filigreeYOffset);
        }
        else
        {
            // move across one
            filigreeLocation += CPoint(c_filigreeXOffset, 0);
        }
    }
    // load images used
    LoadImageFile(IT_ui, "Inventory", &m_imageBackground);
    LoadImageFile(IT_ui, "Inventory", &m_imageBackgroundDisabled);
    LoadImageFile(IT_ui, "CannotEquip", &m_imagesCannotEquip);
    LoadImageFile(IT_ui, "SpellSlotTrainable", &m_imagesJewel);
    LoadImageFile(IT_ui, "RareUnchecked", &m_imagesFiligree);
    LoadImageFile(IT_ui, "RareChecked", &m_imagesFiligreeRare);
    MakeGrayScale(&m_imageBackgroundDisabled, c_transparentColour);
}

void CInventoryDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CInventoryDialog)
    //}}AFX_DATA_MAP
}

void CInventoryDialog::SetGearSet(const EquippedGear & gear)
{
    m_gearSet = gear;
    Invalidate();
}

BOOL CInventoryDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_tooltip.Create(this);
    m_tipCreated = true;

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInventoryDialog::OnEraseBkgnd(CDC* pDC)
{
    return 0;
}

void CInventoryDialog::OnPaint()
{
    CPaintDC pdc(this); // validates the client area on destruction
    pdc.SaveDC();
    // ensure we have a background bitmap
    if (m_bitmapSize == CSize(0, 0))
    {
        m_cachedDisplay.DeleteObject(); // ensure
        // create the bitmap we will render to
        m_cachedDisplay.CreateCompatibleBitmap(
                &pdc,
                c_sizeX,
                c_sizeY);
        m_bitmapSize = CSize(c_sizeX, c_sizeY);
    }
    // draw to a compatible device context and then splat to screen
    CDC memoryDc;
    memoryDc.CreateCompatibleDC(&pdc);
    memoryDc.SaveDC();
    memoryDc.SelectObject(&m_cachedDisplay);
    memoryDc.SelectStockObject(DEFAULT_GUI_FONT);

    // first render the inventory background
    if (IsWindowEnabled())
    {
        m_imageBackground.BitBlt(
                memoryDc.GetSafeHdc(),
                CRect(0, 0, c_sizeX, c_sizeY),
                CPoint(0, 0),
                SRCCOPY);
    }
    else
    {
        m_imageBackgroundDisabled.BitBlt(
                memoryDc.GetSafeHdc(),
                CRect(0, 0, c_sizeX, c_sizeY),
                CPoint(0, 0),
                SRCCOPY);
    }

    // now iterate the current inventory and draw the item icons
    for (size_t i = Inventory_Unknown + 1; i < Inventory_Count; ++i)
    {
        if (i == Inventory_Weapon2
                && m_gearSet.HasItemInSlot(Inventory_Weapon1)
                && !CanEquipTo2ndWeapon(m_gearSet.ItemInSlot(Inventory_Weapon1)))
        {
            // Weapon equipped in main hand that precludes weapon in off hand
            // do not permit selection of an item in the off hand slot
            CRect itemRect = m_hitBoxesInventory[i - 1].Rect();
            m_imagesCannotEquip.TransparentBlt(
                    memoryDc.GetSafeHdc(),
                    itemRect.left,
                    itemRect.top,
                    32,
                    32);
        }
        if (m_gearSet.HasItemInSlot((InventorySlotType)i))
        {
            Item item = m_gearSet.ItemInSlot((InventorySlotType)i);
            CImage image;
            LoadImageFile(IT_item, item.Icon(), &image);
            CRect itemRect = m_hitBoxesInventory[i - 1].Rect();
            image.TransparentBlt(
                    memoryDc.GetSafeHdc(),
                    itemRect.left,
                    itemRect.top,
                    32,
                    32);
        }
    }
    // now pain the Jewel and Filigrees
    if (IsWindowEnabled()
            && m_gearSet.HasItemInSlot(Inventory_Weapon1)
            && m_gearSet.ItemInSlot(Inventory_Weapon1).HasSentientIntelligence())
    {
        SentientJewel jewel = m_gearSet.ItemInSlot(Inventory_Weapon1).SentientIntelligence();
        m_imagesJewel.TransparentBlt(
                memoryDc.GetSafeHdc(),
                c_jewelX,
                c_jewelY,
                34,
                34);
        if (jewel.HasPersonality())
        {
            std::string personality = jewel.Personality();
            Augment jewelAugment = FindAugmentByName(personality);
            CImage image;
            LoadImageFile(IT_augment, jewelAugment.Icon(), &image);
            image.TransparentBlt(
                    memoryDc.GetSafeHdc(),
                    c_jewelX+1,
                    c_jewelY+1,
                    32,
                    32);
        }
        // now draw all the filigrees
        CSize filigreeLocation(c_filigreeX, c_filigreeY);
        size_t count = jewel.NumFiligrees();
        for (size_t i = 0; i < count; ++i)
        {
            // and draw the selected filigree icon if there is one
            Augment filigree = FindAugmentByName(jewel.Filigree(i));
            if (filigree.Name() != "")     // may not have a filigree selection
            {
                bool isRare = jewel.IsRareFiligree(i);
                if (isRare)
                {
                    m_imagesFiligreeRare.TransparentBlt(
                            memoryDc.GetSafeHdc(),
                            filigreeLocation.cx,
                            filigreeLocation.cy,
                            34,
                            48);
                }
                else
                {
                    m_imagesFiligree.TransparentBlt(
                            memoryDc.GetSafeHdc(),
                            filigreeLocation.cx,
                            filigreeLocation.cy,
                            34,
                            48);
                }
                CImage image;
                LoadImageFile(IT_augment, filigree.Icon(), &image);
                image.TransparentBlt(
                        memoryDc.GetSafeHdc(),
                        filigreeLocation.cx+1,
                        filigreeLocation.cy+1,
                        32,
                        32);
            }
            else
            {
                // no rare option shown if no filigree selected
                m_imagesJewel.TransparentBlt(
                        memoryDc.GetSafeHdc(),
                        filigreeLocation.cx+1,
                        filigreeLocation.cy+1,
                        34,
                        34);
            }
            // move draw location for next filigree
            if ((i % 5) == 4)
            {
                // start of next row
                filigreeLocation.cx = c_filigreeX;
                filigreeLocation.cy += c_filigreeYOffset;
            }
            else
            {
                // move across one
                filigreeLocation.cx += c_filigreeXOffset;
            }
        }
    }
    // now draw to display
    pdc.BitBlt(
            0,
            0,
            m_bitmapSize.cx,
            m_bitmapSize.cy,
            &memoryDc,
            0,
            0,
            SRCCOPY);
    memoryDc.RestoreDC(-1);
    memoryDc.DeleteDC();
    pdc.RestoreDC(-1);
}

void CInventoryDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
    CDialog::OnLButtonDown(nFlags, point);
    InventorySlotType slotClicked = FindItemByPoint(NULL);
    if (slotClicked != Inventory_Unknown)
    {
        NotifySlotLeftClicked(slotClicked);
    }
}

void CInventoryDialog::OnRButtonDown(UINT nFlags, CPoint point)
{
    CDialog::OnRButtonDown(nFlags, point);
    InventorySlotType slotClicked = FindItemByPoint(NULL);
    if (slotClicked != Inventory_Unknown)
    {
        NotifySlotRightClicked(slotClicked);
    }
}


InventorySlotType CInventoryDialog::FindItemByPoint(CRect * pRect) const
{
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);
    // see if we need to highlight the item under the cursor
    InventorySlotType slot = Inventory_Unknown;
    std::vector<InventoryHitBox>::const_iterator it = m_hitBoxesInventory.begin();
    while (slot == Inventory_Unknown && it != m_hitBoxesInventory.end())
    {
        if ((*it).IsInRect(point))
        {
            // mouse is over this item
            slot = (*it).Slot();
            if (pRect != NULL)
            {
                *pRect = (*it).Rect();
            }
        }
        ++it;
    }
    return slot;
}

bool CInventoryDialog::FindFiligreeByPoint(
        int * filigree,
        CRect * pRect) const
{
    bool found = false;
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);
    size_t count = 0;
    if (m_gearSet.HasItemInSlot(Inventory_Weapon1)
            && m_gearSet.ItemInSlot(Inventory_Weapon1).HasSentientIntelligence())
    {
        SentientJewel jewel = m_gearSet.ItemInSlot(Inventory_Weapon1).SentientIntelligence();
        // see if we need to highlight the item under the cursor
        std::vector<FiligreeHitBox>::const_iterator it = m_hitBoxesFiligrees.begin();
        while (!found && it != m_hitBoxesFiligrees.end())
        {
            if ((*it).IsInRect(point))
            {
                // mouse is over this item
                // but the item must exist (slot always exists for jewel (-1))
                if ((*it).Slot() < (int)jewel.NumFiligrees())
                {
                    *filigree = (*it).Slot();
                    if (pRect != NULL)
                    {
                        *pRect = (*it).Rect();
                    }
                    found = true;
                }
            }
            ++it;
        }
    }
    return found;
}

void CInventoryDialog::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which item the mouse may be over
    CRect itemRect;
    InventorySlotType slot = FindItemByPoint(&itemRect);
    int filigree = 0;
    bool bFiligree = FindFiligreeByPoint(&filigree, &itemRect);
    if (slot != Inventory_Unknown
            && slot != m_tooltipItem)
    {
        // over a new item or a different item
        m_tooltipItem = slot;
        if (m_gearSet.HasItemInSlot(slot))
        {
            ShowTip(m_gearSet.ItemInSlot(slot), itemRect);
        }
    }
    else if (bFiligree && filigree != m_tooltipFiligree)
    {
        m_tooltipFiligree = filigree;
        // over a jewel/filigree item
        SentientJewel jewel = m_gearSet.ItemInSlot(Inventory_Weapon1).SentientIntelligence();
        if (filigree == -1) // jewel
        {
            if (jewel.HasPersonality())
            {
                std::string augmentName = jewel.Personality();
                Augment augment = FindAugmentByName(augmentName);
                ShowTip(augment, itemRect);
            }
        }
        else    // filigree
        {
            std::string filigreeName = jewel.Filigree(filigree);
            if (filigreeName.size() != 0)
            {
                Augment augment = FindAugmentByName(filigreeName);
                ShowTip(augment, itemRect);
            }
        }
    }
    else
    {
        // over neither an item or a filigree
        if ((m_showingItemTip && slot != m_tooltipItem)
                || (m_showingFiligreeTip && filigree != m_tooltipFiligree))
        {
            // no longer over the same item
            HideTip();
            m_tooltipFiligree = -2;
        }
    }
    // as the mouse is over the enhancement tree, ensure the status bar message prompts available actions
    GetMainFrame()->SetStatusBarPromptText("Left click to choose / edit gear");
}

LRESULT CInventoryDialog::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    // hide any tooltip when the mouse leaves the area its being shown for
    HideTip();
    GetMainFrame()->SetStatusBarPromptText("Ready.");
    return 0;
}

void CInventoryDialog::ShowTip(const Item & item, CRect itemRect)
{
    if (m_showingItemTip || m_showingFiligreeTip)
    {
        m_tooltip.Hide();
    }
    ClientToScreen(&itemRect);
    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
    CPoint tipAlternate(itemRect.left, itemRect.top - 2);
    SetTooltipText(item, tipTopLeft, tipAlternate);
    m_showingItemTip = true;
    // track the mouse so we know when it leaves our window
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
}

void CInventoryDialog::ShowTip(const Augment & augment, CRect itemRect)
{
    if (m_showingItemTip || m_showingFiligreeTip)
    {
        m_tooltip.Hide();
    }
    ClientToScreen(&itemRect);
    CPoint tipTopLeft(itemRect.left, itemRect.bottom + 2);
    CPoint tipAlternate(itemRect.left, itemRect.top - 2);
    SetTooltipText(augment, tipTopLeft, tipAlternate);
    m_showingFiligreeTip = true;
    // track the mouse so we know when it leaves our window
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
}

void CInventoryDialog::HideTip()
{
    // tip not shown if not over a gear slot
    if (m_tipCreated && (m_showingItemTip || m_showingFiligreeTip))
    {
        m_tooltip.Hide();
        m_showingItemTip = false;
        m_showingFiligreeTip = false;
        m_tooltipItem = Inventory_Unknown;
    }
}

void CInventoryDialog::SetTooltipText(
        const Item & item,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetItem(&item);
    m_tooltip.Show();
}

void CInventoryDialog::SetTooltipText(
        const Augment & augment,
        CPoint tipTopLeft,
        CPoint tipAlternate)
{
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, false);
    m_tooltip.SetAugment(&augment);
    m_tooltip.Show();
}

CRect CInventoryDialog::GetItemRect(InventorySlotType slot) const
{
    // iterate the list of hit boxes looking for the item that matches
    CRect itemRect(0, 0, 0, 0);
    bool found = false;
    std::vector<InventoryHitBox>::const_iterator it = m_hitBoxesInventory.begin();
    while (!found && it != m_hitBoxesInventory.end())
    {
        if ((*it).Slot() == slot)
        {
            found = true;
            itemRect = (*it).Rect();
            break;
        }
        ++it;
    }
    ASSERT(found);
    return itemRect;
}

void CInventoryDialog::NotifySlotLeftClicked(InventorySlotType slot)
{
    NotifyAll(&InventoryObserver::UpdateSlotLeftClicked, this, slot);
}

void CInventoryDialog::NotifySlotRightClicked(InventorySlotType slot)
{
    NotifyAll(&InventoryObserver::UpdateSlotRightClicked, this, slot);
}

