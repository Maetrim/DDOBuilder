// SpellsControl.h
//

#pragma once

#include "SpellTip.h"
#include "Character.h"
#include "ComboBoxTooltip.h"

class SpellHitBox
{
    public:
        SpellHitBox(size_t spellLevel, int spellindex, const CRect & rect) :
                m_spellLevel(spellLevel), m_spellIndex(spellindex), m_rect(rect)
        {
        };
        ~SpellHitBox() {};

        bool IsInRect(CPoint point) const
        {
            return (m_rect.PtInRect(point) != 0);
        };
        size_t SpellLevel() const
        {
            return m_spellLevel;
        };
        int SpellIndex() const
        {
            return m_spellIndex;
        };
        CRect Rect() const
        {
            return m_rect;
        };
    private:
        size_t m_spellLevel;
        int m_spellIndex;   // negative for fixed spells
        CRect m_rect;
};

class FixedSpell
{
    public:
        FixedSpell(const std::string & name, size_t level) :
                m_spellName(name), m_spellLevel(level)
        {
        };
        ~FixedSpell() {};

        const std::string & Name() const
        {
            return m_spellName;
        }

        size_t Level() const
        {
            return m_spellLevel;
        }
        bool operator==(const FixedSpell & other) const
        {
            return (m_spellName == other.m_spellName)
                    && (m_spellLevel == other.m_spellLevel);
        }
    private:
        std::string m_spellName;
        size_t m_spellLevel;
};

class CSpellsControl :
    public CStatic
{
    public:
        CSpellsControl();
        virtual ~CSpellsControl();

        void SetCharacter(Character * pCharacter, ClassType ct);
        void SetTrainableSpells(const std::vector<size_t> & spellsPerLevel);
        void UpdateSpells();
        void AddFixedSpell(const std::string & spellName, size_t level);
        void RevokeFixedSpell(const std::string & spellName, size_t level);

        CSize RequiredSize();

    protected:
        //{{AFX_VIRTUAL(CSpellsControl)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CSpellsControl)
        afx_msg void OnPaint();
        afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam) ;
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnSpellSelectOk();
        afx_msg void OnSpellSelectCancel();
        afx_msg LRESULT OnHoverComboBox(WPARAM wParam, LPARAM lParam);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        static void InitialiseStaticImages();
        const SpellHitBox * FindByPoint(CRect * pRect = NULL) const;
        void ShowTip(const SpellHitBox & item, CRect itemRect);
        void HideTip();
        void SetTooltipText(const SpellHitBox & item, CPoint tipTopLeft, CPoint tipAlternate);
        void RemoveTrained(
                std::vector<Spell> * spells,
                std::string * currentSelection);
        void ApplySpellEffects(const std::list<TrainedSpell> & spells);
        void RevokeSpellEffects(const std::list<TrainedSpell> & spells);
        size_t CasterLevel() const;
        void ProcessScrollBars(int cx, int cy);

        Character * m_pCharacter;
        ClassType m_class;
        CSize m_bitmapSize;
        CBitmap m_cachedDisplay;
        std::list<SpellHitBox> m_hitBoxes;
        CSpellTip m_tooltip;
        bool m_showingTip;
        bool m_tipCreated;
        std::vector<size_t> m_spellsPerLevel;
        const SpellHitBox * m_pTooltipItem;
        std::list<TrainedSpell> m_trainedSpells[MAX_SPELL_LEVEL];
        std::list<FixedSpell> m_fixedSpells[MAX_SPELL_LEVEL];
        CComboBoxTooltip m_comboSpellSelect;
        CImageList m_spellImagesList;
        size_t m_editSpellLevel;
        size_t m_editSpellIndex;

        CRect m_clientSize;
        CScrollBar m_scrollHorizontal;
        CScrollBar m_scrollVertical;

        bool m_bHVisible;
        bool m_bVVisible;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
