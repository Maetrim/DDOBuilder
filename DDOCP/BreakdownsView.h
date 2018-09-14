// BreakdownsView.h
//
#pragma once
#include "Resource.h"

#include "BreakdownItem.h"
#include "Character.h"
#include "TreeListCtrl.h"
#include "GroupLine.h"

class BreakdownItemWeaponEffects;

class CBreakdownsView :
    public CFormView,
    public CharacterObserver
{
    public:
        enum { IDD = IDD_BREAKDOWNS_VIEW };
        DECLARE_DYNCREATE(CBreakdownsView)

#ifdef _DEBUG
        virtual void AssertValid() const;
    #ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif
        virtual void OnInitialUpdate();
        BreakdownItem * FindBreakdown(BreakdownType type) const;
    protected:
        CBreakdownsView();           // protected constructor used by dynamic creation
        virtual ~CBreakdownsView();

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg LRESULT OnNewDocument(WPARAM wParam, LPARAM lParam);
        afx_msg void OnSelChangedBreakdownTree(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
        afx_msg void OnDividerClicked();
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnEndtrackBreakdownList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnButtonClipboardCopy();
        DECLARE_MESSAGE_MAP()
    protected:
        virtual void UpdateGearChanged(Character * pCharData, InventorySlotType slot) override;
    private:
        void CreateBreakdowns();
        void CreateAbilityBreakdowns();
        void CreateSavesBreakdowns();
        void CreateSkillBreakdowns();
        void CreatePhysicalBreakdowns();
        void CreateMagicalBreakdowns();
        void CreateTurnUndeadBreakdowns();
        void CreateEnergyResistancesBreakdowns();
        void CreateWeaponBreakdowns();
        void CreateHirelingBreakdowns();
        void AddCasterLevels(HTREEITEM hParent);
        void AddTacticalItem(BreakdownType bt, TacticalType tt, const std::string & name, HTREEITEM hParent);
        void AddSpellPower(BreakdownType bt, SpellPowerType type, const std::string & name, HTREEITEM hParent);
        void AddSpellCriticalChance(BreakdownType bt, SpellPowerType type, const std::string & name, HTREEITEM hParent);
        void AddSpellCriticalMultiplier(BreakdownType bt, SpellPowerType type, const std::string & name, HTREEITEM hParent);
        void AddSpellSchool(BreakdownType bt, SpellSchoolType type, const std::string & name, HTREEITEM hParent);
        void AddEnergyResistance(BreakdownType bt, EnergyType type, const std::string & name, HTREEITEM hParent);
        void AddEnergyAbsorption(BreakdownType bt, EnergyType type, const std::string & name, HTREEITEM hParent);
        void CalculatePercent(CPoint point);
        Character * m_pCharacter;

        std::vector<BreakdownItem *> m_items;
        MfcControls::CTreeListCtrl m_itemBreakdownTree;
        MfcControls::CGroupLine m_divider;
        CMFCButton m_buttonClipboard;
        CListCtrl m_itemBreakdownList;
        bool m_bDraggingDivider;
        int m_treeSizePercent;             // percentage of space used
        BreakdownItemWeaponEffects * m_pWeaponEffects;
        HTREEITEM m_hWeaponsHeader;
        bool m_bUpdateBreakdowns;
};
