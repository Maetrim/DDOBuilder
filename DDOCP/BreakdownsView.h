// BreakdownsView.h
//
#pragma once
#include "Resource.h"
#include "BreakdownItem.h"
#include "TreeListCtrl.h"
#include "GroupLine.h"

class CBreakdownsView :
    public CFormView
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
        void AddSpellPower(SpellPowerType type, const std::string & name, HTREEITEM hParent);
        void AddSpellCriticalChance(SpellPowerType type, const std::string & name, HTREEITEM hParent);
        void AddSpellCriticalMultiplier(SpellPowerType type, const std::string & name, HTREEITEM hParent);
        void AddSpellSchool(SpellSchoolType type, const std::string & name, HTREEITEM hParent);
        void AddEnergyResistance(EnergyType type, const std::string & name, HTREEITEM hParent);
        void AddEnergyAbsorption(EnergyType type, const std::string & name, HTREEITEM hParent);
        void AddWeapons(HTREEITEM hParent);
        void AddWeapon(WeaponType weaponType, HTREEITEM hParent);
        //void AddWeaponToHitBreakdowns(HTREEITEM hParent);
        //void AddWeaponDamageBreakdowns(HTREEITEM hParent);
        //void AddWeaponToHit(WeaponType weaponType, WeaponClassType weaponClass, WeaponDamageType weaponDamage, HTREEITEM hParent);
        //void AddWeaponDamage(WeaponType weaponType, WeaponClassType weaponClass, WeaponDamageType weaponDamage, HTREEITEM hParent);
        void CalculatePercent(CPoint point);
        Character * m_pCharacter;

        std::vector<BreakdownItem *> m_items;
        MfcControls::CTreeListCtrl m_itemBreakdownTree;
        MfcControls::CGroupLine m_divider;
        CMFCButton m_buttonClipboard;
        CListCtrl m_itemBreakdownList;
        bool m_bDraggingDivider;
        int m_treeSizePercent;             // percentage of space used
};
