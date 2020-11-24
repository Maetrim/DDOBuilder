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
    SpellPower_Cold,
    SpellPower_Electric,
    SpellPower_Evil,
    SpellPower_Fire,
    SpellPower_ForceUntyped,
    SpellPower_LightAlignment,
    SpellPower_Negative,
    SpellPower_Physical,
    SpellPower_Positive,
    SpellPower_Repair,
    SpellPower_Rust,
    SpellPower_Sonic,
    SpellPower_Poison,
    SpellPower_Last,
    SpellPower_All,
};
const XmlLib::enumMapEntry<SpellPowerType> spellPowerTypeMap[] =
{
    {SpellPower_Unknown, L"Unknown"},
    {SpellPower_Acid, L"Acid"},
    {SpellPower_Cold, L"Cold"},
    {SpellPower_Electric, L"Electric"},
    {SpellPower_Evil, L"Evil"},
    {SpellPower_Fire, L"Fire"},
    {SpellPower_ForceUntyped, L"Force/Untyped"},
    {SpellPower_LightAlignment, L"Light/Alignment"},
    {SpellPower_Negative, L"Negative"},
    {SpellPower_Physical, L"Physical"},
    {SpellPower_Positive, L"Positive"},
    {SpellPower_Repair, L"Repair"},
    {SpellPower_Rust, L"Rust"},
    {SpellPower_Sonic, L"Sonic"},
    {SpellPower_Poison, L"Poison"},
    {SpellPower_All, L"All"},
    {SpellPowerType(0), NULL}
};
