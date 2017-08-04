// SelectionSelectDialog.h
//
#pragma once
#include "Resource.h"
#include <string>
#include "InfoTip.h"
#include "EnhancementSelectionButton.h"

class Character;
class EnhancementTreeItem;

class CSelectionSelectDialog :
        public CDialog
{
    DECLARE_DYNAMIC(CSelectionSelectDialog)

    public:
        CSelectionSelectDialog(
                CWnd * pParent,
                const Character & charData,
                const EnhancementTreeItem & item,
                const std::string & treeName,
                TreeType type);
        virtual ~CSelectionSelectDialog();

        std::string Selection() const;
        size_t Cost() const;

    // Dialog Data
        enum { IDD = IDD_SELECTION_SELECT_DIALOG };

    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

        DECLARE_MESSAGE_MAP()
    private:
        void ShowTip(size_t index, CRect itemRect);
        void HideTip();
        void SetTooltipText(const EnhancementSelection & item, CPoint tipTopLeft, CPoint tipAlternate);
        enum {c_maxSelections = 16};
        const Character & m_charData;
        const EnhancementTreeItem & m_item;
        const std::string & m_treeName;
        TreeType m_type;
        CEnhancementSelectionButton m_buttonOption[c_maxSelections];
        size_t m_costs[c_maxSelections];
        std::string m_selections[c_maxSelections];
        std::string m_selection;
        size_t m_cost;
        CInfoTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        const CWnd * m_pTooltipItem;
};
