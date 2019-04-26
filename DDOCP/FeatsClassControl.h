#pragma once
#include "Character.h"

enum HitType
{
    HT_Class1 = 0,
    HT_Class2,
    HT_Class3,
};

class HitChecks
{
public:
    HitChecks(HitType type, const CRect & rect) :
        m_type(type), m_rect(rect) {};
     ~HitChecks() {};
private:
    HitType m_type;
    CRect m_rect;
};

class CFeatsClassControl :
    public CWnd,
    public CharacterObserver
{
    DECLARE_DYNAMIC(CFeatsClassControl)

public:
    CFeatsClassControl();
    virtual ~CFeatsClassControl();

    void SetCharacter(Character * pCharacter);

protected:
    afx_msg void OnPaint();
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()

private:
    void SetupControl();
    size_t DrawTopLine(CDC * pDC, CRect rctWindow);

    Character * m_pCharacter;
    bool m_bMenuDisplayed;
    CImageList m_classImagesLarge;
    CImageList m_classImagesSmall;
    CImageList m_imagesFeats;
    std::vector<std::vector<TrainableFeatTypes> > m_availableFeats;
    size_t m_maxRequiredFeats;
    size_t m_numClassColumns;
    std::vector<HitChecks> m_hitChecks;
    CRect m_levelRect;
    CRect m_classRects[3];
    CRect m_featRects[3];
    CRect m_statRects[7];
};


