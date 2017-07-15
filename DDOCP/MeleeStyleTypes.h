// MeleeStyleTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

enum MeleeStyle
{
    MeleeStyle_Unknown = 0,
    MeleeStyle_All,
    MeleeStyle_Unarmed,
    MeleeStyle_SingleWeaponFighting,
    MeleeStyle_SwordAndShield,
    MeleeStyle_TwoWeaponFighting,
    MeleeStyle_TwoHandedFighting,
};
const XmlLib::enumMapEntry<MeleeStyle> meleeStyleMap[] =
{
    {MeleeStyle_Unknown, L"Unknown"},
    {MeleeStyle_All, L"All"},
    {MeleeStyle_Unarmed, L"Unarmed"},
    {MeleeStyle_SingleWeaponFighting, L"SingleWeaponFighting"},
    {MeleeStyle_SwordAndShield, L"SwordAndShield"},
    {MeleeStyle_TwoWeaponFighting, L"TwoWeaponFighting"},
    {MeleeStyle_TwoHandedFighting, L"TwoHandedFighting"},
    {MeleeStyle(0), NULL}
};
