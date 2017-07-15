// WeaponDamageTypes.h
//
#pragma once

#include "XmlLib\SaxContentElement.h"

// Special note - The  "Unknown" entries of each enum must be entry 0 in the
// enum map as this is used by the SAX loaders to check for badly loaded values
// these enumerations are used across multiple object types

enum WeaponDamageType
{
    WeaponDamage_Unknown = 0,
    WeaponDamage_Bludgeoning,
    WeaponDamage_Piercing,
    WeaponDamage_Ranged,
    WeaponDamage_Thrown,
    WeaponDamage_Slashing,
    WeaponDamage_All,
};
const XmlLib::enumMapEntry<WeaponDamageType> weaponDamageTypeMap[] =
{
    {WeaponDamage_Unknown, L"Unknown"},
    {WeaponDamage_Bludgeoning, L"Bludgeoning"},
    {WeaponDamage_Piercing, L"Piercing"},
    {WeaponDamage_Ranged, L"Ranged"},
    {WeaponDamage_Thrown, L"Thrown"},
    {WeaponDamage_Slashing, L"Slashing"},
    {WeaponDamage_All, L"All"},
    {WeaponDamageType(0), NULL}
};
