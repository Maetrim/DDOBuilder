// InventoryDialog.h
//

#pragma once

#include "Resource.h"
#include "inventorySlotTypes.h"
#include "InfoTip.h"
#include "Character.h"

class InventoryHitBox
{
    public:
        InventoryHitBox(InventorySlotType slot, const CRect & rect) :
                m_slot(slot), m_rect(rect)
        {
        };
        ~InventoryHitBox() {};

        bool IsInRect(CPoint point) const
        {
            return (m_rect.PtInRect(point) != 0);
        };
        InventorySlotType Slot() const
        {
            return m_slot;
        };
        CRect Rect() const
        {
            return m_rect;
        };
    private:
        InventorySlotType m_slot;
        CRect m_rect;
};
class CInventoryDialog;

// this dialog handles selection of gear
class InventoryObserver :
    public Observer<CInventoryDialog>
{
    public:
        virtual void UpdateSlotLeftClicked(CInventoryDialog * dialog, InventorySlotType slot) {};
        virtual void UpdateSlotRightClicked(CInventoryDialog * dialog, InventorySlotType slot) {};
};

class CInventoryDialog :
    public CDialog,
    public Subject<InventoryObserver>
{
    public:
        CInventoryDialog(CWnd* pParent);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CInventoryDialog)
        enum { IDD = IDD_INVENTORY_SELECTION };
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CInventoryDialog)
        virtual void DoDataExchange(CDataExchange* pDX);
        virtual BOOL OnInitDialog();
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CInventoryDialog)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void RenderInventoryItem(CDC * pDC);
        InventorySlotType FindByPoint(CRect * pRect = NULL) const;
        void ShowTip(const EnhancementTreeItem & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const EnhancementTreeItem & item, CPoint tipTopLeft);
        CRect GetItemRect(InventorySlotType slot) const;
        void NotifySlotLeftClicked(InventorySlotType slot);
        void NotifySlotRightClicked(InventorySlotType slot);
        Character * m_pCharacter;
        CSize m_bitmapSize;
        CBitmap m_cachedDisplay;
        CImage m_imageBackground;
        CImage m_imageBackgroundDisabled;
        std::list<InventoryHitBox> m_hitBoxes;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        InventorySlotType m_tooltipItem;
        InventorySlotType m_selectedItem;
        CImage m_selectedImage;
};

//{{AFX_INSERT_LOCATION}}
