// BreakdownItemCasterLevel.cpp
//
#include "stdafx.h"
#include "BreakdownItemCasterLevel.h"

#include "GlobalSupportFunctions.h"

BreakdownItemCasterLevel::BreakdownItemCasterLevel(
        ClassType classType,
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem),
    m_class(classType)
{
}

BreakdownItemCasterLevel::~BreakdownItemCasterLevel()
{
}

// required overrides
CString BreakdownItemCasterLevel::Title() const
{
    CString text = EnumEntryText(m_class, classTypeMap);
    return text;
}

CString BreakdownItemCasterLevel::Value() const
{
    CString value;
    value.Format(
            "%3d",          // Caster level values are always whole numbers
            (int)Total());
    return value;
}

void BreakdownItemCasterLevel::CreateOtherEffects()
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

bool BreakdownItemCasterLevel::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_CasterLevel
            && effect.Class() == m_class)
    {
        // it is a caster level bonus
        isUs = true;
    }
    return isUs;
}

void BreakdownItemCasterLevel::UpdateClassChanged(Character * charData, ClassType type, size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, type, level);
    if (type == m_class)
    {
        CreateOtherEffects();
        Populate();
    }
}

