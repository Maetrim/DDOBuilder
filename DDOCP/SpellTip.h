// SpellTip.h
//
#pragma once
#include "Spell.h"

class Character;

class CSpellTip :
        public CWnd
{
    public:
        CSpellTip();
        virtual ~CSpellTip();

        BOOL Create(CWnd * parent);
        void SetSpell(
                Character * pCharacter,
                const Spell & spell,
                ClassType ct,
                size_t spellLevel,
                size_t maxSpellLevel);

        void SetOrigin(CPoint origin, CPoint alternate, bool rightAlign);
        void Show();
        void Hide();

    protected:
        BOOL GetWindowSize(CDC * pDC, CSize * size);
        //{{AFX_MSG(CSpellTip)
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void GenerateLineBreaks(CString * text);
        CPoint m_origin;
        CPoint m_alternate;
        bool m_bRightAlign;
        CFont m_standardFont;
        CFont m_boldFont;
        CImage m_image;
        Spell m_spell;
        ClassType m_class;
        CRect m_rcDescription;
        CSize m_csMetas;
        size_t m_DC;
        size_t m_spellLevel;
        Character * m_pCharacter;
};