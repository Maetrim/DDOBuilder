// DCButton.h
//

#pragma once

#include "Resource.h"
#include <string>
#include "DC.h"

class Character;

// this window handles selection of stances and enhancement sub-option
class CDCButton :
    public CStatic
{
    public:
        CDCButton(Character * charData, const DC & dc);

        const DC & GetDCItem() const;

        void AddStack();
        void RevokeStack();
        size_t NumStacks() const;
        bool IsYou(const DC & dc);

    ///////////////////////////////////////////////////////////////////
    // MFC

    public:
        //{{AFX_DATA(CDCButton)
        //}}AFX_DATA

    protected:
        //{{AFX_VIRTUAL(CDCButton)
        //}}AFX_VIRTUAL

        //{{AFX_MSG(CDCButton)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        Character * m_pCharacter;
        DC m_dc;
        CImage m_image;
        size_t m_stacks;
};

//{{AFX_INSERT_LOCATION}}
