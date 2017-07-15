// AbilityTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum AbilityType
{
    Ability_Unknown = 0,
    // these are not alphabetical, but in the order they appear in the UI
    Ability_Strength,
    Ability_Dexterity,
    Ability_Constitution,
    Ability_Intelligence,
    Ability_Wisdom,
    Ability_Charisma,

    Ability_Count,
    Ability_All,
};
const XmlLib::enumMapEntry<AbilityType> abilityTypeMap[] =
{
    {Ability_Unknown, L"Unknown"},
    {Ability_Charisma, L"Charisma"},
    {Ability_Constitution, L"Constitution"},
    {Ability_Dexterity, L"Dexterity"},
    {Ability_Intelligence, L"Intelligence"},
    {Ability_Strength, L"Strength"},
    {Ability_Wisdom, L"Wisdom"},
    {Ability_All, L"All"},
    {AbilityType(0), NULL}
};
