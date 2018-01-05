// Effect.cpp
//
#include "StdAfx.h"
#include "Effect.h"
#include "XmlLib\SaxWriter.h"
#include "GlobalSupportFunctions.h"

#define DL_ELEMENT Effect

namespace
{
    const wchar_t f_saxElementName[] = L"Effect";
    DL_DEFINE_NAMES(Effect_PROPERTIES)

    const unsigned f_verCurrent = 1;
}

Effect::Effect() :
    XmlLib::SaxContentElement(f_saxElementName, f_verCurrent)
{
    DL_INIT(Effect_PROPERTIES)
}

DL_DEFINE_ACCESS(Effect_PROPERTIES)

XmlLib::SaxContentElementInterface * Effect::StartElement(
        const XmlLib::SaxString & name,
        const XmlLib::SaxAttributes & attributes)
{
    XmlLib::SaxContentElementInterface * subHandler =
            SaxContentElement::StartElement(name, attributes);

    DL_START(Effect_PROPERTIES)

    return subHandler;
}

void Effect::EndElement()
{
    SaxContentElement::EndElement();
    DL_END(Effect_PROPERTIES)
}

void Effect::Write(XmlLib::SaxWriter * writer) const
{
    writer->StartElement(ElementName());//, VersionAttributes());
    DL_WRITE(Effect_PROPERTIES)
    writer->EndElement();
}

double Effect::Amount(size_t tier) const
{
    double amount = 0;
    if (HasAmountVector())
    {
        ASSERT(tier <= m_AmountVector.size());
        amount = m_AmountVector.at(tier-1);     // 0 based vs 1 based
    }
    else
    {
        ASSERT(HasAmount());
        ASSERT(tier == 1);
        amount = Amount();
    }
    return amount;
}

bool Effect::VerifyObject(std::stringstream * ss) const
{
    bool ok = true;
    switch (m_Type)
    {
        case Effect_Unknown:
            (*ss) << "Has unknown effect type\n";
            ok = false;
            break;
        case Effect_AbilityBonus:
            if (!HasAbility())
            {
                (*ss) << "Ability effect missing ability field\n";
                ok = false;
            }
            else if (Ability() == Ability_Unknown)
            {
                (*ss) << "Ability effect has bad enum value\n";
                ok = false;
            }
            break;
        case Effect_SaveBonus:
            if (!HasSave())
            {
                (*ss) << "SaveBonus effect missing save field\n";
                ok = false;
            }
            else if (Save() == Save_Unknown)
            {
                (*ss) << "Save effect has bad enum value\n";
                ok = false;
            }
            break;
        case Effect_SkillBonus:
            if (!HasSkill()
                    && !HasAbility())
            {
                (*ss) << "Skill effect missing skill field\n";
                ok = false;
            }
            else if ((HasSkill() && Skill() == Skill_Unknown)
                    || (HasAbility() && Ability() == Ability_Unknown))
            {
                (*ss) << "Skill effect has bad enum value\n";
                ok = false;
            }
            break;
        case Effect_TacticalDC:
            if (!HasTactical())
            {
                (*ss) << "TacticalDC effect missing tactical field\n";
                ok = false;
            }
            else if (Tactical() == Tactical_Unknown)
            {
                (*ss) << "Tactical effect has bad enum value\n";
                ok = false;
            }
            break;
        case Effect_SpellDC:
            if (!HasSchool())
            {
                (*ss) << "SpellDC effect missing school field\n";
                ok = false;
            }
            else if (School() == SpellSchool_Unknown)
            {
                (*ss) << "School effect has bad enum value\n";
                ok = false;
            }
            break;
        case Effect_SpellPower:
            if (!HasSpellPower())
            {
                (*ss) << "SpellPower effect missing school field\n";
                ok = false;
            }
            else if (SpellPower() == SpellPower_Unknown)
            {
                (*ss) << "SpellPower effect has bad enum value\n";
                ok = false;
            }
            break;
        case Effect_AttackBonus:
        case Effect_AttackSpeed:
        case Effect_CenteredWeapon:
        case Effect_CriticalAttackBonus:
        case Effect_CriticalMultiplier:
        case Effect_CriticalRange:
        case Effect_DamageBonus:
        case Effect_EnchantWeapon:
        case Effect_Seeker:
        case Effect_VorpalRange:
        case Effect_WeaponBaseDamageBonus:
        case Effect_WeaponDamageBonus:
        case Effect_WeaponEnchantment:
        case Effect_WeaponProficiency:
            if (!HasWeapon()
                    && !HasWeaponClass()
                    && !HasDamageType()
                    && !HasStyle())
            {
                (*ss) << "Weapon effect missing Weapon/Class/DamageType field\n";
                ok = false;
            }
            if (HasWeapon() && Weapon() == Weapon_Unknown)
            {
                (*ss) << "Weapon effect has bad enum value\n";
                ok = false;
            }
            if (HasWeaponClass() && WeaponClass() == WeaponClass_Unknown)
            {
                (*ss) << "WeaponClass effect has bad enum value\n";
                ok = false;
            }
            if (HasDamageType() && DamageType() == WeaponDamage_Unknown)
            {
                (*ss) << "DamageType effect has bad enum value\n";
                ok = false;
            }
            if (HasStyle() && Style() == MeleeStyle_Unknown)
            {
                (*ss) << "Style effect has bad enum value\n";
                ok = false;
            }
            break;
        case Effect_SpellLikeAbility:
            if (!HasSpellLikeAbility())
            {
                (*ss) << "SpellLikeAbility effect missing SpellLikeAbility field\n";
                ok = false;
            }
            else if (FindSpellByName(SpellLikeAbility()).Name() == "")
            {
                (*ss) << "SpellLikeAbility field missing from Spells.xml\n";
                ok = false;
            }
            break;
    }
    if (HasAmountVector())
    {
        // realize the vector to catch size/data mismatches
        std::vector<double> d = m_AmountVector;
    }
    return ok;
}

bool Effect::operator==(const Effect & other) const
{
    return (m_hasDisplayName == other.m_hasDisplayName)
            && (m_DisplayName == other.m_DisplayName)
            && (m_Type == other.m_Type)
            && (m_Bonus == other.m_Bonus)
            && (m_hasAmount == other.m_hasAmount)
            && (m_Amount == other.m_Amount)
            && (m_hasAmountVector == other.m_hasAmountVector)
            && (m_AmountVector == other.m_AmountVector)
            && (m_hasAmountPerLevel == other.m_hasAmountPerLevel)
            && (m_AmountPerLevel == other.m_AmountPerLevel)
            && (m_hasAmountPerAP == other.m_hasAmountPerAP)
            && (m_AmountPerAP == other.m_AmountPerAP)
            && (m_hasPercent == other.m_hasPercent)
            && (m_hasDiceRoll == other.m_hasDiceRoll)
            && (m_DiceRoll == other.m_DiceRoll)
            && (m_hasDieRoll == other.m_hasDieRoll)
            && (m_DieRoll == other.m_DieRoll)
            && (m_hasDivider == other.m_hasDivider)
            && (m_Divider == other.m_Divider)
            && (m_hasBase == other.m_hasBase)
            && (m_Base == other.m_Base)
            && (m_hasFeat == other.m_hasFeat)
            && (m_Feat == other.m_Feat)
            && (m_hasNoFailOn1 == other.m_hasNoFailOn1)
            && (m_NoFailOn1 == other.m_NoFailOn1)
            && (m_hasSpell == other.m_hasSpell)
            && (m_Spell == other.m_Spell)
            && (m_hasSpellLevel == other.m_hasSpellLevel)
            && (m_SpellLevel == other.m_SpellLevel)
            && (m_Stance == other.m_Stance)
            && (m_hasEnhancementTree == other.m_hasEnhancementTree)
            && (m_EnhancementTree == other.m_EnhancementTree)
            && (m_hasSpellLikeAbility == other.m_hasSpellLikeAbility)
            && (m_SpellLikeAbility == other.m_SpellLikeAbility)
            && (m_hasImmunity == other.m_hasImmunity)
            && (m_Immunity == other.m_Immunity)
            && (m_hasAbility == other.m_hasAbility)
            && (m_Ability == other.m_Ability)
            && (m_hasClass == other.m_hasClass)
            && (m_Class == other.m_Class)
            && (m_DR == other.m_DR)
            && (m_hasEnergy == other.m_hasEnergy)
            && (m_Energy == other.m_Energy)
            && (m_hasFavoredEnemy == other.m_hasFavoredEnemy)
            && (m_FavoredEnemy == other.m_FavoredEnemy)
            && (m_hasStyle == other.m_hasStyle)
            && (m_Style == other.m_Style)
            && (m_hasSave == other.m_hasSave)
            && (m_Save == other.m_Save)
            && (m_hasSkill == other.m_hasSkill)
            && (m_Skill == other.m_Skill)
            && (m_hasSpellPower == other.m_hasSpellPower)
            && (m_SpellPower == other.m_SpellPower)
            && (m_hasSchool == other.m_hasSchool)
            && (m_School == other.m_School)
            && (m_hasTactical == other.m_hasTactical)
            && (m_Tactical == other.m_Tactical)
            && (m_hasWeaponClass == other.m_hasWeaponClass)
            && (m_WeaponClass == other.m_WeaponClass)
            && (m_hasDamageType == other.m_hasDamageType)
            && (m_DamageType == other.m_DamageType)
            && (m_hasWeapon == other.m_hasWeapon)
            && (m_Weapon == other.m_Weapon);
}
