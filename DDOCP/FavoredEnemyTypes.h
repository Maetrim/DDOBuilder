// FavoredEnemyTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

// favoured enemies
enum FavoredEnemyType
{
    FavoredEnemy_Unknown = 0,
    FavoredEnemy_Aberration,
    FavoredEnemy_Animal,
    FavoredEnemy_ChaoticOutsider,
    FavoredEnemy_Construct,
    FavoredEnemy_Dragon,
    FavoredEnemy_Dwarf,
    FavoredEnemy_Elemental,
    FavoredEnemy_Elf,
    FavoredEnemy_EvilOutsider,
    FavoredEnemy_Giant,
    FavoredEnemy_Gnoll,
    FavoredEnemy_Goblinoid,
    FavoredEnemy_Halfling,
    FavoredEnemy_Human,
    FavoredEnemy_LawfulOutsider,
    FavoredEnemy_MagicalBeast,
    FavoredEnemy_MonstrousHumanoid,
    FavoredEnemy_Ooze,
    FavoredEnemy_Orc,
    FavoredEnemy_Plant,
    FavoredEnemy_Reptilian,
    FavoredEnemy_Undead,
    FavoredEnemy_Vermin,
};
const XmlLib::enumMapEntry<FavoredEnemyType> favoredEnemyTypeMap[] =
{
    {FavoredEnemy_Unknown, L"Unknown"},
    {FavoredEnemy_Aberration, L"Aberration"},
    {FavoredEnemy_Animal, L"Animal"},
    {FavoredEnemy_ChaoticOutsider, L"Chaotic Outsider"},
    {FavoredEnemy_Construct, L"Construct"},
    {FavoredEnemy_Dragon, L"Dragon"},
    {FavoredEnemy_Dwarf, L"Dwarf"},
    {FavoredEnemy_Elemental, L"Elemental"},
    {FavoredEnemy_Elf, L"Elf"},
    {FavoredEnemy_EvilOutsider, L"Evil Outsider"},
    {FavoredEnemy_Giant, L"Giant"},
    {FavoredEnemy_Gnoll, L"Gnoll"},
    {FavoredEnemy_Goblinoid, L"Goblinoid"},
    {FavoredEnemy_Halfling, L"Halfling"},
    {FavoredEnemy_Human, L"Human"},
    {FavoredEnemy_LawfulOutsider, L"Lawful Outsider"},
    {FavoredEnemy_MagicalBeast, L"Magical Beast"},
    {FavoredEnemy_MonstrousHumanoid, L"Monstrous Humanoid"},
    {FavoredEnemy_Ooze, L"Ooze"},
    {FavoredEnemy_Orc, L"Orc"},
    {FavoredEnemy_Plant, L"Plant"},
    {FavoredEnemy_Reptilian, L"Reptilian"},
    {FavoredEnemy_Undead, L"Undead"},
    {FavoredEnemy_Vermin, L"Vermin"},
    {FavoredEnemyType(0), NULL}
};
