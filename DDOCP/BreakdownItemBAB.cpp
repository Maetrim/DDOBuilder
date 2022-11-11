// BreakdownItemBAB.cpp
//
#include "stdafx.h"
#include "BreakdownItemBAB.h"

#include "GlobalSupportFunctions.h"

BreakdownItemBAB::BreakdownItemBAB(
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(Breakdown_BAB, treeList, hItem)
{
}

BreakdownItemBAB::~BreakdownItemBAB()
{
}

// required overrides
CString BreakdownItemBAB::Title() const
{
    CString text;
    text = "Base Attack Bonus";
    return text;
}

CString BreakdownItemBAB::Value() const
{
    CString value;

    value.Format(
            "%4d",
            (int)Total());
    return value;
}

void BreakdownItemBAB::CreateOtherEffects()
{
    // add class hit points
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        std::vector<size_t> classLevels = m_pCharacter->ClassLevels(m_pCharacter->MaxLevel());
        for (size_t ci = Class_Unknown; ci < Class_Count; ++ci)
        {
            if (classLevels[ci] > 0)
            {
                std::string className = EnumEntryText((ClassType)ci, classTypeMap);
                className += " Levels";
                double classBab = BAB((ClassType)ci);
                ActiveEffect classBonus(
                        Bonus_class,
                        className,
                        classLevels[ci],
                        classBab,
                        "");        // no tree
                classBonus.SetWholeNumbersOnly();
                AddOtherEffect(classBonus);
            }
        }
        if (classLevels[Class_Epic] > 0)
        {
            int amount = (int)((classLevels[Class_Epic] + 1) / 2);
            std::string className = EnumEntryText(Class_Epic, classTypeMap);
            className += " Levels";
            ActiveEffect epicBonus(
                    Bonus_class,
                    className,
                    1,
                    amount,
                    "");        // no tree
            epicBonus.SetWholeNumbersOnly();
            AddOtherEffect(epicBonus);
        }

        // Override of BAB to 25
        BreakdownItem * pOB = FindBreakdown(Breakdown_OverrideBAB);
        if (pOB != NULL)
        {
            pOB->AttachObserver(this); // need to know about changes
            int overrideCount = static_cast<int>(pOB->Total());
            if (overrideCount != 0)
            {
                size_t currentBab = m_pCharacter->BaseAttackBonus(m_pCharacter->MaxLevel());
                ActiveEffect amountTrained(
                        Bonus_enhancement,
                        "BAB boost to max 25",
                        1,
                        25 - currentBab,
                        "");        // no tree
                AddOtherEffect(amountTrained);
            }
        }
    }
}

bool BreakdownItemBAB::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    return isUs;
}

void BreakdownItemBAB::UpdateClassChanged(
        Character * charData,
        ClassType classFrom,
        ClassType classTo,
        size_t level)
{
    BreakdownItem::UpdateClassChanged(charData, classFrom, classTo, level);
    // need to re-create other effects list
    CreateOtherEffects();
    Populate();
}
