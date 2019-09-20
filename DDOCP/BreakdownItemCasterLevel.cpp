// BreakdownItemCasterLevel.cpp
//
#include "stdafx.h"
#include "BreakdownItemCasterLevel.h"

#include "GlobalSupportFunctions.h"

BreakdownItemClassCasterLevel::BreakdownItemClassCasterLevel(
        ClassType classType,
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_class(classType)
{
}

BreakdownItemClassCasterLevel::~BreakdownItemClassCasterLevel()
{
}

// required overrides
CString BreakdownItemClassCasterLevel::Title() const
{
    CString text = EnumEntryText(m_class, classTypeMap);
    return text;
}

CString BreakdownItemClassCasterLevel::Value() const
{
    CString value;
    value.Format(
            "%3d",          // Caster level values are always whole numbers
            (int)Total());
    return value;
}

void BreakdownItemClassCasterLevel::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        const Character & charData = *m_pCharacter;
        std::vector<size_t> classLevels = charData.ClassLevels(MAX_LEVEL);
        if (classLevels[m_class] != 0)
        {
            CString text;
            text.Format("%s(%d)",
                    EnumEntryText(m_class, classTypeMap),
                    classLevels[m_class]);
            ActiveEffect levels(
                    Bonus_class,
                    (LPCTSTR)text,
                    1,
                    classLevels[m_class],
                    "");        // no tree
            AddOtherEffect(levels);
        }
    }
}

bool BreakdownItemClassCasterLevel::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_CasterLevel
            && effect.HasClass()
            && effect.Class() == m_class)
    {
        // it is a caster level class bonus
        isUs = true;
    }
    return isUs;
}

void BreakdownItemClassCasterLevel::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    if (classFrom == m_class
            || classTo == m_class)
    {
        CreateOtherEffects();
        Populate();
    }
}

