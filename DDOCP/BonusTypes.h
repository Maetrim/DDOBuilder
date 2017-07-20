// BonusTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum BonusType
{
    Bonus_Unknown = 0,
    // only the highest of these effects stack if multiple present
    Bonus_artifact,
    Bonus_armor,
    Bonus_competence,
    Bonus_deflection,
    Bonus_enchantment,
    Bonus_enhancement,
    Bonus_equipment,
    Bonus_exceptional,
    Bonus_feat,
    Bonus_guild,
    Bonus_insightful,
    Bonus_implement,
    Bonus_luck,
    Bonus_mythic,
    Bonus_penalty,
    Bonus_profane,
    Bonus_quality,
    Bonus_raging,
    Bonus_reaper,
    Bonus_resistance,
    Bonus_spooky,
    Bonus_unique,
    Bonus_vitality,
};

const XmlLib::enumMapEntry<BonusType> bonusTypeMap[] =
{
    {Bonus_Unknown, L"Unknown"},
    {Bonus_artifact, L"Artifact"},
    {Bonus_armor, L"Armor"},
    {Bonus_competence, L"Competence"},
    {Bonus_deflection, L"Deflection"},
    {Bonus_enchantment, L"Enchantment"},
    {Bonus_enhancement, L"Enhancement"},
    {Bonus_equipment, L"Equipment"},
    {Bonus_exceptional, L"Exceptional"},
    {Bonus_feat, L"Feat"},
    {Bonus_guild, L"Guild"},
    {Bonus_insightful, L"Insightful"},
    {Bonus_implement, L"Implement"},
    {Bonus_luck, L"Luck"},
    {Bonus_mythic, L"Mythic"},
    {Bonus_penalty, L"Penalty"},
    {Bonus_profane, L"Profane"},
    {Bonus_quality, L"Quality"},
    {Bonus_raging, L"Rage"},
    {Bonus_reaper, L"Reaper"},
    {Bonus_resistance, L"Resistance"},
    {Bonus_spooky, L"Spooky"},
    {Bonus_unique, L"Unique"},
    {Bonus_vitality, L"Vitality"},
    {BonusType(0), NULL}
};
