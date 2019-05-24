#pragma once
#include "Character.h"

enum HitType
{
    HT_None = 0,
    HT_Class1,
    HT_Class2,
    HT_Class3,
    HT_LevelClass1,
    HT_LevelClass2,
    HT_LevelClass3,
};

class HitChecks
{
public:
    HitChecks(HitType type, const CRect & rect, size_t data) :
        m_type(type), m_rect(rect), m_data(data) {};
     ~HitChecks() {};

     bool PointInRect(const CPoint & pt) const { return (m_rect.PtInRect(pt) != 0); }
     HitType Type() const { return m_type; }
     size_t Data() const { return m_data; }
private:
    HitType m_type;
    CRect m_rect;
    size_t m_data;
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
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg LRESULT UpdateControl(WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()

private:
    void SetupControl();
    size_t DrawTopLine(CDC * pDC, CRect rctWindow);
    size_t DrawLevelLine(CDC * pDC, size_t top, size_t level, CRect rctWindow);
    void DrawFeat(CDC * pDC, CRect rctItem, size_t featIndex, size_t level);
    HitType HitCheck(CPoint mouse, size_t * data) const;
    void DoClass1();
    void DoClass2();
    void DoClass3();
    void SetClassLevel(ClassType ct, size_t level);
    size_t FeatImageIndex(const std::string & name) const;

    // CharacterObserver
    void UpdateClassChoiceChanged(Character * charData) override;
    void UpdateClassChanged(Character * charData, ClassType classFrom, ClassType classTo, size_t level) override;

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
    bool m_bUpdatePending;
    size_t m_highlightedLevelLine;
    CRect m_levelRects[MAX_LEVEL];
};


