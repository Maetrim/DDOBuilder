// FeatTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum FeatAcquisitionType
{
    FeatAcquisition_Unknown = 0,
    FeatAcquisition_Automatic,
    FeatAcquisition_HeroicPastLife,
    FeatAcquisition_RacialPastLife,
    FeatAcquisition_IconicPastLife,
    FeatAcquisition_EpicPastLife,
    FeatAcquisition_Special,
    FeatAcquisition_Train,
};
const XmlLib::enumMapEntry<FeatAcquisitionType> featAcquisitionMap[] =
{
    {FeatAcquisition_Unknown, L"Unknown"},
    {FeatAcquisition_Automatic, L"Automatic"},
    {FeatAcquisition_HeroicPastLife, L"HeroicPastLife"},
    {FeatAcquisition_RacialPastLife, L"RacialPastLife"},
    {FeatAcquisition_IconicPastLife, L"IconicPastLife"},
    {FeatAcquisition_EpicPastLife, L"EpicPastLife"},
    {FeatAcquisition_Train, L"Train"},
    {FeatAcquisition_Special, L"Special"},
    {FeatAcquisitionType(0), NULL}
};

enum TrainableFeatTypes
{
    TFT_Unknown,
    TFT_Automatic,
    TFT_Standard,
    TFT_Special,

    TFT_ArtificerBonus,
    TFT_BelovedOf,
    TFT_ChildOf,
    TFT_DamageReduction,
    TFT_Deity,
    TFT_Domain,
    TFT_DomainFeat,
    TFT_DruidWildShape,
    TFT_EnergyResistance,
    TFT_EpicDestinyFeat,
    TFT_EpicFeat,
    TFT_FavoredSoulBattle,
    TFT_FavoredSoulHeart,
    TFT_FighterBonus,
    TFT_FollowerOf,
    TFT_HalfElfDilettanteBonus,
    TFT_HumanBonus,
    TFT_LegendaryFeat,
    TFT_MonkBonus,
    TFT_MonkBonus6,
    TFT_MonkPhilosphy,
    TFT_PDKBonus,
    TFT_RangerFavoredEnemy,
    TFT_RogueSpecialAbility,
    TFT_WarlockPact,
    TFT_WarlockPactAbility,
    TFT_WarlockPactSaveBonus,
    TFT_WarlockPactSpell,
    TFT_WizardMetamagic,
    TFT_DragonbornRacial,
};

const XmlLib::enumMapEntry<TrainableFeatTypes> trainableFeatTypesMap[] =
{
    {TFT_Unknown, L"Unknown"},
    {TFT_Automatic, L"Automatic"},
    {TFT_Standard, L"Standard"},
    {TFT_Special, L"Special"},

    {TFT_ArtificerBonus, L"ArtificerBonus"},
    {TFT_BelovedOf, L"BelovedOf"},
    {TFT_ChildOf, L"ChildOf"},
    {TFT_DamageReduction, L"DamageReduction"},
    {TFT_Deity, L"Deity"},
    {TFT_Domain, L"Domain"},
    {TFT_DomainFeat, L"DomainFeat"},
    {TFT_DragonbornRacial, L"DragonbornRacial"},
    {TFT_DruidWildShape, L"DruidWildShape"},
    {TFT_EnergyResistance, L"EnergyResistance"},
    {TFT_EpicDestinyFeat, L"EpicDestinyFeat"},
    {TFT_EpicFeat, L"EpicFeat"},
    {TFT_FavoredSoulBattle, L"FavoredSoulBattle"},
    {TFT_FavoredSoulHeart, L"FavoredSoulHeart"},
    {TFT_FighterBonus, L"FighterBonus"},
    {TFT_FollowerOf, L"FollowerOf"},
    {TFT_HalfElfDilettanteBonus, L"Dilettante"},
    {TFT_HumanBonus, L"HumanBonus"},
    {TFT_LegendaryFeat, L"Legendary"},
    {TFT_MonkBonus, L"MonkBonus"},
    {TFT_MonkBonus6, L"MonkBonus6"},
    {TFT_MonkPhilosphy, L"MonkPhilosophy"},
    {TFT_PDKBonus, L"PDKBonus"},
    {TFT_RangerFavoredEnemy, L"FavoredEnemy"},
    {TFT_RogueSpecialAbility, L"RogueSpecialAbility"},
    {TFT_WarlockPact, L"WarlockPact"},
    {TFT_WarlockPactAbility, L"WarlockPactAbility"},
    {TFT_WarlockPactSaveBonus, L"WarlockPactSaveBonus"},
    {TFT_WarlockPactSpell, L"WarlockPactSpell"},
    {TFT_WizardMetamagic, L"Metamagic"},
    {TrainableFeatTypes(0), NULL}
};
