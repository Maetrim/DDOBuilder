// BreakdownItemTurnUndeadHitDice.cpp
//
#include "stdafx.h"
#include "BreakdownItemTurnUndeadHitDice.h"

#include "BreakdownsView.h"
#include "GlobalSupportFunctions.h"

BreakdownItemTurnUndeadHitDice::BreakdownItemTurnUndeadHitDice(
        BreakdownType type,
        MfcControls::CTreeListCtrl * treeList,
        HTREEITEM hItem) :
    BreakdownItem(type, treeList, hItem)
{
}

BreakdownItemTurnUndeadHitDice::~BreakdownItemTurnUndeadHitDice()
{
}

// required overrides
CString BreakdownItemTurnUndeadHitDice::Title() const
{
    CString text("Turn Undead Hit Dice");
    return text;
}

CString BreakdownItemTurnUndeadHitDice::Value() const
{
    CString value;

    value.Format(
            "2d6 + %d",
            (int)Total());
    return value;
}

void BreakdownItemTurnUndeadHitDice::CreateOtherEffects()
{
    if (m_pCharacter != NULL)
    {
        m_otherEffects.clear();

        // your base level is the higher off:
        // effective cleric level or effective paladin level - 3
        // get the caster level breakdowns for cleric and paladin
        BreakdownItem * pCB = m_pBreakdownView->FindBreakdown(Breakdown_CasterLevel_Cleric);
        pCB->AttachObserver(this);
        BreakdownItem * pPB = m_pBreakdownView->FindBreakdown(Breakdown_CasterLevel_Paladin);
        pPB->AttachObserver(this);
        double clericLevels = pCB->Total();
        double paladinLevels = pPB->Total() - 3;
        if (clericLevels >= paladinLevels)
        {
            ActiveEffect cl(
                    Bonus_class,
                    "Cleric Levels",
                    1,
                    clericLevels,
                    "");        // no tree
            AddOtherEffect(cl);
        }
        else
        {
            ActiveEffect pl(
                    Bonus_class,
                    "Effective Cleric Levels (Paladin-3)",
                    1,
                    paladinLevels,
                    "");        // no tree
            AddOtherEffect(pl);
        }

        // also includes your charisma modifier
        BreakdownItem * pBI = m_pBreakdownView->FindBreakdown(StatToBreakdown(Ability_Charisma));
        ASSERT(pBI != NULL);
        pBI->AttachObserver(this);  // need to know about changes
        int bonus = BaseStatToBonus(pBI->Total());
        if (bonus != 0) // only add to list if non zero
        {
            // should now have the best option
            ActiveEffect feat(
                    Bonus_ability,
                    "Ability bonus (Charisma)",
                    1,
                    bonus,
                    "");        // no tree
            feat.SetBreakdownDependency(StatToBreakdown(Ability_Charisma)); // so we know which effect to update
            AddOtherEffect(feat);
        }

        // seek eternal rest gives a stacking +4 (assumed used)
        if (m_pCharacter->IsSpellTrained("Seek Eternal Rest"))
        {
            ActiveEffect ser(
                    Bonus_sacred,
                    "Seek Eternal Rest (Spell Bonus)",
                    1,
                    4,
                    "");        // no tree
            AddOtherEffect(ser);
        }
    }
}

bool BreakdownItemTurnUndeadHitDice::AffectsUs(const Effect & effect) const
{
    // return true if the effect applies to this save
    bool isUs = false;
    if (effect.Type() == Effect_TurnDiceBonus)
    {
        isUs = true;
    }
    return isUs;
}

// BreakdownObserver overrides
void BreakdownItemTurnUndeadHitDice::UpdateTotalChanged(BreakdownItem * item, BreakdownType type)
{
    // a class level that a base effect depends on has changed
    CreateOtherEffects();
    Populate();
}

void BreakdownItemTurnUndeadHitDice::UpdateClassChanged(
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

void BreakdownItemTurnUndeadHitDice::UpdateSpellTrained(
        Character * charData,
        const TrainedSpell & spell)
{
    BreakdownItem::UpdateSpellTrained(charData, spell);
    // need to re-create other effects list
    CreateOtherEffects();
    Populate();
}

void BreakdownItemTurnUndeadHitDice::UpdateSpellRevoked(
        Character * charData,
        const TrainedSpell & spell)
{
    BreakdownItem::UpdateSpellRevoked(charData, spell);
    // need to re-create other effects list
    CreateOtherEffects();
    Populate();
}
