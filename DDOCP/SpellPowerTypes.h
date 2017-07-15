// SpellPowerTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types
enum SpellPowerType
{
    SpellPower_Unknown = 0,
    SpellPower_Acid,
    SpellPower_Alignment,
    SpellPower_All,
    SpellPower_Cold,
    SpellPower_Earth,
    SpellPower_Electric,
    SpellPower_Fire,
    SpellPower_Force,
    SpellPower_Light,
    SpellPower_Negative,
    SpellPower_Positive,
    SpellPower_Repair,
    SpellPower_Rust,
    SpellPower_Sonic,
    SpellPower_Water,
};
const XmlLib::enumMapEntry<SpellPowerType> spellPowerTypeMap[] =
{
    {SpellPower_Unknown, L"Unknown"},
    {SpellPower_Acid, L"Acid"},
    {SpellPower_Alignment, L"Alignment"},
    {SpellPower_All, L"All"},
    {SpellPower_Cold, L"Cold"},
    {SpellPower_Earth, L"Earth"},
    {SpellPower_Electric, L"Electric"},
    {SpellPower_Fire, L"Fire"},
    {SpellPower_Force, L"Force"},
    {SpellPower_Light, L"Light"},
    {SpellPower_Negative, L"Negative"},
    {SpellPower_Positive, L"Positive"},
    {SpellPower_Repair, L"Repair"},
    {SpellPower_Rust, L"Rust"},
    {SpellPower_Sonic, L"Sonic"},
    {SpellPower_Water, L"Water"},
    {SpellPowerType(0), NULL}
};
