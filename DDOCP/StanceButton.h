// StanceButton.h
//

#pragma once

#include "Resource.h"
#include <string>
#include "Stance.h"

class Character;

// this window handles selection of stances and enhancement sub-option
class CStanceButton :
    public CStatic
{
    public:
        CStanceButton(Character * charData, const Stance & stance);

        void SetSelected(bool selected);
        bool IsSelected() const;
        const Stance & GetStance() const;

        void AddStack();
        void RevokeStack();
        size_t NumStacks() const;
        bool IsYou(const Stance & stance);
        void ShowStacks(bool state);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CStanceButton)
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CStanceButton)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CStanceButton)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        Character * m_pCharacter;
        Stance m_stance;
        bool m_bSelected;
        CImage m_image;
        size_t m_stacks;
        bool m_bShowStacks;
};

//{{AFX_INSERT_LOCATION}}
