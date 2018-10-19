// BreakdownItemAC.cpp
//
#include "stdafx.h"
#include "BreakdownItemAC.h"

#include "GlobalSupportFunctions.h"

BreakdownItemAC::BreakdownItemAC(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem)
{
}

BreakdownItemAC::~BreakdownItemAC()
{
}

// required overrides
CString BreakdownItemAC::Title() const
{
    CString text = "AC";
    return text;
}

CString BreakdownItemAC::Value() const
{
    CString value;
    value.Format(
            "%3d",
            (int)Total());
    return value;
}

void BreakdownItemAC::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();
        const Character & charData = *m_pCharacter;
        // need to know about Max Dex bonus, Dexterity, Max Dex Bonus shields
        BreakdownItem * pBIDex = FindBreakdown(StatToBreakdown(Ability_Dexterity));
        pBIDex->AttachObserver(this);  // need to know about changes
        int dexBonus = BaseStatToBonus(pBIDex->Total());
        bool bCappedByMDB = false;
        bool bCappedByMDBShields = false;

        // this value is limited by MDB
        BreakdownItem * pBIMDB = FindBreakdown(Breakdown_MaxDexBonus);
        pBIMDB->AttachObserver(this);
        int mdb = (int)pBIMDB->Total();
        if (dexBonus > mdb)
        {
            bCappedByMDB = true;
            dexBonus = mdb;
        }
        // MDB Shields affects max if a tower shield is in use
        BreakdownItem * pBIMDBShields = FindBreakdown(Breakdown_MaxDexBonusShields);
        pBIMDBShields->AttachObserver(this);
        if (charData.IsStanceActive("Tower Shield"))
        {
            int mdbShields = (int)pBIMDBShields->Total();
            if (dexBonus > mdbShields)
            {
                bCappedByMDBShields = true;
                dexBonus = mdbShields;
            }
        }
        // should now have the best option
        if (!bCappedByMDB && !bCappedByMDBShields)
        {
            ActiveEffect feat(
                    Bonus_ability,
                    "Dex Bonus",
                    1,
                    dexBonus,
                    "");        // no tree
            AddOtherEffect(feat);
        }
        else if (bCappedByMDBShields)
        {
            // capped by shields max dex bonus
            ActiveEffect feat(
                    Bonus_ability,
                    "Dex Bonus (MDB Tower Shield Capped)",
                    1,
                    dexBonus,
                    "");        // no tree
            AddOtherEffect(feat);
        }
        else
        {
            // capped by armor max dex bonus
            ActiveEffect feat(
                    Bonus_ability,
                    "Dex Bonus (MDB Capped)",
                    1,
                    dexBonus,
                    "");        // no tree
            AddOtherEffect(feat);
        }
        // some enhancements provide a bonus to existing totals
        // of either Armor and Shields
        BreakdownItem * pBI = FindBreakdown(Breakdown_BonusArmorAC);
        if (pBI != NULL)
        {
            pBI->AttachObserver(this);      // need to know about changes
            double percentBonus = pBI->Total();
            double value = GetEffectValue(Bonus_armor);
            int amount = (int)((value * percentBonus) / 100.0);
            std::stringstream ss;
            ss << "Armor " << percentBonus << "% Bonus";
            // now add a percentage of that
            ActiveEffect feat(
                    Bonus_enhancement,
                    ss.str(),
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(feat);
        }
        pBI = FindBreakdown(Breakdown_BonusShieldAC);
        if (pBI != NULL)
        {
            pBI->AttachObserver(this);      // need to know about changes
            double percentBonus = pBI->Total();
            double value = GetEffectValue(Bonus_shield);
            int amount = (int)((value * percentBonus) / 100.0);
            std::stringstream ss;
            ss << "Shield " << percentBonus << "% Bonus";
            // now add a percentage of that
            ActiveEffect feat(
                    Bonus_enhancement,
                    ss.str(),
                    1,
                    amount,
                    "");        // no tree
            AddOtherEffect(feat);
        }
    }
}

bool BreakdownItemAC::AffectsUs(const Effect & effect) const
{
    bool isUs = false;
    // see if this effect applies to us
    if (effect.Type() == Effect_ACBonus)
    {
        isUs = true;
    }
    return isUs;
}

void BreakdownItemAC::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    // and also call the base class
    BreakdownItem::UpdateTotalChanged(item, type);
    // our Dex bonus to AC may have changed. Update
    CreateOtherEffects();
}
