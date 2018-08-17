// InfoTip.h
//
#pragma once
#include <vector>
#include "ClassTypes.h"

class Augment;
class Character;
class EnhancementSelection;
class EnhancementTreeItem;
class Feat;
class Item;
class Stance;
class LevelTraining;

class CInfoTip :
        public CWnd
{
    public:
        CInfoTip();
        virtual ~CInfoTip();

        BOOL Create(CWnd * parent);

        void SetEnhancementTreeItem(
                const Character & charData,
                const EnhancementTreeItem * pItem,
                const std::string & selection,
                size_t spentInTree);
        void SetEnhancementSelectionItem(
                const Character & charData,
                const EnhancementTreeItem * pItem,
                const EnhancementSelection * pSelection,
                size_t ranks);
        void SetFeatItem(
                const Character & charData,
                const Feat * pItem,
                bool featSwapWarning,
                size_t level,
                bool grantedFeat);
        void SetStanceItem(
                const Character & charData,
                const Stance * pItem);
        void SetItem(
                const Item * pItem);
        void SetAugment(
                const Augment * pAugment);
        void SetLevelItem(
                const Character & charData,
                size_t level,
                const LevelTraining * levelData,
                ClassType expectedClass);
        void SetSelfBuff(const std::string & name);

        void SetOrigin(CPoint origin, CPoint alternate, bool rightAlign);
        void Show();
        void Hide();

    protected:
        BOOL GetWindowSize(CDC * pDC, CSize * size);
        //{{AFX_MSG(CInfoTip)
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
        CString m_title;
        CString m_description;
        std::vector<CString> m_requirements;
        std::vector<bool> m_bRequirementMet;
        std::vector<CString> m_effectDescriptions;
        CString m_cost;
        CString m_ranks;
};