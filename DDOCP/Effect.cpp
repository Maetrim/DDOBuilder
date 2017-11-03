// Effect.cpp
//
#include "StdAfx.h"
#include "Effect.h"
#include "XmlLib\SaxWriter.h"

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
    }
    if (HasAmountVector())
    {
        // realize the vector to catch size/data mismatches
        std::vector<double> d = m_AmountVector;
    }
    return ok;
}

