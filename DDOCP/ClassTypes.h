// ClassTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum ClassType
{
    Class_Unknown = 0,
    Class_Artificer,
    Class_Barbarian,
    Class_Bard,
    Class_Cleric,
    Class_Druid,
    Class_Epic,
    Class_FavoredSoul,
    Class_Fighter,
    Class_Monk,
    Class_Paladin,
    Class_Ranger,
    Class_Rogue,
    Class_Sorcerer,
    Class_Warlock,
    Class_Wizard,
    Class_Count,
    Class_All,
    Class_NotEpic,
};
const XmlLib::enumMapEntry<ClassType> classTypeMap[] =
{
    {Class_Unknown, L"Unknown"},
    {Class_Artificer, L"Artificer"},
    {Class_Barbarian, L"Barbarian"},
    {Class_Bard, L"Bard"},
    {Class_Cleric, L"Cleric"},
    {Class_Druid, L"Druid"},
    {Class_Epic, L"Epic"},
    {Class_FavoredSoul, L"Favored Soul"},
    {Class_Fighter, L"Fighter"},
    {Class_Monk, L"Monk"},
    {Class_Paladin, L"Paladin"},
    {Class_Ranger, L"Ranger"},
    {Class_Rogue, L"Rogue"},
    {Class_Sorcerer, L"Sorcerer"},
    {Class_Warlock, L"Warlock"},
    {Class_Wizard, L"Wizard"},
    {Class_All, L"All"},
    {Class_NotEpic, L"NotEpic"},
    {ClassType(0), NULL}
};
