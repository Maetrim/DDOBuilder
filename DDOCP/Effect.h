// Effect.h
//
#pragma once
#include "XmlLib\DLMacros.h"
#include "Dice.h"

#include "AbilityTypes.h"
#include "BonusTypes.h"
#include "ClassTypes.h"
#include "DamageReductionTypes.h"
#include "EnergyTypes.h"
#include "FavoredEnemyTypes.h"
#include "InventorySlotTypes.h"
#include "SaveTypes.h"
#include "SkillTypes.h"
#include "SpellPowerTypes.h"
#include "SpellSchoolTypes.h"
#include "TacticalTypes.h"
#include "WeaponClassTypes.h"
#include "WeaponTypes.h"
#include "WeaponDamageTypes.h"

// add new feat effects here and update the map for the text saved for that
// effect type so it will be loaded/saved correctly.
enum EffectType
{
    Effect_Unknown = 0,
    Effect_AbilityBonus,
    Effect_ACBonus,
    Effect_ACBonusShield,
    Effect_ActivateStance,
    Effect_AdditionalSpellPointCost,
    Effect_ArcaneSpellFailure,
    Effect_ArcaneSpellFailureShields,
    Effect_ArmorACBonus,
    Effect_ArmorCheckPenalty,
    Effect_APBonus,
    Effect_Alacrity,
    Effect_Assassinate,
    Effect_AttackBonus,
    Effect_BlockingDR,
    Effect_CasterLevel,
    Effect_CenteredWeapon,
    Effect_CriticalAttackBonus,
    Effect_CriticalMultiplier,
    Effect_CriticalMultiplier19To20,
    Effect_CriticalRange,
    Effect_DamageBonus,
    Effect_DragonmarkUse,
    Effect_DodgeBonus,
    Effect_DodgeBonusTowerShield,
    Effect_DodgeBypass,
    Effect_DodgeCapBonus,
    Effect_DoubleShot,
    Effect_DoubleStrike,
    Effect_DR,
    Effect_DRBypass,
    Effect_EldritchBlast,
    Effect_EnhancementTree,
    Effect_EnchantArmor,
    Effect_EnchantWeapon,
    Effect_EnergyAbsorbance,
    Effect_EnergyResistance,
    Effect_ExcludeFeatSelection,
    Effect_ExtraActionBoost,
    Effect_ExtraLayOnHands,
    Effect_ExtraRage,
    Effect_ExtraRemoveDisease,
    Effect_ExtraSmite,
    Effect_ExtraTurns,
    Effect_ExtraWildEmpathy,
    Effect_FatePoint,
    Effect_Fortification,
    Effect_FortificationBypass,
    Effect_GlancingBlows,
    Effect_GlancingBlowsDamage,
    Effect_GlancingBlowEffects,
    Effect_GrantFeat,
    Effect_GrantSpell,
    Effect_Guard,
    Effect_HealingAmplification,
    Effect_HirelingAbilityBonus,
    Effect_HirelingHitpoints,
    Effect_HirelingFortification,
    Effect_HirelingPRR,
    Effect_HirelingMRR,
    Effect_HirelingDodge,
    Effect_HirelingMeleePower,
    Effect_HirelingRangedPower,
    Effect_HirelingSpellPower,
    Effect_Hitpoints,
    Effect_Immunity,
    Effect_Keen,
    Effect_MaxDexBonus,
    Effect_MaxDexBonusTowerShield,
    Effect_MeleePower,
    Effect_MissileDeflection,
    Effect_MissileDeflectionBypass,
    Effect_MRR,
    Effect_MRRCap,
    Effect_MonkWeaponDie,
    Effect_MovementSpeed,
    Effect_NegativeHealingAmplification,
    Effect_OffHandAttackBonus,
    Effect_OffHandDoublestrike,
    Effect_OverrideBAB,
    Effect_PointBlankShotRange,
    Effect_PRR,
    Effect_RangedPower,
    Effect_RangedSneakAttackDamage,
    Effect_RangedSneakAttackRange,
    Effect_Regeneration,
    Effect_RepairAmplification,
    Effect_RustSusceptability,
    Effect_SaveBonus,
    Effect_SecondaryShieldBash,
    Effect_Seeker,
    Effect_ShieldEnchantment,
    Effect_SkillBonus,
    Effect_SneakAttackAttack,
    Effect_SneakAttackDamage,
    Effect_SneakAttackDice,
    Effect_SneakAttackRange,
    Effect_SongBonus,
    Effect_SongDuration,
    Effect_SpellCriticalDamage,
    Effect_SpellDC,
    Effect_SpellLikeAbility,
    Effect_SpellLore,
    Effect_SpellPenetrationBonus,
    Effect_SpellPoints,
    Effect_SpellPower,
    Effect_SpellResistance,
    Effect_TacticalDC,
    Effect_ThreatBonusMelee,
    Effect_ThreatBonusRanged,
    Effect_ThreatBonusSpell,
    Effect_TurnBonus,
    Effect_TurnDiceBonus,
    Effect_TurnLevelBonus,
    Effect_TurnMaxDice,
    Effect_TwistOfFate,
    Effect_UnconsciousRange,
    Effect_VorpalRange,
    Effect_WeaponBaseDamageBonus,
    Effect_WeaponDamageBonus,
    Effect_WeaponOtherDamageBonus,
    Effect_WeaponOtherCriticalDamageBonus,
    Effect_WeaponEnchantment,
    Effect_WeaponProficiency,
};
const XmlLib::enumMapEntry<EffectType> effectTypeMap[] =
{
    {Effect_Unknown, L"Unknown"},
    {Effect_AbilityBonus, L"AbilityBonus"},
    {Effect_ACBonus, L"ACBonus"},
    {Effect_ACBonusShield, L"ACBonusShield"},
    {Effect_ActivateStance, L"ActivateStance"},
    {Effect_AdditionalSpellPointCost, L"AdditionalSpellPointCost"},
    {Effect_ArcaneSpellFailure, L"ArcaneSpellFailure"},
    {Effect_ArcaneSpellFailureShields, L"ArcaneSpellFailureShields"},
    {Effect_ArmorACBonus, L"ArmorACBonus"},
    {Effect_ArmorCheckPenalty, L"ArmorCheckPenalty"},
    {Effect_APBonus, L"APBonus"},
    {Effect_Alacrity, L"Alacrity"},
    {Effect_Assassinate, L"Assassinate"},
    {Effect_AttackBonus, L"AttackBonus"},
    {Effect_BlockingDR, L"BlockingDR"},
    {Effect_CasterLevel, L"CasterLevel"},
    {Effect_CenteredWeapon, L"CenteredWeapon"},
    {Effect_CriticalAttackBonus, L"CriticalAttackBonus"},
    {Effect_CriticalMultiplier, L"CriticalMultiplier"},
    {Effect_CriticalMultiplier19To20, L"CriticalMultiplier19To20"},
    {Effect_CriticalRange, L"CriticalRange"},
    {Effect_DamageBonus, L"DamageBonus"},
    {Effect_DragonmarkUse, L"DragonmarkUse"},
    {Effect_DodgeBonus, L"DodgeBonus"},
    {Effect_DodgeBonusTowerShield, L"DodgeBonusTowerShield"},
    {Effect_DodgeBypass, L"DodgeBypass"},
    {Effect_DodgeCapBonus, L"DodgeCapBonus"},
    {Effect_DoubleShot, L"Doubleshot"},
    {Effect_DoubleStrike, L"Doublestrike"},
    {Effect_DR, L"DR"},
    {Effect_DRBypass, L"DRBypass"},
    {Effect_EldritchBlast, L"EldritchBlast"},
    {Effect_EnhancementTree, L"EnhancementTree"},
    {Effect_EnchantArmor, L"EnchantArmor"},
    {Effect_EnchantWeapon, L"EnchantWeapon"},
    {Effect_EnergyAbsorbance, L"EnergyAbsorbance"},
    {Effect_EnergyResistance, L"EnergyResistance"},
    {Effect_ExcludeFeatSelection, L"ExcludeFeatSelection"},
    {Effect_ExtraActionBoost, L"ExtraActionBoost"},
    {Effect_ExtraLayOnHands, L"ExtraLayOnHands"},
    {Effect_ExtraRage, L"ExtraRage"},
    {Effect_ExtraRemoveDisease, L"ExtraRemoveDisease"},
    {Effect_ExtraSmite, L"ExtraSmite"},
    {Effect_ExtraTurns, L"ExtraTurns"},
    {Effect_ExtraWildEmpathy, L"ExtraWildEmpathy"},
    {Effect_FatePoint, L"FatePoint"},
    {Effect_Fortification, L"Fortification"},
    {Effect_FortificationBypass, L"FortificationBypass"},
    {Effect_GlancingBlows, L"GlancingBlows"},
    {Effect_GlancingBlowsDamage, L"GlancingBlowsDamage"},
    {Effect_GlancingBlowEffects, L"GlancingBlowEffects"},
    {Effect_GrantFeat, L"GrantFeat"},
    {Effect_GrantSpell, L"GrantSpell"},
    {Effect_Guard, L"Guard"},
    {Effect_HealingAmplification, L"HealingAmplification"},
    {Effect_HirelingAbilityBonus, L"HirelingAbilityBonus"},
    {Effect_HirelingHitpoints, L"HirelingHitpoints"},
    {Effect_HirelingFortification, L"HirelingFortification"},
    {Effect_HirelingPRR, L"HirelingPRR"},
    {Effect_HirelingMRR, L"HirelingMRR"},
    {Effect_HirelingDodge, L"HirelingDodge"},
    {Effect_HirelingMeleePower, L"HirelingMeleePower"},
    {Effect_HirelingRangedPower, L"HirelingRangedPower"},
    {Effect_HirelingSpellPower, L"HirelingSpellPower"},
    {Effect_Hitpoints, L"Hitpoints"},
    {Effect_Immunity, L"Immunity"},
    {Effect_Keen, L"Keen"},
    {Effect_MaxDexBonus, L"MaxDexBonus"},
    {Effect_MaxDexBonusTowerShield, L"MaxDexBonusTowerShield"},
    {Effect_MeleePower, L"MeleePower"},
    {Effect_MRR, L"MRR"},
    {Effect_MRRCap, L"MRRCap"},
    {Effect_MissileDeflection, L"MissileDeflection"},
    {Effect_MissileDeflectionBypass, L"MissileDeflectionBypass"},
    {Effect_MonkWeaponDie, L"MonkWeaponDie"},
    {Effect_MovementSpeed, L"MovementSpeed"},
    {Effect_NegativeHealingAmplification, L"NegativeHealingAmplification"},
    {Effect_OffHandAttackBonus, L"OffHandAttackBonus"},
    {Effect_OffHandDoublestrike, L"OffHandDoublestrike"},
    {Effect_OverrideBAB, L"OverrideBAB"},
    {Effect_PointBlankShotRange, L"PointBlankShotRange"},
    {Effect_PRR, L"PRR"},
    {Effect_RangedPower, L"RangedPower"},
    {Effect_RangedSneakAttackDamage, L"RangedSneakAttackDamage"},
    {Effect_RangedSneakAttackRange, L"RangedSneakAttackRange"},
    {Effect_Regeneration, L"Regeneration"},
    {Effect_RepairAmplification, L"RepairAmplification"},
    {Effect_RustSusceptability, L"RustSusceptability"},
    {Effect_SaveBonus, L"SaveBonus"},
    {Effect_SecondaryShieldBash, L"SecondaryShieldBash"},
    {Effect_Seeker, L"Seeker"},
    {Effect_ShieldEnchantment, L"ShieldEnchantment"},
    {Effect_SkillBonus, L"SkillBonus"},
    {Effect_SneakAttackAttack, L"SneakAttackAttack"},
    {Effect_SneakAttackDamage, L"SneakAttackDamage"},
    {Effect_SneakAttackDice, L"SneakAttackDice"},
    {Effect_SneakAttackRange, L"SneakAttackRange"},
    {Effect_SongBonus, L"SongBonus"},
    {Effect_SongDuration, L"SongDuration"},
    {Effect_SpellCriticalDamage, L"SpellCriticalDamage"},
    {Effect_SpellDC, L"SpellDC"},
    {Effect_SpellLikeAbility, L"SpellLikeAbility"},
    {Effect_SpellPenetrationBonus, L"SpellPenetrationBonus"},
    {Effect_SpellLore, L"SpellLore"},
    {Effect_SpellPoints, L"SpellPoints"},
    {Effect_SpellPower, L"SpellPower"},
    {Effect_SpellResistance, L"SpellResistance"},
    {Effect_TacticalDC, L"TacticalDC"},
    {Effect_ThreatBonusMelee, L"ThreatBonusMelee"},
    {Effect_ThreatBonusRanged, L"ThreatBonusRanged"},
    {Effect_ThreatBonusSpell, L"ThreatBonusSpell"},
    {Effect_TurnBonus, L"TurnBonus"},
    {Effect_TurnDiceBonus, L"TurnDiceBonus"},
    {Effect_TurnLevelBonus, L"TurnLevelBonus"},
    {Effect_TurnMaxDice, L"TurnMaxDice"},
    {Effect_TwistOfFate, L"TwistOfFate"},
    {Effect_UnconsciousRange, L"UnconsciousRange"},
    {Effect_VorpalRange, L"VorpalRange"},
    {Effect_WeaponBaseDamageBonus,L"WeaponBaseDamageBonus"},
    {Effect_WeaponDamageBonus, L"WeaponDamageBonus"},
    {Effect_WeaponOtherDamageBonus, L"WeaponOtherDamageBonus"},
    {Effect_WeaponOtherCriticalDamageBonus, L"WeaponOtherCriticalDamageBonus"},
    {Effect_WeaponEnchantment, L"WeaponEnchantment"},
    {Effect_WeaponProficiency, L"WeaponProficiency"},
    {EffectType(0), NULL}
};

class Effect :
    public XmlLib::SaxContentElement
{
    public:
        Effect(void);
        void Write(XmlLib::SaxWriter * writer) const;

        bool IncludesWeapon(WeaponType wt) const;
        bool IncludesSpellPower(SpellPowerType sp) const;
        bool IncludesEnergy(EnergyType energy) const;
        bool IncludesSkill(SkillType skill) const;
        double Amount(size_t tier) const;
        bool VerifyObject(std::stringstream * ss) const;

        bool operator==(const Effect & other) const;

    protected:
        XmlLib::SaxContentElementInterface * StartElement(
                const XmlLib::SaxString & name,
                const XmlLib::SaxAttributes & attributes);

        virtual void EndElement();

        // note that all the optional XML elements can be
        // used by a variety of feat effects as different interpretations
        #define Effect_PROPERTIES(_) \
                DL_OPTIONAL_STRING(_, DisplayName) \
                DL_ENUM(_, EffectType, Type, Effect_Unknown, effectTypeMap) \
                DL_ENUM(_, BonusType, Bonus, Bonus_Unknown, bonusTypeMap) \
                DL_OPTIONAL_SIMPLE(_, double, Amount, 0) \
                DL_OPTIONAL_VECTOR(_, double, AmountVector) \
                DL_OPTIONAL_SIMPLE(_, double, AmountPerLevel, 0.0) \
                DL_OPTIONAL_SIMPLE(_, double, AmountPerAP, 0.0) \
                DL_FLAG(_, Percent) \
                DL_FLAG(_, IsItemSpecific) \
                DL_FLAG(_, CriticalMultiplier) \
                DL_OPTIONAL_OBJECT(_, Dice, DiceRoll) \
                DL_OPTIONAL_SIMPLE(_, int, Divider, 0) \
                DL_OPTIONAL_STRING(_, Feat) \
                DL_OPTIONAL_VECTOR(_, size_t, NoFailOn1) \
                DL_OPTIONAL_STRING(_, Spell) \
                DL_OPTIONAL_SIMPLE(_, int, SpellLevel, 0) \
                DL_STRING_VECTOR(_, Stance) \
                DL_OPTIONAL_STRING(_, EnhancementTree) \
                DL_OPTIONAL_STRING(_, SpellLikeAbility) \
                DL_STRING_LIST(_, Immunity) \
                DL_OPTIONAL_ENUM(_, AbilityType, Ability, Ability_Unknown, abilityTypeMap) \
                DL_OPTIONAL_ENUM(_, AbilityType, FullAbility, Ability_Unknown, abilityTypeMap) \
                DL_OPTIONAL_ENUM(_, ClassType, Class, Class_Unknown, classTypeMap) \
                DL_ENUM_LIST(_, DamageReductionType, DR, DR_Unknown, drTypeMap) \
                DL_ENUM_LIST(_, EnergyType, Energy, Energy_Unknown, energyTypeMap) \
                DL_OPTIONAL_ENUM(_, FavoredEnemyType, FavoredEnemy, FavoredEnemy_Unknown, favoredEnemyTypeMap) \
                DL_OPTIONAL_ENUM(_, SaveType, Save, Save_Unknown, saveTypeMap) \
                DL_ENUM_LIST(_, SkillType, Skill, Skill_Unknown, skillTypeMap) \
                DL_ENUM_LIST(_, SpellPowerType, SpellPower, SpellPower_Unknown, spellPowerTypeMap) \
                DL_OPTIONAL_ENUM(_, SpellSchoolType, School, SpellSchool_Unknown, spellSchoolTypeMap) \
                DL_OPTIONAL_ENUM(_, TacticalType, Tactical, Tactical_Unknown, tacticalTypeMap) \
                DL_OPTIONAL_ENUM(_, WeaponClassType, WeaponClass, WeaponClass_Unknown, weaponClassTypeMap) \
                DL_OPTIONAL_ENUM(_, WeaponDamageType, DamageType, WeaponDamage_Unknown, weaponDamageTypeMap) \
                DL_ENUM_LIST(_, WeaponType, Weapon, Weapon_Unknown, weaponTypeMap)

        DL_DECLARE_ACCESS(Effect_PROPERTIES)
        DL_DECLARE_VARIABLES(Effect_PROPERTIES)

        friend class Character;
        friend class Spell;
        friend class BreakdownItemWeaponEffects;
};
