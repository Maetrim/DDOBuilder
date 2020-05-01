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
    ON_CBN_SELENDOK(IDC_COMBO_FILIGREESELECT, OnFiligreeSelectOk)
    ON_CBN_SELENDCANCEL(IDC_COMBO_FILIGREESELECT, OnFiligreeSelectCancel)
    ON_MESSAGE(WM_MOUSEHOVER, OnHoverComboBox)
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
    m_tooltipFiligree(-2),
    m_bIgnoreNextMessage(false),
    m_filigreeIndex(0)
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
    m_hitBoxesInventory.push_back(InventoryHitBox(Inventory_FindItems, CRect(99, 96, 128, 124)));
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

void CInventoryDialog::SetGearSet(
        Character * pCharacter,
        const EquippedGear & gear)
{
    m_pCharacter = pCharacter;
    m_gearSet = gear;
    Invalidate();
}

BOOL CInventoryDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_tooltip.Create(this);
    m_tipCreated = true;

    if (!IsWindow(m_comboFiligreeSelect.GetSafeHwnd()))
    {
        CComboBox test;
        m_comboFiligreeSelect.Create(
                WS_CHILD | WS_VSCROLL | WS_TABSTOP
                | CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS,
                CRect(77,36,119,250),
                this,
                IDC_COMBO_FILIGREESELECT);
        CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
        m_comboFiligreeSelect.SetFont(pDefaultGUIFont, TRUE);
    }

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
                && !CanEquipTo2ndWeapon(m_pCharacter, m_gearSet.ItemInSlot(Inventory_Weapon1)))
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
    // now paint the Jewel and Filigrees
    if (IsWindowEnabled())
    {
        SentientJewel jewel = m_gearSet.SentientIntelligence();
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
            Augment filigree = FindAugmentByName(jewel.GetFiligree(i));
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
    else
    {
        // also check if they clicked on a filigree
        if (IsWindowEnabled())
        {
            SentientJewel jewel = m_gearSet.SentientIntelligence();
            int filigree = 0;
            bool isRareSection = false;
            CRect itemRect;
            bool bFiligree = FindFiligreeByPoint(&filigree, &isRareSection, &itemRect);
            if (bFiligree)
            {
                // if we have a filigree in this location allow it to be changed or
                // toggle the rare state of this filigree
                std::string clickedFiligree = jewel.GetFiligree(filigree);
                if (clickedFiligree.empty())
                {
                    // just allow the user to select a filigree for this position
                    EditFiligree(filigree, itemRect);
                }
                else
                {
                    // if they clicked the rare section, toggle its rare state
                    if (isRareSection)
                    {
                        ToggleRareState(filigree);
                    }
                    else
                    {
                        // allow the user to change this filigree selection
                        EditFiligree(filigree, itemRect);
                    }
                }
            }
        }
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
    else
    {
        // also check if they clicked on a filigree
        if (IsWindowEnabled())
        {
            SentientJewel jewel = m_gearSet.SentientIntelligence();
            int filigree = 0;
            bool isRareSection = false;
            CRect itemRect;
            bool bFiligree = FindFiligreeByPoint(&filigree, &isRareSection, &itemRect);
            if (bFiligree)
            {
                // clear this Filigree/jewel
                if (filigree >= 0)
                {
                    jewel.SetFiligreeRare(filigree, false);
                    jewel.SetFiligree(filigree, "");
                }
                else
                {
                    jewel.SetPersonality("");
                }
                m_gearSet.Set_SentientIntelligence(jewel);
                m_pCharacter->UpdateActiveGearSet(m_gearSet);
                Invalidate();
            }
        }
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
        bool * isRareSection,
        CRect * pRect) const
{
    bool found = false;
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);
    size_t count = 0;
    SentientJewel jewel = m_gearSet.SentientIntelligence();
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
                int yPos = point.y - (*it).Rect().top;
                *isRareSection =  (yPos > 34);  // true if click rare part
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
    return found;
}

void CInventoryDialog::OnMouseMove(UINT nFlags, CPoint point)
{
    // determine which item the mouse may be over
    CRect itemRect;
    InventorySlotType slot = FindItemByPoint(&itemRect);
    int filigree = 0;
    bool isRareSection = false;
    bool bFiligree = FindFiligreeByPoint(&filigree, &isRareSection, &itemRect);
    if (slot != Inventory_Unknown
            && slot != m_tooltipItem
            && slot != Inventory_FindItems)
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
        SentientJewel jewel = m_gearSet.SentientIntelligence();
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
            std::string filigreeName = jewel.GetFiligree(filigree);
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
    SetTooltipText(augment, tipTopLeft, tipAlternate, false);
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
        CPoint tipAlternate,
        bool rightAlign)
{
    m_tooltip.SetOrigin(tipTopLeft, tipAlternate, rightAlign);
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

void CInventoryDialog::EditFiligree(int filigreeIndex, CRect itemRect)
{
    int sel = CB_ERR;   // assume no selection
    m_comboFiligreeSelect.SetImageList(NULL);
    m_comboFiligreeSelect.ResetContent();
    m_filigreeIndex = filigreeIndex;

    SentientJewel jewel = m_gearSet.SentientIntelligence();
    if (filigreeIndex < 0)
    {
        // its the sentient jewel type being edited
        // populate with the list of available personalities
        std::vector<Augment> augments = CompatibleAugments("Personality");
        // build the image list for this control
        BuildImageList(augments);
        m_comboFiligreeSelect.SetImageList(&m_filigreeImagesList);
        // now add the available personalities
        m_comboFiligreeSelect.ResetContent();
        std::string selectedPersonality;
        size_t index = 0;
        std::vector<Augment>::iterator it = augments.begin();
        while (it != augments.end())
        {
            size_t pos = m_comboFiligreeSelect.AddString((*it).Name().c_str());
            m_comboFiligreeSelect.SetItemData(pos, index);
            if (jewel.HasPersonality()
                    && (*it).Name() == jewel.Personality())
            {
                 // this is the selected personality, select it
                sel = index;
            }
            ++it;
            ++index;
        }
    }
    else
    {
        // must be a filigree being edited
        // populate with the list of available filigree minus any selections
        // made in other filigree slots
        std::vector<Augment> augments = CompatibleAugments("Filigree");

        // DDO BUG - It is possible to slot the same filigree multiple times
        // while this is possible in game, it will be allowed here in the planner
        // when the bug is fixed, uncomment the following code.

        //for (size_t fi = 0; fi < MAX_FILIGREE; ++fi)
        //{
        //    // do not remove any selection from current slot
        //    if (fi != filigreeIndex)
        //    {
        //        // get current selection (if any)
        //        std::string filigree = jewel.GetFiligree(fi);
        //        if (filigree != "")
        //        {
        //            // remove it from the augments list
        //            std::vector<Augment>::iterator it = augments.begin();
        //            while (it != augments.end())
        //            {
        //                if ((*it).Name() == filigree)
        //                {
        //                    it = augments.erase(it);
        //                }
        //                else
        //                {
        //                    ++it;
        //                }
        //            }
        //        }
        //    }
        //}
        BuildImageList(augments);
        m_comboFiligreeSelect.SetImageList(&m_filigreeImagesList);

        // now add the filigree names to the combo control
        std::vector<Augment>::const_iterator it = augments.begin();
        size_t fi = 0;
        while (it != augments.end())
        {
            size_t pos = m_comboFiligreeSelect.AddString((*it).Name().c_str());
            m_comboFiligreeSelect.SetItemData(pos, fi);
            if (jewel.GetFiligree(filigreeIndex) == (*it).Name())
            {
                sel = fi;
            }
            ++fi;
            ++it;
        }
    }
    // hide the current tip
    HideTip();
    // position the drop list combo under the filigree slot being trained
    CRect comboRect(itemRect);
    comboRect += CPoint(0, c_filigreeYOffset);
    comboRect.right = comboRect.left + 250;
    comboRect.bottom = comboRect.top + 360;   // 12 items visible in drop list
    m_comboFiligreeSelect.MoveWindow(comboRect);
    m_comboFiligreeSelect.SetCurSel(sel);
    m_comboFiligreeSelect.SetDroppedWidth(350); // wider to show extra text
    // set control visible to the user
    m_comboFiligreeSelect.ShowWindow(SW_SHOW);
    m_comboFiligreeSelect.SetFocus();
    m_comboFiligreeSelect.ShowDropDown();
}

void CInventoryDialog::OnFiligreeSelectOk()
{
    // spell selection complete
    m_comboFiligreeSelect.ShowWindow(SW_HIDE); // hide control
    int sel = m_comboFiligreeSelect.GetCurSel();
    if (sel != CB_ERR)
    {
        // get the true index of the item as combo is sorted
        sel = m_comboFiligreeSelect.GetItemData(sel);
        CString selectedItem;
        m_comboFiligreeSelect.GetLBText(sel, selectedItem);
        SentientJewel jewel = m_gearSet.SentientIntelligence();
        if (m_filigreeIndex < 0)
        {
            // if they selected "No Augment" then clear the selection
            if (selectedItem == " No Augment")
            {
                jewel.SetPersonality("");
            }
            else
            {
                // its the personality thats been changed
                jewel.SetPersonality((LPCTSTR)selectedItem);
            }
        }
        else
        {
            // if they selected "No Augment" then clear the selection
            if (selectedItem == " No Augment")
            {
                jewel.SetFiligreeRare(m_filigreeIndex, false);
                jewel.SetFiligree(m_filigreeIndex, "");
            }
            else
            {
                jewel.SetFiligree(m_filigreeIndex, (LPCTSTR)selectedItem);
            }
        }
        m_gearSet.Set_SentientIntelligence(jewel);
        m_pCharacter->UpdateActiveGearSet(m_gearSet);
        Invalidate();
    }
    HideTip();
}

void CInventoryDialog::OnFiligreeSelectCancel()
{
    // spell selection aborted, just hide the control
    m_comboFiligreeSelect.ShowWindow(SW_HIDE);
    HideTip();
    m_bIgnoreNextMessage = true;
}

LRESULT CInventoryDialog::OnHoverComboBox(WPARAM wParam, LPARAM lParam)
{
    if (!m_bIgnoreNextMessage)
    {
        // wParam = selected index
        // lParam = control ID
        // as these are all augment combo boxes, we can treat them all the same
        if (m_showingFiligreeTip)
        {
            m_tooltip.Hide();
        }
        if (wParam >= 0)
        {
            // we have a selection, get the filigree name
            CString augmentName;
            CWnd * pWnd = GetDlgItem(lParam);
            CComboBox * pCombo =  dynamic_cast<CComboBox*>(pWnd);
            pCombo->GetLBText(wParam, augmentName);
            if (!augmentName.IsEmpty())
            {
                CRect rctWindow;
                pCombo->GetWindowRect(&rctWindow);
                rctWindow.right = rctWindow.left + pCombo->GetDroppedWidth();
                // tip is shown to the left or the right of the combo box
                CPoint tipTopLeft(rctWindow.left, rctWindow.top);
                CPoint tipAlternate(rctWindow.right, rctWindow.top);
                Augment augment = FindAugmentByName((LPCTSTR)augmentName);
                SetTooltipText(augment, tipTopLeft, tipAlternate, true);
                m_showingFiligreeTip = true;
            }
        }
    }
    m_bIgnoreNextMessage = false;
    return 0;
}

void CInventoryDialog::OnCancel()
{
    // do nothing to stop dialog being dismissed
}

void CInventoryDialog::BuildImageList(const std::vector<Augment> & augments)
{
    m_filigreeImagesList.DeleteImageList();
    m_filigreeImagesList.Create(
            32,             // all icons are 32x32 pixels
            32,
            ILC_COLOR32,
            0,
            augments.size());
    std::vector<Augment>::const_iterator it = augments.begin();
    while (it != augments.end())
    {
        (*it).AddImage(&m_filigreeImagesList);
        ++it;
    }
}

void CInventoryDialog::ToggleRareState(int filigree)
{
    SentientJewel jewel = m_gearSet.SentientIntelligence();
    jewel.SetFiligreeRare(filigree, !jewel.IsRareFiligree(filigree));
    m_gearSet.Set_SentientIntelligence(jewel);
    m_pCharacter->UpdateActiveGearSet(m_gearSet);
    Invalidate();
}

